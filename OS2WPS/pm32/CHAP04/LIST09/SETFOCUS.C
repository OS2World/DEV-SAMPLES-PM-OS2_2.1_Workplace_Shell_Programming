// SETFOCUS.C - Focus chain
// Listing 04-09

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>

// macros & definitions
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)

#define LOOSE_FOCUS     0
#define GET_FOCUS       1

#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "SETFOCUS" ;
    CHAR szWindowTitle[] = "Intercepting WM_SETFOCUS" ;
    CHAR szKey[] = "Position" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG ulFCFrame = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_ICON & ~FCF_SHELLPOSITION ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // OWNER class
    if( !WinRegisterClass(  hab, szClassName,
                            ClientWndProc,
                            CS_SIZEREDRAW, 0L))
        WinAlarm( HWND_DESKTOP, WA_NOTE) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &ulFCFrame,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE,
                                    0L,
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

    return 0 ;
}

MRESULT EXPENTRY ClientWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
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

        case WM_SETFOCUS:
        {
            CHAR szWindowTitle[ 60] ;

            switch( SHORT1FROMMP( mp2))
            {
                case LOOSE_FOCUS:
                    sprintf( szWindowTitle, "Passing focus to 0x%lx", HWNDFROMMP( mp1)) ;
                    break ;

                case GET_FOCUS:
                    sprintf( szWindowTitle, "Getting focus from 0x%lx", HWNDFROMMP( mp1)) ;
                    break ;
            }
            WinSetWindowText( PAPA( hwnd), szWindowTitle) ;
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
