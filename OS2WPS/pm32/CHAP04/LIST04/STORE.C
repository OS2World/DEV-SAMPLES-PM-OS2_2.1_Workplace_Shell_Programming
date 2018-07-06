// STORE.C - Store and restore window pos
// Listing 04-04

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX


// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "STORE" ;
    CHAR szWindowTitle[] = "Store/restore" ;
    CHAR szKey[] = "Position" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG ulFCFrame = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_ICON & ~FCF_SHELLPOSITION ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

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

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
