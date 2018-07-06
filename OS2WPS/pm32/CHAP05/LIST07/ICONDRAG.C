// ICONDRAG.C - Dragging an icon
// Listing 05-07

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include "icondrag.h"

// definitions
#define STARTX 50L
#define STARTY 50L

// macros
#define HAB( x)     WinQueryAnchorBlock( x)
#define SYS( x)     WinQuerySysValue( HWND_DESKTOP, x)

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main(void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 25] ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0) ;

    // load class and window title
    WinLoadString( hab, NULLHANDLE, ST_CLASSNAME, sizeof( szClassName), szClassName) ;
    WinLoadString( hab, NULLHANDLE, ST_WINDOWTITLE, sizeof( szWindowTitle), szWindowTitle) ;

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
    static BOOL fDrag = FALSE ;
    static RECTL rc ;
    static POINTL ptl ;
    static HPOINTER hptr ;
    static USHORT uscx, uscy ;

    switch( msg)
    {
        case WM_CREATE:
            hptr = WinLoadPointer( HWND_DESKTOP, 0, RS_ICON) ;
            rc.xRight = SYS( SV_CXPOINTER) + STARTX ;
            rc.yTop = SYS( SV_CYPOINTER) + STARTY ;
            rc.xLeft = ptl.x = STARTX ;
            rc.yBottom = ptl.y = STARTY ;
            uscx = (USHORT)SYS( SV_CXPOINTER) ;
            uscy = (USHORT)SYS( SV_CYPOINTER) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinDrawPointer( hps, (SHORT)ptl.x, (SHORT)ptl.y, hptr, DP_NORMAL) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_MOUSEMOVE:
            if( !fDrag)
                break ;

            WinSetPointer( HWND_DESKTOP, hptr) ;
            return (MRESULT)1L ;

        case WM_BUTTON2DOWN:
            // check if we are clicking on the icon
            ptl.x = (SHORT) SHORT1FROMMP( mp1) ;
            ptl.y = (SHORT) SHORT2FROMMP( mp1) ;

            // skip if not on the icon
            if( !WinPtInRect( HAB( hwnd), &rc, &ptl))
                break ;

            fDrag = TRUE ;

            // overlapped new mouse pointer to the existing icon
            ptl.x = rc.xLeft + uscx / 2 ;
            ptl.y = rc.yBottom + uscy / 2 ;
            WinMapWindowPoints( hwnd, HWND_DESKTOP, &ptl, 1) ;
            WinSetPointerPos( HWND_DESKTOP, (SHORT)ptl.x, (SHORT)ptl.y) ;

            // show new pointer
            WinSetPointer( HWND_DESKTOP, hptr) ;
            break ;

        case WM_BUTTON2UP:
            // skip if we are not dragging
            if( !fDrag)
                break ;

            // stop dragging
            fDrag = FALSE ;

            // calculate new position
            ptl.x = rc.xLeft = SHORT1FROMMP( mp1) - uscx / 2 ;
            ptl.y = rc.yBottom = SHORT2FROMMP( mp1) - uscy / 2 ;
            rc.xRight = uscx + rc.xLeft ;
            rc.yTop = uscy + rc.yBottom ;

            // show arrow pointer
            WinSetPointer(  HWND_DESKTOP,
                            WinQuerySysPointer( HWND_DESKTOP,  SPTR_ARROW, TRUE)) ;
            // show the icon in the client area
            WinInvalidateRect( hwnd, NULL, FALSE) ;
            break ;

        case WM_DESTROY:
            WinDestroyPointer( hptr) ;
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
