// SUPERCLS.C - Superclassing example
// Listing 10-03

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "supercls.h"

// definition
#define WM_PASSPROC WM_USER
#define CT_EDIT1        1000
#define CT_EDIT2        1001
#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~ FCF_MINMAX


// macros
#define TITLE( x)   WinWindowFromID( x, FID_TITLEBAR)
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define HAB( x)     WinQueryAnchorBlock( x)

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY MyWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    CHAR szClassName[] = "SUPERCLAS" ;
    CHAR szWindowTitle[] = "Superclassing" ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_ACCELTABLE & ~FCF_MENU ;

    hab = WinInitialize( 0L) ;
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

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg( hab, &qmsg) ;

    WinDestroyWindow (hwndFrame) ;
    WinDestroyMsgQueue (hmq) ;
    WinTerminate (hab) ;

    return 0L ;
}

MRESULT EXPENTRY ClientWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    switch( msg)
    {
        case WM_CREATE:
        {
            HWND hwndE1, hwndE2 ;
            CLASSINFO clsi ;
            CHAR szClassName[] = "NewEdit" ;

            // get all the info from class WC_ENTRYFIELD
            if( !WinQueryClassInfo( HAB( hwnd), WC_ENTRYFIELD, &clsi))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // register the superclass
            if( !WinRegisterClass(  HAB( hwnd), szClassName, MyWndProc,
                                    clsi.flClassStyle & ~CS_PUBLIC,
                                    clsi.cbWindowData))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // pass the old wndproc address
            (*MyWndProc)(   NULLHANDLE, WM_PASSPROC,
                            MPFROMLONG( (ULONG)clsi.pfnWindowProc), 0L) ;

            // create 2 controls of the new class
            hwndE1 = WinCreateWindow(   hwnd, szClassName,
                                        NULL,
                                        WS_VISIBLE | ES_AUTOSCROLL | ES_MARGIN,
                                        30, 60, 300, 20,
                                        hwnd, HWND_TOP,
                                        CT_EDIT1,
                                        NULL, NULL) ;

            hwndE2 = WinCreateWindow(   hwnd, szClassName,
                                        NULL,
                                        WS_VISIBLE | ES_AUTOSCROLL | ES_MARGIN,
                                        30, 30, 300, 20,
                                        hwnd, HWND_TOP,
                                        CT_EDIT2,
                                        NULL, NULL) ;
            // set focus on the first control
            WinSetFocus( HWND_DESKTOP, hwndE1) ;
        }
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            POINTL pt ;
            CHAR szText[] = "Two controls of class NewEdit: no lower case allowed" ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL);
            GpiErase( hps) ;
            pt.x = 30L ;
            pt.y = 90L ;
            GpiCharStringAt(    hps, &pt, strlen( szText), szText) ;
            WinEndPaint( hps);
        }
            break ;

        default:
            break ;
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