// BMPMENU.C - Bitmasp menuitems
// Listing 06-08

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "bmpmenu.h"

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
    static COLOR clr ;

    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            WinFillRect( hps, &rc, clr) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_RED:
                    clr = CLR_RED ;
                    break ;

                case MN_BLUE:
                    clr = CLR_BLUE ;
                    break ;

                case MN_PINK:
                    clr = CLR_PINK ;
                    break ;

                case MN_GREEN:
                    clr = CLR_GREEN ;
                    break ;

                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
                    break ;

                default:
                    break ;
            }
            WinInvalidateRect( hwnd, NULL, TRUE) ;
            WinUpdateWindow( hwnd) ;
            break ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

