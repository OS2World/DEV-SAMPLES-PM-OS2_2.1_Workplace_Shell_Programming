// ENTRY.C - WC_ENTRYFIELD
// Listing 07-08

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <stdio.h>
#include <os2.h>
#include "entry.h"

// macros
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define MENU( x)    WinWindowFromID( x, FID_MENU)
#define HAB( x)     WinQueryAnchorBlock( x)

// new message
#define WM_SETUP WM_USER

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
int main( void) ;


int main( void)
{
    CHAR szClassName[ ] = "ENTRY";
    CHAR szWindowTitle[ ] = "WC_ENTRYFIELD" ;
    HWND hwndFrame, hwndClient ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg;
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_ACCELTABLE & ~FCF_SHELLPOSITION ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE, RS_ALL,
                                    &hwndClient) ;

    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        2, 10, 300, 400,
                        SWP_SIZE | SWP_MOVE | SWP_ACTIVATE) ;

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
    static HWND hwndEField ;
    ULONG ulText ;

    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CREATE:
        {
            ENTRYFDATA efd ;

            efd.cb = sizeof( efd) ;
            efd.cchEditLimit = 6 ;
            efd.ichMinSel = 2 ;
            efd.ichMaxSel = 4 ;
            hwndEField = WinCreateWindow(   hwnd, WC_ENTRYFIELD,
                                            "123456",
                                            0x8a01000c,
                                            10, 10, 120, 20,
                                            hwnd, HWND_TOP,
                                            NULLHANDLE,
                                            &efd,
                                            NULL) ;
            WinSetFocus( HWND_DESKTOP, hwndEField) ;
        }
            break ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                    break ;

                default:
                    break ;
            }

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
