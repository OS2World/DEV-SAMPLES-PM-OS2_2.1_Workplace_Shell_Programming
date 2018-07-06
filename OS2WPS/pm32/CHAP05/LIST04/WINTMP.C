// WINTMP.C - A window from a WINDOWTEMPLATE
// Listing 05-04

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include "wintmp.h"

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "WINTMP" ;
    HMQ hmq ;
    HWND hwndFrame ;
    QMSG qmsg ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinRegisterClass(   hab,
                        szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW,
                        0L) ;

    hwndFrame = WinLoadDlg( HWND_DESKTOP,
                            HWND_DESKTOP,
                            NULL,
                            NULLHANDLE,
                            ID_WINTMP,
                            NULL) ;

    if( !hwndFrame)
        return 0L ;

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
    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            WinFillRect( hps, &rc, CLR_RED) ;
            WinEndPaint( hps) ;
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
