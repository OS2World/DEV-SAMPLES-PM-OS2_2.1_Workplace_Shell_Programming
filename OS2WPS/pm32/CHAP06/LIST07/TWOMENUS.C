// TWOMENUS.C - Two menus loaded at startup
// Listing 06-07

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "twomenus.h"

// definitions & macros
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define MENU( x)        WinWindowFromID( x, FID_MENU)

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
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load class name & window title from resource file
    WinLoadString(  hab, NULLHANDLE,
                    ST_CLASSNAME,
                    sizeof( szClassName),
                    szClassName) ;
    WinLoadString(  hab, NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof( szWindowTitle),
                    szWindowTitle) ;

    // register class
    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    // create main window
    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE,
                                    RS_ALL,
                                    &hwndClient) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    // destroy resources
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
    static HWND hmenu1, hmenu2 ;

    switch( msg)
    {
        case WM_CREATE:
            hmenu1 = MENU( PAPA( hwnd)) ;
            hmenu2 = WinLoadMenu( HWND_OBJECT, 0, RS_SECOND) ;
            WinSetOwner( hmenu2, PAPA( hwnd)) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_NEW:
                    WinSetParent( hmenu1, HWND_OBJECT, FALSE) ;
                    WinSetParent( hmenu2, PAPA( hwnd), TRUE) ;
                    WinSendMsg( PAPA( hwnd), WM_UPDATEFRAME, MPFROMSHORT( FCF_MENU), 0L) ;
                    break ;

                case MN_BACK:
                    WinSetParent( hmenu2, HWND_OBJECT, FALSE) ;
                    WinSetParent( hmenu1, PAPA( hwnd), TRUE) ;
                    WinSendMsg( PAPA( hwnd), WM_UPDATEFRAME, MPFROMSHORT( FCF_MENU), 0L) ;
                    break ;

                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                    break ;

                default:
                    break ;
            }
                break ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

