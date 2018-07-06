// RTMENU.C - Run-time menu
// Listing 06-10

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "rtmenu.h"

// definitions & macros
#define OFFSET 3

#define PARENT( x)  WinQueryWindow( x, QW_PARENT)
#define MENU( x)    WinWindowFromID( x, FID_MENU)
#define HAB( x)     WinQueryAnchorBlock( x)
#define SYSVAL( x)  WinQuerySysValue( HWND_DESKTOP, x)

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
int main( void) ;
HWND CreateMenu( HWND hwnd) ;


int main( void)
{
    CHAR szClassName[ 15]  ;
    CHAR szWindowTitle[ 25] ;
    HWND hwndFrame, hwndClient ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg;
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_MENU & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load class name & window title from resource file
    WinLoadString(  hab, NULLHANDLE,
                    ST_CLASSNAME,
                    sizeof( szClassName), szClassName) ;

    WinLoadString(  hab, NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof( szWindowTitle), szWindowTitle) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

     hwndFrame = WinCreateStdWindow(    HWND_DESKTOP,
                                        WS_VISIBLE,
                                        &flFrameFlags,
                                        szClassName,
                                        szWindowTitle,
                                        0L,
                                        NULLHANDLE,
                                        RS_ICON,
                                        &hwndClient) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    WinDestroyWindow( hwndFrame) ;
    WinDestroyMsgQueue( hmq) ;
    WinTerminate( hab) ;

    return 0L ;
}
MRESULT EXPENTRY ClientWndProc(    HWND hwnd,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
    static HWND hmenu ;
    static COLOR clr = 1 ;

    switch( msg)
    {
        case WM_CREATE:
            hmenu = CreateMenu( hwnd) ;
            if( !hmenu)
                WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, 0, &rc) ;
            WinFillRect( hps, &rc, clr) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_COMMAND:
            // enable previous color menuitem
            WinSendMsg( hmenu,
                        MM_SETITEMATTR,
                        MPFROM2SHORT( MN_FIRST + clr - 1, TRUE),
                        MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;

            // calculate the client window color
            clr = COMMANDMSG( &msg) -> cmd - MN_FIRST + 1 ;

            // client window invalidation
            WinInvalidateRect( hwnd, NULL, TRUE) ;
            WinUpdateWindow( hwnd) ;
            WinSendMsg( hmenu,
                        MM_ENDMENUMODE,
                        MPFROMSHORT( TRUE),
                        0L) ;
            break ;

        case WM_BUTTON2DOWN:
            // position the popup menu
            WinSetWindowPos(    hmenu,
                                HWND_TOP,
                                SHORT1FROMMP( mp1) - OFFSET,
                                SHORT2FROMMP( mp1) + SYSVAL( SV_CYMENU) + OFFSET,
                                0L, 0L,
                                SWP_MOVE | SWP_SHOW) ;

            // disable the previously chosen color
            WinSendMsg( hmenu,
                        MM_SETITEMATTR,
                        MPFROM2SHORT( MN_FIRST + clr - 1, TRUE),
                        MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;

            // start menu operations
            WinPostMsg( hmenu,
                        MM_STARTMENUMODE,
                        MPFROM2SHORT( TRUE, TRUE),
                        0L) ;

            // select the first available menuitem
            WinSendMsg( hmenu,
                        MM_SELECTITEM,
                        MPFROM2SHORT( MN_FIRST + ((clr == 1) ? (NUMBER - 1) : (clr - 2)), TRUE),
                        MPFROMSHORT( FALSE)) ;
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
HWND CreateMenu( HWND hwnd)
{
    PMENUCREATETEMPLATE pmt ;
    PMITEM pmi ;
    HWND hmenu ;
    PVOID pvoid ;
    CHAR *szItem[] = { "Blue", "Red", "Pink"} ;
    SHORT sID = MN_FIRST - 1, sSub = MN_FIRST ;

    // allocate room to create a menu in RAM
    DosAllocMem( (PPVOID)&pmt, 2 * sizeof( MENUCREATETEMPLATE) +
                    ( (NUMBER + 1) * MENUSTRINGSIZE) +
                    (NUMBER + 1) * sizeof( MITEM), PAG_READ | PAG_WRITE | PAG_COMMIT) ;

    pvoid = pmt ;

    pmt -> size = 0 ;
    pmt -> version = 0 ;
    pmt -> codepage = 0 ;
    pmt -> mnemonic = 0 ;
    pmt -> itemcount = 1 ;

    // use a new pointer to the MITEM structure
    pmi = (PMITEM)++pmt ;

    pmi -> afStyle= MIS_TEXT | MIS_SUBMENU ;
    pmi -> afAttribute = 0 ;
    pmi -> id = sID ;
    strcpy( pmi -> text, "Colors") ;

    // update the pointer
    pmt = (PMENUCREATETEMPLATE)( pmi -> text + strlen( pmi -> text) + 1) ;

    pmt -> size = 0 ;
    pmt -> version = 0 ;
    pmt -> codepage = 0 ;
    pmt -> mnemonic = 0 ;
    pmt -> itemcount = 3 ;

    // use a new pointer to the MITEM structure
    pmi = (PMITEM)++pmt ;

    for( ; sSub < MN_FIRST + NUMBER; sSub++)
    {
        pmi -> afStyle = MIS_TEXT ;
        pmi -> afAttribute = 0 ;
        pmi -> id = sSub ;
        strcpy( pmi -> text, szItem[ sSub - MN_FIRST]) ;
        pmi = (PMITEM)( pmi -> text + strlen( pmi -> text) + 1) ;
    }

    // create drop-down menu
    hmenu = WinCreateWindow(    hwnd, WC_MENU,
                                NULL,
                                MS_ACTIONBAR,
                                0L, 0L, 0L, 0L,
                                hwnd, HWND_TOP,
                                FID_MENU,
                                (PCH)pvoid, NULL) ;

    if( hmenu)
        return hmenu ;

    // error
    return NULLHANDLE ;
}
