// ENUM.C - Windows enumeration
// Listing 04-08

// Stefano Maruzzi 1993

#define INCL_WIN

#include <os2.h>
#include <stdio.h>
#include <string.h>

// macros & definitions
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX
#define WM_CHECKINSTANCE        WM_USER


// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "ENUM" ;
    CHAR szWindowTitle[] = "This window just once" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient, hwndNext ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ICON & ~FCF_ACCELTABLE ;
    HENUM henum ;
    CHAR szBuffer[ 80] ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    // start enumeration
    henum = WinBeginEnumWindows( HWND_DESKTOP) ;

    // loop
    while( ( hwndNext = WinGetNextWindow( henum)))
    {
        // lets find the client window class
        if( ( hwndClient = WinWindowFromID( hwndNext, FID_CLIENT)))
        {
            WinQueryClassName( hwndClient, sizeof( szBuffer), szBuffer) ;

            // compare class names
            if( strstr( szBuffer, strupr( szClassName)))
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                WinMessageBox(  HWND_DESKTOP, HWND_DESKTOP,
                                "Instance is already active",
                                "Instance management",
                                0L, MB_OK | MB_MOVEABLE) ;
                WinEndEnumWindows( henum) ;
                WinDestroyMsgQueue( hmq) ;
                WinTerminate( hab) ;
                return FALSE ;
            }
        }
    }
    WinEndEnumWindows( henum) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE,
                                    0L,
                                    &hwndClient) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg( hab, &qmsg) ;

    WinDestroyWindow( hwndFrame) ;
    WinDestroyMsgQueue( hmq) ;
    WinTerminate( hab) ;

    return 0 ;
}

MRESULT EXPENTRY ClientWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    switch( msg)
    {
        case WM_CREATE:
            // broadcasting the message
            WinBroadcastMsg(    HWND_DESKTOP, WM_CHECKINSTANCE,
                                MPFROMLONG( hwnd), 0L,
                                BMSG_FRAMEONLY | BMSG_SEND) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
