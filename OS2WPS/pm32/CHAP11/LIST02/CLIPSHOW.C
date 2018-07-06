// CLIPSHOW.C - Retrieving data from the Clipboard
// Listing 11-02

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL
#include <os2.h>
#include "clipshow.h"

// definitions & macros
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define MENU( x)        WinWindowFromID( x, FID_MENU)
#define HAB( x)         WinQueryAnchorBlock( x)

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

#define CT_MLE      1000
#define MAXCHARS    300

#define WM_PASSPROC     WM_USER + 0

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    CHAR szWindowTitle[ 45] ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load window title from resource file
    WinLoadString(  hab, NULLHANDLE, ST_WINDOWTITLE,
                    sizeof( szWindowTitle), szWindowTitle) ;

    // create main window
    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    WC_MLE,
                                    szWindowTitle,
                                    WS_CLIPCHILDREN,
                                    NULLHANDLE,
                                    RS_ALL,
                                    &hwndClient) ;

    // subclass the MLE
    WinSendMsg( hwndClient, WM_PASSPROC, MPFROMP( WinSubclassWindow( hwndClient, ClientWndProc)), 0L) ;

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
    static PFNWP pfnwp ;
    static HWND hmenu ;

    switch( msg)
    {
        case WM_PASSPROC:
            pfnwp = (PFNWP)mp1 ;
            // limit text to MAXCHARS characters
            WinSendMsg( hwnd, MLM_SETTEXTLIMIT, MPFROMLONG( MAXCHARS), 0L) ;
            // get the menu handle
            hmenu = MENU( PAPA( hwnd)) ;
            return 0L ;

        case WM_INITMENU:
            if( SHORT1FROMMP( mp1) == MN_EDIT)
            {
                HAB hab = HAB( hwnd) ;
                ULONG ulFmtInfo = 0L ;

                // check the Clipboard content
                WinQueryClipbrdFmtInfo( hab, CF_TEXT, &ulFmtInfo) ;

                if( ulFmtInfo == CFI_POINTER)
                {
                    // enable the paste option
                    WinSendMsg( (HWND)mp2, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_PASTE, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;
                }
                else
                {
                    // disable the paste option
                    WinSendMsg( (HWND)mp2, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_PASTE, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;
                }
            }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_PASTE:
                {
                    PSZ pszString ;
                    HAB hab = HAB( hwnd) ;

                    // open the Clipboard
                    if( !WinOpenClipbrd( hab))
                        break ;

                    if( !( pszString = (PSZ)WinQueryClipbrdData( hab, CF_TEXT)))
                    {
                        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                        WinCloseClipbrd( hab) ;
                        break ;
                    }
                    // set text in the MLE window
                    WinSetWindowText( hwnd, pszString) ;

                    // close the Clipboard
                    WinCloseClipbrd( hab) ;
                }
                    break ;

                default:
                    break ;

                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                    break ;
            }
                break ;

        default:
            break;
    }
    return (*pfnwp)( hwnd, msg, mp1, mp2) ;
}

