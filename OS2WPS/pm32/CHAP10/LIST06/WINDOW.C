// WINDOW.C - Creating a window from a class registred in a DLL
// Listing 10-06

// Stefano Maruzzi 1993


#define INCL_DOS
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include "window.h"
#include "mie02.h"

// macros
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define SYS( x)         WinQuerySysValue( HWND_DESKTOP, x)

// definition
#define ID_LISTBOX 1
#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// functions prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
int main(void) ;

int main( void)
{
    CHAR szClassName[] = "WINDOW" ;
    CHAR szWindowTitle[] = "A generic window" ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndClient, hwndFrame ;
    QMSG qmsg ;
    ULONG ulFCFrame = FCF_WPS & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0) ;

    if( !WinRegisterClass(  hab, szClassName,
                            ClientWndProc,
                            CS_SIZEREDRAW, 0L))
        return 0L ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &ulFCFrame,
                                    szClassName,
                                    szWindowTitle,
                                    WS_CLIPCHILDREN,
                                    NULLHANDLE,
                                    RS_ALL,
                                    &hwndClient) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, (PQMSG)&qmsg) ;

    WinDestroyWindow( hwndFrame) ;
    WinDestroyMsgQueue( hmq) ;
    WinTerminate( hab) ;
}

MRESULT EXPENTRY ClientWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static HMODULE hmodDLL ;

    switch( msg)
    {
        case WM_CREATE:
            // register the new class in the DLL
            if( !( hmodDLL = InitTweny()))
                return FALSE ;
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
                    break ;

                case MN_PRODINFO:
                {
                    DLGDATA dlgData ;

                    dlgData.hwndOwner = hwnd ;
                    strcpy( dlgData.szWindowTitle, "Using a DLL") ;
                    MieDlgWnd( hwnd, &dlgData) ;
                }
                    break;
            }
            break ;

        case WM_BUTTON1CLICK:
        {
            static LONG cnt ;
            HWND hwndFrame, hwndClient ;
            ULONG ulFCFrame = FCF_NOMOVEWITHOWNER | FCF_WPS & ~FCF_ACCELTABLE & ~FCF_MENU ;
            CHAR szWindowTitle[ 20] ;

            // window title
            sprintf( szWindowTitle, "Window #%ld", cnt) ;

            // Create one control of TWENY class
            hwndFrame = WinCreateStdWindow(    HWND_DESKTOP, 0L,
                                                &ulFCFrame, "TWENY",
                                                szWindowTitle,
                                                0L, NULLHANDLE, RS_ICON, &hwndClient) ;

            if( !hwndFrame)
            {
                WinMessageBox(  HWND_DESKTOP, hwnd,
                                "Unable to create the window",
                                "No class: TWENY!", 0,
                                MB_OK | MB_ICONEXCLAMATION) ;
                break ;
            }

            // set the owner
            WinSetOwner( hwndFrame, PAPA( hwnd)) ;

            WinSetWindowPos(    hwndFrame, HWND_TOP,
                                cnt * 15L, 10L, 300L, 100L,
                                SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER) ;

            // set the appropriate ID
            WinSetWindowUShort( hwndFrame, QWS_ID, FID_OWNED + cnt++) ;
        }
            break ;

        case WM_CLOSE:
            // free the DLL
            DosFreeModule( hmodDLL) ;
            // terminate the application
            WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps );
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

