// MOUSE.C - Tracking mouse coordinates & more
// Listing 05-02

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>

// macro
#define PAPA( x) WinQueryWindow( x, QW_PARENT)

#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    CHAR szClassName [] = "MOUSE" ;
    CHAR szWindowTitle[] = "Mouse" ;
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

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName,
                                    NULL,
                                    0L,
                                    NULLHANDLE,
                                    0L,
                                    &hwndClient) ;

    while( WinGetMsg (hab, &qmsg, NULLHANDLE, 0L, 0L))
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
    switch( msg)
    {
        case WM_BUTTON1DOWN:
        case WM_BUTTON2DOWN:
        {
            POINTL ptl ;
            HWND hwndt ;

            ptl.x = SHORT1FROMMP( mp1) ;
            ptl.y = SHORT2FROMMP( mp1) ;

            // map the points for the DESKTOP
            WinMapWindowPoints( hwnd, HWND_DESKTOP, &ptl, 1L) ;
            // determine the selected window
            hwndt = WinWindowFromPoint( HWND_DESKTOP, &ptl, TRUE) ;

            // skip if we are not on a different window or on the desktop
            if( !hwndt || hwndt == HWND_DESKTOP || hwndt == hwnd)
                break ;

            // release the mouse capture
            WinSetCapture( HWND_DESKTOP, 0L) ;

            // map the points for the new window
            WinMapWindowPoints( HWND_DESKTOP, hwndt, &ptl, 1L) ;

            // post the message to the appropriate window
            WinPostMsg( hwndt, msg,
                        MPFROM2SHORT( (SHORT)ptl.x, (SHORT) ptl.y),
                        (MPARAM)0L) ;
        }
            break ;

        case WM_BUTTON1DBLCLK:
            // capture the mouse
            WinSetCapture( HWND_DESKTOP, hwnd) ;
            break ;

        case WM_BUTTON2DBLCLK:
            // release the mouse
            if( WinQueryCapture( HWND_DESKTOP))
                WinSetCapture( HWND_DESKTOP, NULLHANDLE) ;
            break ;

        case WM_MOUSEMOVE:
        {
            CHAR szBuffer[ 30] ;

            sprintf( szBuffer, "Mouse position: x=%3d y=%3d", (short)SHORT1FROMMP( mp1), (short)SHORT2FROMMP( mp1)) ;
            WinSetWindowText( PAPA( hwnd), szBuffer) ;
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

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
