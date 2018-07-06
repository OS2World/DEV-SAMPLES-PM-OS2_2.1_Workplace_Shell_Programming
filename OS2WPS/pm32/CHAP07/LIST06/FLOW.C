// FLOW.C - Trapping the flow of messages
// Listing 07-06

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "flow.h"

// definition
#define WM_SETUP WM_USER

// macros
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define MENU( x)    WinWindowFromID( x, FID_MENU)
#define HAB( x)     WinQueryAnchorBlock( x)

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY TestWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
int main( void) ;


int main( void)
{
    CHAR szClassName[ 15] ;
    CHAR szTestClass[ 15] ;
    CHAR szWindowTitle[ 15] ;
    HWND hwndFrame, hwndClient ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg;
    ULONG flFrameFlags = FCF_WPS & ~FCF_ACCELTABLE & ~FCF_SHELLPOSITION ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinLoadString(  hab, NULLHANDLE,
                    ST_CLASSNAME,
                    sizeof szClassName, szClassName) ;
    WinLoadString(  hab, NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof szWindowTitle, szWindowTitle) ;

    WinLoadString(  hab, NULLHANDLE,
                    ST_TESTCLASS,
                    sizeof szTestClass, szTestClass) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, sizeof( HWND)) ;

    WinRegisterClass(   hab, szTestClass,
                        TestWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName, szWindowTitle,
                                    0L,
                                    NULLHANDLE, RS_ALL,
                                    &hwndClient) ;

    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        2, 10, 400, 400,
                        SWP_SIZE | SWP_MOVE | SWP_ACTIVATE) ;

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
    static HWND hwndListbox ;
    static CHAR szTestClass[ 15] ;

    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CREATE:
        {
            CLASSINFO clsi ;

            // load test windows class name
            WinLoadString(  HAB( hwnd), NULLHANDLE,
                            ST_TESTCLASS,
                            sizeof szTestClass, szTestClass) ;

            hwndListbox = WinCreateWindow(  hwnd, WC_LISTBOX,
                                            NULL,
                                            WS_VISIBLE | LS_NOADJUSTPOS,
                                            -1, -1, 0, 0,
                                            hwnd, HWND_TOP,
                                            0L,
                                            NULL, NULL) ;
            // get the TestWndProc address
            WinQueryClassInfo( HAB( hwnd), szTestClass, &clsi) ;

            // pass the hwndListbox handle
            (*clsi.pfnWindowProc)(0L, WM_SETUP, (MPARAM)hwndListbox, 0L) ;
        }
            break ;

        case WM_SIZE:
            WinSetWindowPos(    hwndListbox, HWND_TOP,
                                0L, 0L,
                                SHORT1FROMMP( mp2) + 1,
                                SHORT2FROMMP( mp2) + 2,
                                SWP_SHOW | SWP_SIZE | SWP_MOVE) ;
            break ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_DIALOG:
                {
                    HWND hwndFrame ;

                    // destroy the other window
                    WinDestroyWindow( WinQueryWindowULong( hwnd, QWL_USER)) ;
                    // empty the listbox
                    WinSendMsg( hwndListbox, LM_DELETEALL, 0L, 0L) ;

                    hwndFrame = WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP,
                                            NULL,
                                            NULLHANDLE,
                                            TM_WINDOW, NULL) ;

                    WinSetWindowPos(    hwndFrame, HWND_TOP,
                                        420, 10, 200, 180,
                                        SWP_SHOW | SWP_SIZE | SWP_MOVE) ;

                    // disable this option
                    WinSendMsg( MENU( PAPA( hwnd)), MM_SETITEMATTR,
                                MPFROM2SHORT( MN_DIALOG, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;
                    // enable the other option
                    WinSendMsg( MENU( PAPA( hwnd)), MM_SETITEMATTR,
                                MPFROM2SHORT( MN_WINDOW, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;

                    // store the frame hwnd
                    WinSetWindowULong( hwnd, QWL_USER, hwndFrame) ;
                }
                    break ;

                case MN_WINDOW:
                {
                    HWND hwndFrame, hwndClient ;
                    ULONG flFrameFlags =    FCF_STANDARD &
                                            ~FCF_ACCELTABLE &
                                            ~FCF_MENU &
                                            ~FCF_ICON &
                                            ~FCF_SHELLPOSITION ;
                    CHAR szWindowTitle[ 20] ;

                    // destroy the other window
                    WinDestroyWindow( WinQueryWindowULong( hwnd, QWL_USER)) ;
                    // empty the listbox
                    WinSendMsg( hwndListbox, LM_DELETEALL, 0L, 0L) ;

                    WinLoadString(  HAB( hwnd), NULLHANDLE,
                                    ST_TESTTITLE,
                                    sizeof szWindowTitle, szWindowTitle) ;

                    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                                    WS_VISIBLE,
                                                    &flFrameFlags,
                                                    szTestClass,
                                                    szWindowTitle,
                                                    0L,
                                                    NULLHANDLE, 0L,
                                                    &hwndClient) ;
                    WinSetWindowPos(    hwndFrame, HWND_TOP,
                                        420, 200, 200, 180,
                                        SWP_SIZE | SWP_MOVE) ;
                    // disable this option
                    WinSendMsg( MENU( PAPA( hwnd)), MM_SETITEMATTR,
                                MPFROM2SHORT( MN_WINDOW, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;
                    // enable the other option
                    WinSendMsg( MENU( PAPA( hwnd)), MM_SETITEMATTR,
                                MPFROM2SHORT( MN_DIALOG, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;

                    // store the client hwnd
                    WinSetWindowULong( hwnd, QWL_USER, hwndFrame) ;
                }
                    break ;

                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL);
                    break ;

                default:
                    break ;
            }
            default:
                break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY TestWndProc(   HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    char szString[ 40], szMsg[ 80] ;
    static HWND hwndListbox ;

    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, 0L, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_SETUP:
            hwndListbox = HWNDFROMMP( mp1) ;
            return (MRESULT)TRUE ;

        default:
            break ;
    }

    // send messages to the listbox
    if( !WinLoadString( HAB( hwnd), 0, msg, sizeof szString, szString))
        strcpy( szString, "undocumented") ;
    sprintf( szMsg, "%04lx - %s", msg, szString) ;
    WinSendMsg( hwndListbox, LM_INSERTITEM,
                MPFROMSHORT( LIT_END),
                MPFROMP( (PSZ) szMsg)) ;

    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;

}
