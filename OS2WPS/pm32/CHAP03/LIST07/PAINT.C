// PAINT.C - Painting the client window
// Listing 03-07

// Stefano Maruzzi 1992



#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <string.h>

// definitions and macros
#define DISTANCE    30L
#define XSTART      10L
#define YSTART      150L

#define WIDTH   300
#define HEIGHT  280

#define SYS( x) WinQuerySysValue( HWND_DESKTOP, x)
#define PAPA( x) WinQueryWindow( x, QW_PARENT)

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd , ULONG msg, MPARAM mp1, MPARAM mp2) ;



int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "PAINT" ;
    CHAR szWindowTitle[] = "Painting example" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_MENU & ~FCF_ICON & ~FCF_ACCELTABLE & ~FCF_SHELLPOSITION ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinRegisterClass(   hab,
                        szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
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
    CHAR *szString[] = { "Have you ever been to", "Milan,", "Italy, my town?" } ;

    switch( msg)
    {
        case WM_CREATE:
        {
            CHAR szPresParam[] = "10.Courier" ;

            WinSetPresParam(    hwnd,
                                PP_FONTNAMESIZE,
                                sizeof( szPresParam),
                                szPresParam) ;

            WinSetWindowPos(    PAPA( hwnd), HWND_TOP,
                                SYS( SV_CXSCREEN) / 2 - WIDTH /2,
                                SYS( SV_CYSCREEN) / 2 - HEIGHT / 2,
                                WIDTH, HEIGHT,
                                SWP_SIZE | SWP_MOVE | SWP_ACTIVATE) ;
        }
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            POINTL pt = { XSTART, YSTART}, ptl ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            GpiCharStringAt( hps, &pt, strlen( szString[ 0]), szString[ 0]) ;
            pt.y -= DISTANCE ;
            GpiCharStringAt( hps, &pt, strlen( szString[ 1]), szString[ 1]) ;
            GpiQueryCurrentPosition( hps, &ptl) ;
            pt.y -= DISTANCE ;
            GpiCharStringAt( hps, &pt, strlen( szString[ 2]), szString[ 2]) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_BUTTON2DOWN:
        {
            RECTL rc ;
            HPS hps ;
            FONTMETRICS fm ;
 
            hps = WinGetPS( hwnd) ;
            GpiQueryFontMetrics( hps, sizeof( fm), &fm) ;

            rc.xLeft = rc.xRight = XSTART ;
            rc.yBottom = rc.yTop = YSTART - DISTANCE ;
            rc.xRight += fm.lAveCharWidth * strlen( szString[ 1]) ;
            rc.yTop += fm.lMaxBaselineExt ;

            WinDrawBorder( hps, &rc, 2,2, CLR_BLACK, CLR_WHITE, DB_STANDARD) ;
            WinReleasePS( hps) ;

            strcpy( szString[ 1], "Paris,") ;
            strcpy( szString[ 2], "France?") ;

            WinInvalidateRect( hwnd, &rc, TRUE) ;
            WinUpdateWindow( hwnd) ;
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
