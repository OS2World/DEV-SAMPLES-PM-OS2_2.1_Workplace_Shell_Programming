// LISTBOX.C - WC_LISTBOX
// Listing 07-05

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include "listbox.h"

// definitions
#define ID_LISTBOX 1
#define ID_STATIC  2

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// macros
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define MENU( x)    WinWindowFromID( x, FID_MENU)
#define TITLE( x)   WinWindowFromID( x, FID_TITLEBAR)
#define HAB( x)     WinQueryAnchorBlock( x)

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
int main( void) ;


int main( void)
{
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 25] ;
    HWND hwndFrame, hwndClient ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg;
    ULONG flFrameFlags = FCF_WPS  ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinLoadString(  hab, NULLHANDLE,
                    ST_CLASSNAME,
                    sizeof szClassName, szClassName) ;
    WinLoadString(  hab, NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof szWindowTitle, szWindowTitle) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName, szWindowTitle,
                                    WS_CLIPCHILDREN,
                                    NULLHANDLE, RS_ALL,
                                    &hwndClient) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    WinDestroyWindow( hwndFrame) ;
    WinDestroyMsgQueue( hmq) ;
    WinTerminate( hab) ;

    return 0L ;
}

MRESULT EXPENTRY ClientWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static HWND hwndListbox, hwndStatic ;

    switch( msg)
    {
        case WM_CREATE:
        {
            short i = ST_MARRIOTT ;
            char szString[ 80] ;

            hwndStatic = WinCreateWindow(   hwnd, WC_STATIC,
                                            NULL,
                                            SS_TEXT | WS_VISIBLE,
                                            10, 140, 280, 22,
                                            hwnd, HWND_TOP,
                                            ID_STATIC,
                                            NULL, NULL) ;

            hwndListbox = WinCreateWindow(  hwnd, WC_LISTBOX,
                                            NULL,
                                            LS_NOADJUSTPOS | WS_VISIBLE,
                                            10, 10, 280, 120,
                                            hwnd, HWND_TOP,
                                            ID_LISTBOX,
                                            NULL, NULL) ;

            while( WinLoadString(   HAB( hwnd), NULLHANDLE, i++,
                                    sizeof szString, szString))
                WinSendMsg( hwndListbox, LM_INSERTITEM,
                            MPFROMSHORT( LIT_SORTASCENDING),
                            MPFROMP( szString)) ;
        }
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case ID_LISTBOX:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case LN_SELECT:
                        {
                            CHAR szString[ 60] ;
                            SHORT sPos ;

                            sPos = (SHORT)WinSendMsg(   hwndListbox, LM_QUERYSELECTION,
                                                        MPFROMSHORT( LIT_FIRST), 0L) ;
                            WinSendMsg( hwndListbox, LM_QUERYITEMTEXT,
                                        MPFROM2SHORT( sPos, sizeof szString),
                                        MPFROMP( (PSZ)szString)) ;
                            WinSetWindowText( hwndStatic, szString) ;
                        }
                            break ;

                        case LN_ENTER:
                            break ;

                        default:
                            break ;
                }
                    break ;

                default:
                    break ;
            }
            break ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> source)
            {
                case CMDSRC_MENU:
                    switch( COMMANDMSG( &msg) -> cmd)
                    {
                        default:
                            break;

                        case MN_PRODINFO:
                            WinMessageBox(  HWND_DESKTOP, hwnd,
                                            "Using a listbox",
                                            "Stefano Maruzzi 1993",
                                            0, MB_OK) ;
                            break ;

                        case MN_EXIT:
                            WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                            break ;
            }
                break ;
        }
        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
