// OWNER.C - Ownership
// Listing 04-2

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc1( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ClientWndProc2( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;



int main( void)
{
    HAB hab ;
    CHAR szClassOwner[] = "OWNER" ;
    CHAR szTitleOwner[] = "Owner" ;
    CHAR szClassOwned[] = "OWNED" ;
    CHAR szTitleOwned[] = "Owned" ;
    HMQ hmq ;
    HWND hwndFrame1, hwndFrame2, hwndClient ;
    QMSG qmsg ;
    ULONG ulFCFrame = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_ICON & ~FCF_SHELLPOSITION ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // OWNER class
    if( !WinRegisterClass(  hab, szClassOwner,
                            ClientWndProc1,
                            CS_SIZEREDRAW, 0L))
        WinAlarm( HWND_DESKTOP, WA_NOTE) ;

    // OWNED class
    if( !WinRegisterClass(  hab, szClassOwned,
                            ClientWndProc2,
                            CS_SIZEREDRAW, 0L))
        WinAlarm( HWND_DESKTOP, WA_NOTE) ;

    hwndFrame1 = WinCreateStdWindow(    HWND_DESKTOP,
                                        WS_VISIBLE,
                                        &ulFCFrame,
                                        szClassOwner,
                                        szTitleOwner,
                                        0L,
                                        NULLHANDLE,
                                        0L,
                                        &hwndClient) ;

    WinSetWindowPos( hwndFrame1, HWND_TOP,
                     90, 90, 200, 300,
                     SWP_EXTSTATECHANGE | SWP_SIZE | SWP_MOVE | SWP_ACTIVATE) ;

    hwndFrame2 = WinCreateStdWindow(    HWND_DESKTOP,
                                        WS_VISIBLE,
                                        &ulFCFrame,
                                        szClassOwned,
                                        szTitleOwned,
                                        0L,
                                        NULLHANDLE,
                                        0L,
                                        &hwndClient) ;

    // set the owner
    WinSetOwner( hwndFrame2, hwndFrame1) ;

    WinSetWindowPos(    hwndFrame2, HWND_TOP,
                        400, 90, 390, 300,
                        SWP_SIZE | SWP_MOVE | SWP_ACTIVATE) ;

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