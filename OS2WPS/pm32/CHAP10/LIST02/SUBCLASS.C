// SUBCLASS.C - Subclassing a control
// Listing 10-02

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "subclass.h"

// definitions
#define WM_PASSPROC WM_USER
#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX
#define CT_EDIT1        1000
#define CT_EDIT2        1001

// macros
#define TITLE( x) WinWindowFromID( x, FID_TITLEBAR)
#define PAPA( x) WinQueryWindow( x, QW_PARENT, FALSE)

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY MyWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY Frame( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    CHAR szClassName[] = "SUBCLASS" ;
    CHAR szWindowTitle[] = "Subclassing a WC_MLE window" ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_ACCELTABLE & ~FCF_MENU ;

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
                                    NULLHANDLE,
                                    RS_ICON,
                                    &hwndClient) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    WinDestroyWindow (hwndFrame) ;
    WinDestroyMsgQueue (hmq) ;
    WinTerminate (hab) ;

    return 0 ;
}

MRESULT EXPENTRY ClientWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static HWND hwndE1, hwndE2 ;
    static USHORT cx, cy ;

    switch( msg)
    {
        case WM_CREATE:
        {
            PFNWP pfnwp ;

            hwndE1 = WinCreateWindow(   hwnd, WC_MLE,
                                        NULL,
                                        WS_VISIBLE | MLS_BORDER,
                                        0L, 0L, 0L, 0L,
                                        hwnd, HWND_TOP,
                                        CT_EDIT1,
                                        NULL, NULL) ;

            hwndE2 = WinCreateWindow(   hwnd, WC_MLE,
                                        NULL,
                                        WS_VISIBLE | MLS_BORDER,
                                        0L, 0L, 0L, 0L,
                                        hwnd, HWND_TOP,
                                        CT_EDIT2,
                                        NULL, NULL) ;

            // subclassing the first mle
            pfnwp = WinSubclassWindow( hwndE1, MyWndProc) ;
            WinSendMsg( hwndE1, WM_PASSPROC, MPFROMLONG( pfnwp), 0L) ;
            // set focus on the subclassed window
            WinSetFocus( HWND_DESKTOP, hwndE1) ;
        }
            break ;

        case WM_SIZE:
            // window height & width
            cy = SHORT2FROMMP( mp2) - 25 ;
            cx = SHORT1FROMMP( mp2) ;
 

            WinSetWindowPos(    hwndE1, HWND_TOP,
                                0L, 0L, cx / 2, cy,
                                SWP_ZORDER | SWP_SIZE) ;

            WinSetWindowPos(    hwndE2, HWND_TOP,
                                cx / 2, 0L, cx / 2, cy,
                                SWP_ZORDER | SWP_SIZE | SWP_MOVE) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            POINTL pt ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL);
            GpiErase( hps) ;
            pt.x = 2L ;
            pt.y = cy + 4 ;
            GpiCharStringAt( hps, &pt, strlen( "No lower case"), "No lower case") ;
            pt.x = cx / 2 ;
            GpiCharStringAt( hps, &pt, strlen( "Any case"), "Any case") ;
            WinEndPaint( hps);
        }
            break ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY MyWndProc( HWND hwnd,
                            ULONG msg,
                            MPARAM mp1,
                            MPARAM mp2)
{
    static PFNWP pfnwp ;

    switch( msg)
    {
        case WM_PASSPROC:
            pfnwp = (PFNWP)mp1 ;
            break ;

        case WM_CHAR:
        {
            USHORT usChr, usVKey ;

            usChr = CHARMSG(&msg) -> chr ;
            usVKey = CHARMSG(&msg) -> vkey ;

            if( usChr > 96 && usChr < 123)
                usChr -= 32 ;

            mp2 = MPFROM2SHORT( usChr, usVKey) ;
        }
            break ;

        default:
            break ;
    }
    return (*pfnwp)( hwnd, msg, mp1, mp2) ;
}
