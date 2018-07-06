// PARENT.C - Parenthood
// Listing 04-1

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc1( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ClientWndProc2( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    CHAR szClassOwner[] = "PARENT" ;
    CHAR szTitleOwner[] = "Parent" ;
    CHAR szClassOwned[] = "SON" ;
    CHAR szTitleOwned[] = "Son" ;
    HMQ hmq ;
    HWND hwndFrame1, hwndFrame2, hwndClient ;
    QMSG qmsg ;
    ULONG ulFCFrame = FCF_HIDEMAX | FCF_STANDARD & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_ICON & ~FCF_MINMAX ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // PARENT class
    if( !WinRegisterClass(  hab, szClassOwner,
                            ClientWndProc1,
                            CS_SIZEREDRAW, 0L))
        WinAlarm( HWND_DESKTOP, WA_NOTE) ;

    // SON class
    if( !WinRegisterClass(  hab, szClassOwned,
                            ClientWndProc2,
                            CS_SIZEREDRAW, 0L))
        WinAlarm( HWND_DESKTOP, WA_NOTE) ;

    hwndFrame1 = WinCreateStdWindow(    HWND_DESKTOP,
                                        WS_VISIBLE,
                                        &ulFCFrame,
                                        szClassOwner,
                                        szTitleOwner,
                                        WS_CLIPCHILDREN,
                                        NULLHANDLE,
                                        0L,
                                        &hwndClient) ;

    hwndFrame2 = WinCreateStdWindow(    hwndClient,
                                        0L,
                                        &ulFCFrame,
                                        szClassOwned,
                                        szTitleOwned,
                                        0L,
                                        NULLHANDLE,
                                        0L,
                                        &hwndClient) ;

    WinSetWindowPos(    hwndFrame2, HWND_TOP,
                        10L, 10L, 200L, 200L,
                        SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    WinDestroyWindow( hwndFrame1) ;
    WinDestroyWindow( hwndFrame2) ;
    WinDestroyMsgQueue( hmq) ;
    WinTerminate( hab) ;

    return 0L ;
}

MRESULT EXPENTRY ClientWndProc1(    HWND hwnd,
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

MRESULT EXPENTRY ClientWndProc2(    HWND hwnd,
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