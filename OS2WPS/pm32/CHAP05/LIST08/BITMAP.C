// BITMAP.C - Tracking a bitmap
// Listing 05-08

// Stefano Maruzzi 1993


#define INCL_WIN
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_GPIBITMAPS

#include <os2.h>
#include "bitmap.h"

// definitions
#define STARTX 50L
#define STARTY 50L

// macros
#define HAB( x) WinQueryAnchorBlock( x)
#define SYS( x) WinQuerySysValue( HWND_DESKTOP, x)

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main(void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "BITMAP" ;
    CHAR szWindowTitle[] = "Bitmap" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE, RS_ICON,
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
    static RECTL rc ;
    static POINTL ptl ;
    static HBITMAP hbm ;
    static SHORT cx, cy ;

    switch( msg)
    {
        case WM_CREATE:
        {
            HPS hps ;
            BITMAPINFOHEADER bmp ;

            hps = WinGetPS( hwnd) ;
            hbm = GpiLoadBitmap( hps, 0, ID_BMP, 128L, 128L) ;
            GpiQueryBitmapParameters( hbm, &bmp) ;
            WinReleasePS( hps) ;
            rc.xRight = bmp.cx + STARTX ;
            rc.yTop = bmp.cy + STARTY ;
            rc.xLeft = ptl.x = STARTX ;
            rc.yBottom = ptl.y = STARTY ;
        }
            break ;

        case WM_SIZE:
            cx = SHORT1FROMMP( mp2) ;
            cy = SHORT2FROMMP( mp2) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinDrawBitmap( hps, hbm, NULL, &ptl, CLR_WHITE, CLR_BLACK, DBM_NORMAL) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CHAR:
            if( CHARMSG( &msg) -> fs & KC_CHAR)
            {
                if( CHARMSG( &msg) -> vkey != VK_NEWLINE)
                    break ;

                // simulate the mouse button pressure
                WinSendMsg( hwnd, WM_BUTTON2DOWN, MPFROM2SHORT( rc.xLeft + 1, rc.yBottom + 1), 0L) ;
            }
                break ;

        case WM_BUTTON2DOWN:
        {
            TRACKINFO ti ;

            // check if we are clicking on the icon
            ptl.x = SHORT1FROMMP( mp1) ;
            ptl.y = SHORT2FROMMP( mp1) ;

            // skip if not on the icon
            if( !WinPtInRect( HAB( hwnd), &rc, &ptl))
                break ;

            ti.fs = TF_MOVE | TF_STANDARD ;
            ti.cxBorder = ti.cyBorder = 2 ;
            ti.cxGrid = ti.cyGrid = 0 ;
            ti.cxKeyboard = ti.cyKeyboard = 4 ;
            ti.rclTrack = rc ;
            ti.rclBoundary.xLeft = 0L ;
            ti.rclBoundary.yBottom = 0L ;
            ti.rclBoundary.xRight = cx ;
            ti.rclBoundary.yTop = cy ;
            ti.ptlMinTrackSize.x = ti.ptlMinTrackSize.y = 1 ;
            ti.ptlMaxTrackSize.x = cx ;
            ti.ptlMaxTrackSize.y = cy ;

            WinTrackRect( hwnd, NULLHANDLE, &ti) ;
            // save new object position
            rc = ti.rclTrack ;

            // save coordinates
            ptl.x = rc.xLeft ;
            ptl.y = rc.yBottom ;

            // show the icon in the client area
            WinInvalidateRect( hwnd, NULL, FALSE) ;
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
