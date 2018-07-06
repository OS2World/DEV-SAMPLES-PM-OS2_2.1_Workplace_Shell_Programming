// CLCOLOR.C - Window List content
// Listing 04-04

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_WINSWITCHLIST
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>

#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX


// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "CLCOLOR" ;
    CHAR szTaskTitle[] = "Boca Raton, Fl" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ICON & ~FCF_ACCELTABLE & ~FCF_TASKLIST ;
    ULONG ulApps, ulSize ;
    PID pid ;
    TID tid ;
    SWCNTRL swctl ;
    PSWENTRY p ;
    HSWITCH hswitch ;
    PSWBLOCK pswblk ;
    LONG lIndex, l ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // query Window List
    ulApps = WinQuerySwitchList( hab, NULL, 0) ;

    // calculate memory block size
    ulSize = ulApps * sizeof( SWENTRY) + sizeof( USHORT) ;

    // allocate block of memory
    DosAllocMem( (PPVOID)&pswblk, ulSize, PAG_COMMIT | PAG_READ | PAG_WRITE) ;

    // query Window List
    if( !WinQuerySwitchList( hab, pswblk, ulSize))
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;

    // check string title & Window List entries
    l = pswblk -> cswentry ;
    p = &(pswblk -> aswentry[ 0]) ;
    for( lIndex = 1 ; lIndex < l; lIndex++)
    {
        if( strcmp( p -> swctl.szSwtitle, szTaskTitle) == 0)
        {
            WinAlarm( HWND_DESKTOP, WA_ERROR) ;
            WinMessageBox(  HWND_DESKTOP, HWND_DESKTOP,
                            "Instance active",
                            "Instance management",
                            0L, MB_OK | MB_MOVEABLE) ;

            // free the memory
            DosFreeMem( pswblk) ;

            WinDestroyMsgQueue( hmq) ;
            WinTerminate( hab) ;
            return FALSE ;
        }
        p++ ;
    }

    // register application class
    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName,
                                    NULL,
                                    0L,
                                    NULLHANDLE,
                                    0L,
                                    &hwndClient) ;

    WinQueryWindowProcess( hwndFrame, &pid, &tid) ;
    swctl.hwnd = hwndFrame ;
    swctl.hwndIcon = NULLHANDLE ;
    swctl.hprog = NULLHANDLE ;
    swctl.idSession = 0L ;
    swctl.bProgType = 0L ;
    swctl.idProcess = pid ;
    swctl.uchVisibility = SWL_VISIBLE ;
    swctl.fbJump = SWL_JUMPABLE ;
    strcpy( swctl.szSwtitle, szTaskTitle) ;
    hswitch = WinCreateSwitchEntry( hab, &swctl) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    // free memory block
    DosFreeMem( pswblk) ;

    WinDestroyWindow( hwndFrame) ;
    WinDestroyMsgQueue( hmq) ;
    WinTerminate( hab) ;

    return 0 ;
}

MRESULT EXPENTRY ClientWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static COLOR clrBck = CLR_WHITE ;

    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;
            char buffer[ 30] ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            WinFillRect( hps, &rc, clrBck) ;
            sprintf( buffer, "Color #: %4d", clrBck) ;
            WinSetWindowText( WinQueryWindow( hwnd, QW_PARENT), (PSZ)buffer) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_ACTIVATE:
            if( (BOOL) SHORT1FROMMP( mp1))
            {
                ( clrBck > 14) ? clrBck = CLR_WHITE : clrBck++ ;
                WinInvalidateRect( hwnd, NULL, FALSE) ;
            }
            break ;

        case WM_SIZE:
            ( clrBck > 14) ? clrBck = CLR_WHITE : clrBck++ ;
            break ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
