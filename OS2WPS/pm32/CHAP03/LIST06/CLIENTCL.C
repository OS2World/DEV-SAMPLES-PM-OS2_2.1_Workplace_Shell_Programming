// CLIENTCL.C - Painting client window
// Listing 03-06

// Stefano Maruzzi 1992



#define INCL_WIN

#include <os2.h>


// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND, ULONG, MPARAM, MPARAM) ;



int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "CLIENTCOLOR" ;
    CHAR szWindowTitle[] = "Painting the client window" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_MENU & ~FCF_ICON & ~FCF_ACCELTABLE ;

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
    static COLOR clrBck = CLR_WHITE ;

    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;
            char buffer[ 30] ;

            hps = WinBeginPaint( hwnd, 0, &rc) ;
            WinFillRect( hps, &rc, clrBck) ;
            sprintf( buffer, "Color: %4ld", clrBck) ;
            WinSetWindowText( WinQueryWindow( hwnd, QW_PARENT), buffer) ;
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
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
