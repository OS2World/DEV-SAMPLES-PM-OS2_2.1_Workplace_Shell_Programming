// TIMER.C - Timer example
// Listing 05-03

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>

// macros
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define HAB( x)         WinQueryAnchorBlock( x)
#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    CHAR szClassName[ ] = "TIMER" ;
    CHAR szWindowTitle[] = "Timer example" ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_ICON ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow(     HWND_DESKTOP,
                                        WS_VISIBLE,
                                        &flFrameFlags,
                                        szClassName,
                                        szWindowTitle,
                                        0L,
                                        NULLHANDLE,
                                        0L,
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

        case WM_BUTTON1DOWN:
            WinStartTimer( HAB( hwnd), hwnd, 1, 2000) ;
            break ;

        case WM_BUTTON2DOWN:
            WinStopTimer( HAB( hwnd), hwnd, 1) ;
            break ;

        case WM_TIMER:
            switch( SHORT1FROMMP( mp1))
            {
                case 1:
                    WinFlashWindow( PAPA( hwnd), TRUE ) ;
                    WinStartTimer( HAB( hwnd), hwnd, 2, 1000) ;
                    break ;

                case 2:
                    WinStopTimer( HAB( hwnd), hwnd, 2) ;
                    WinFlashWindow( PAPA( hwnd), FALSE) ;
                    break ;

                default:
                    break ;
            }
                break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
