// SNOOPER.C - Snooping other windows
// Listing 07-01

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "snooper.h"

// definitions
#define ITEMHEIGHT 15L

// macros
#define PARENT( x)  WinQueryWindow( x, QW_PARENT)
#define HAB( x)     WinQueryAnchorBlock( x)

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 25] ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_SHELLPOSITION ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinLoadString(  hab, NULLHANDLE,
                    ST_CLASSNAME,
                    sizeof( szClassName), szClassName) ;

    WinLoadString(  hab, NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof( szWindowTitle), szWindowTitle) ;

    WinRegisterClass(   hab,
                        szClassName,
                        ClientWndProc,
                        CS_SYNCPAINT, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE,
                                    RS_ICON,
                                    &hwndClient) ;

    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        5, 5, 420, 280,
                        SWP_SHOW | SWP_MOVE | SWP_SIZE | SWP_ACTIVATE) ;

    while( WinGetMsg (hab, &qmsg, NULLHANDLE, 0L, 0L))
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
    static SNOOP snoop ;

    switch( msg)
    {
        case WM_CREATE:
            rc.xLeft = 90L ;
            rc.yBottom = 0L ;
            break ;

        case WM_BUTTON1DOWN:
            WinSetCapture( HWND_DESKTOP, hwnd) ;
            break ;

        case WM_BUTTON1DBLCLK:
            WinSetCapture( HWND_DESKTOP, 0L) ;
            WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
            break ;

        case WM_BUTTON2DOWN:
            WinSetCapture( HWND_DESKTOP, 0L) ;
            break ;

        case WM_MOUSEMOVE:
        {
            POINTL pt ;
            HWND hwndt ;
            LONG lLen ;
            CHAR szClass[ 20] ;

            pt.x = SHORT1FROMMP( mp1) ;
            pt.y = SHORT2FROMMP( mp1) ;

            // map the points for the DESKTOP
            WinMapWindowPoints( hwnd, HWND_DESKTOP, &pt, 1) ;
            // determine the selected window
            hwndt = WinWindowFromPoint( HWND_DESKTOP, &pt, TRUE) ;

            // skip if we are not on a different window or on the desktop
            if( hwndt == snoop.hwnd)
                break ;

            // now let's query the class and the window
            snoop.hwnd = hwndt ;

            *snoop.szClass = '\0' ;
            lLen = WinQueryClassName( hwndt, sizeof snoop.szClass, snoop.szClass) ;
            WinQueryClassInfo( HAB( hwndt), snoop.szClass, &snoop.clsi) ;

            if( lLen < 4 && *snoop.szClass == '#')
            {
                LONG lClass ;

                if( ( lClass = atol( snoop.szClass + 1)) > 0x28)
                    lClass = 100 ;
                WinLoadString(  HAB( hwnd),
                                NULLHANDLE,
                                lClass,
                                sizeof szClass, szClass) ;
                strcat( snoop.szClass, szClass) ;
            }

            // window procedure address
            snoop.pfnWndProc = (PFNWP)WinQueryWindowPtr( hwndt, QWP_PFNWP) ;
            snoop.hab = HAB( hwndt) ;
            snoop.hwndParent = WinQueryWindow( hwndt, QW_PARENT) ;

            *snoop.szParentClass = '\0' ;
            lLen = WinQueryClassName( snoop.hwndParent, sizeof snoop.szParentClass, snoop.szParentClass) ;
            if( lLen < 4 && *snoop.szParentClass == '#')
            {
                LONG lClass ;

                if( ( lClass = atol( snoop.szParentClass + 1)) > 0x28)
                    lClass = 100 ;
                WinLoadString(  HAB( hwnd),
                                NULLHANDLE,
                                lClass,
                                sizeof szClass, szClass) ;
                strcat( snoop.szParentClass, szClass) ;
            }

            snoop.hwndOwner = WinQueryWindow( hwndt, QW_OWNER) ;

            *snoop.szOwnerClass = '\0' ;
            lLen = WinQueryClassName( snoop.hwndOwner, sizeof snoop.szOwnerClass, snoop.szOwnerClass) ;
            if( lLen < 4 && *snoop.szOwnerClass == '#')
            {
                WinLoadString(  HAB( hwnd),
                                NULLHANDLE,
                                atol( snoop.szOwnerClass + 1),
                                sizeof szClass, szClass) ;
                strcat( snoop.szOwnerClass, szClass) ;
            }

            snoop.usID = WinQueryWindowUShort( hwndt, QWS_ID) ;
            snoop.ulStyles = WinQueryWindowULong( hwndt, QWL_STYLE) ;
            WinQueryWindowProcess( hwndt, &snoop.pid, &snoop.tid) ;

            WinInvalidateRect( hwnd, NULL, TRUE) ;
            WinQueryWindowText( snoop.hwnd, sizeof( snoop.szText), snoop.szText) ;
        }
            break ;

        case WM_SIZE:
            rc.xRight = SHORT1FROMMP( mp2) ;
            rc.yTop = SHORT2FROMMP( mp2) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            POINTL pt ;
            LONG lLen ;
            char szString[ 80] ;

            hps = WinBeginPaint( hwnd, 0, NULL);
            GpiErase( hps) ;
            pt.x = 10L ;
            pt.y = rc.yTop - ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "ClassName: %s", snoop.szClass) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "Caption: %s", snoop.szText) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "hwnd: 0x%lx", snoop.hwnd) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "hab: 0x%lx", snoop.hab) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "ID: 0x%x", snoop.usID) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "WindowStyle: 0x%lx", snoop.ulStyles) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "Parent: 0x%lx", snoop.hwndParent) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "Parent ClassName: %s", snoop.szParentClass) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "Owner: 0x%lx", snoop.hwndOwner) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "Owner ClassName: %s", snoop.szOwnerClass) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "ClassStyle: 0x%lx", snoop.clsi.flClassStyle) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "WndProc: %p", snoop.clsi.pfnWindowProc) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "WndProc: %p", snoop.pfnWndProc) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "Extra bytes: 0x%lx", snoop.clsi.cbWindowData) ;
            GpiCharStringAt(    hps, &pt, lLen, szString) ;

            pt.y -= ITEMHEIGHT ;
            lLen = (LONG) sprintf( szString, "PID:%ld - TID:%ld", snoop.pid, snoop.tid) ;
            GpiCharStringAt( hps, &pt, lLen, szString) ;

            WinEndPaint( hps);
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
