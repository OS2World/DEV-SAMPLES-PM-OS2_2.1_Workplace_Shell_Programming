// USECTL.C - Using a control of a public class
// LIST 10-10

// Stefano Maruzzi 1993

#define INCL_DOS
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <string.h>
#include "usectl.h"
#include "miecc.h"

// definitions & macros
#define ID_TWENY    1
#define HAB( x)     WinQueryAnchorBlock( x)

// function prototype
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    HMQ hmq ;
    HWND hwndClient, hwndFrame ;
    CHAR szClassName[ 15], szKey[] = "Data" ;
    CHAR szWindowTitle[ 35] ;
    QMSG qmsg ;
    ULONG ulFCFrame = FCF_STANDARD & ~FCF_ACCELTABLE & ~FCF_MENU ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0) ;

    // load the class name
    WinLoadString( hab, NULLHANDLE, ST_CLASSNAME, sizeof( szClassName), szClassName) ;
    // load the class name
    WinLoadString( hab, NULLHANDLE, ST_WINDOWTITLE, sizeof( szWindowTitle), szWindowTitle) ;

    if( !WinRegisterClass(  hab, szClassName,
                            ClientWndProc,
                            CS_SYNCPAINT | CS_SIZEREDRAW, 0L))
        return 0L ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &ulFCFrame,
                                    szClassName,
                                    szWindowTitle,
                                    WS_CLIPCHILDREN,
                                    NULLHANDLE, RS_ICON,
                                    &hwndClient) ;

    if( !WinRestoreWindowPos( szWindowTitle, szKey, hwndFrame))
    {
        SWP swp ;

        WinQueryTaskSizePos( hab, 0, &swp) ;
        WinSetWindowPos( hwndFrame, HWND_TOP,
                         swp.x, swp.y, swp.cx, swp.cy,
                         SWP_SIZE | SWP_MOVE) ;
    }
    WinSetActiveWindow( HWND_DESKTOP, hwndFrame) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    WinStoreWindowPos( szWindowTitle, szKey, hwndFrame) ;

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
        case WM_CREATE:
            // set red as the default color
            WinSetWindowULong( hwnd, 0L, (ULONG)0x00FFFFFF) ;
            break ;

        case WM_BUTTON2DOWN:
        {
            HWND hwndMyCtrl, hwndClient ;
            ULONG ulFCFrame = FCF_STANDARD & ~FCF_ACCELTABLE & ~FCF_MENU & ~FCF_ICON ;
            CHAR szControlName[ 15] ;
            HAB hab = HAB( hwnd) ;

            // load the class name
            WinLoadString( hab, NULLHANDLE, ST_CONTROLNAME, sizeof( szControlName), szControlName) ;

            hwndMyCtrl = WinCreateStdWindow(    HWND_DESKTOP, 0L,
                                                &ulFCFrame,
                                                szControlName,
                                                "Tweny control",
                                                0L,
                                                NULLHANDLE,
                                                0,
                                                &hwndClient) ;
            WinSetWindowPos(    hwndMyCtrl, HWND_TOP,
                                10, 10, 180, 200,
                                SWP_SHOW | SWP_SIZE | SWP_MOVE) ;

            hwndMyCtrl = WinCreateWindow(   hwnd, szControlName,
                                            NULL,
                                            WS_VISIBLE | WS_CLIPSIBLINGS,
                                            100, 100, 100, 100,
                                            hwnd, HWND_TOP,
                                            ID_TWENY,
                                            NULL, NULL) ;
        }
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case ID_TWENY:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case TN_CHANGE:
                        {
                            COLOR clr ;

                            clr = (COLOR)WinSendMsg( HWNDFROMMP( mp2), TM_QUERYCOLOR, 0L, 0L) ;
                        }
                            break ;

                        default:
                            break ;
                    }
                    break ;

                default:
                    break ;
            }
            break ;


        case WM_PRESPARAMCHANGED:
        {
            ULONG pp = (ULONG)HWNDFROMMP( mp1) ;
            COLOR clr ;

            WinQueryPresParam(  hwnd,
                                pp,
                                0L,
                                NULL,
                                sizeof( COLOR),
                                (PULONG)&clr,
                                QPF_NOINHERIT) ; 

            WinInvalidateRect( hwnd, NULL, TRUE) ;
        }
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;
            COLOR clr ;

            WinQueryPresParam(  hwnd,
                                PP_BACKGROUNDCOLOR,
                                0L,
                                NULL,
                                sizeof( COLOR),
                                (PULONG)&clr,
                                QPF_NOINHERIT) ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            GpiCreateLogColorTable( hps, LCOL_RESET, LCOLF_RGB, 0L, 0L, (PLONG)NULL) ;
            WinFillRect( hps, &rc, clr) ;
            WinEndPaint( hps ) ;
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
