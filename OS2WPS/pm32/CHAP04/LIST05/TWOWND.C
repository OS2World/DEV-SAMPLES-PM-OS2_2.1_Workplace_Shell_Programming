// TWOWND.C - Overlapped windows
// Listing 04-03

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>

#define HAB( x)         WinQueryAnchorBlock( x)
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX
#define CT_MLE      1000

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
int main( void) ;


int main( void)
{
    CHAR szClassName[] = "TWOWND" ;
    CHAR szWindowTitle[] = "A predefined window overlapping the client" ;
    HWND hwndFrame, hwndClient ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_ICON ;

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
                                    WS_CLIPCHILDREN,
                                    NULLHANDLE,
                                    0L,
                                    &hwndClient) ;

    while( WinGetMsg (hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

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
    static HWND hwndMLE ;

    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break;

        case WM_CREATE:
            hwndMLE = WinCreateWindow(  hwnd, WC_MLE,
                                        NULL,
                                        WS_VISIBLE | MLS_VSCROLL,
                                        0L, 0L, 0L, 0L,
                                        hwnd, HWND_TOP,
                                        CT_MLE,
                                        NULL, NULL) ;

            WinSetFocus( HWND_DESKTOP, hwndMLE) ;
            break ;

        case WM_SIZE:
            WinSetWindowPos(    hwndMLE, HWND_DESKTOP,
                                0L, 0L,
                                SHORT1FROMMP( mp2),
                                SHORT2FROMMP( mp2),
                                SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ACTIVATE) ;
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
