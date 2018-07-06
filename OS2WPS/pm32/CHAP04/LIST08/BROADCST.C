// BROADCST.C - Broadcasting
// Listing 04-08

// Stefano Maruzzi 1993

#define INCL_WIN

#include <os2.h>
#include <stdio.h>


// macros & definitions
#define WM_CHECKINSTANCE    WM_USER + 0
#define WM_ANSWERINSTANCE   WM_USER + 1

#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX
#define PAPA( x)            WinQueryWindow( x, QW_PARENT)

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

BOOL fStatus = TRUE ;

int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "BROADCST" ;
    CHAR szWindowTitle[] = "This window just once" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ICON & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE,
                                    0L,
                                    &hwndClient) ;

    if( fStatus)
    {
        WinShowWindow( hwndFrame, TRUE) ;

        while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0, 0))
            WinDispatchMsg( hab, &qmsg) ;
    }
    WinAlarm( HWND_DESKTOP, WA_ERROR) ;

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

        case WM_ANSWERINSTANCE:
            fStatus = FALSE ;
            break ;

        case WM_CHECKINSTANCE:
        {
            HWND hwndSender ;

            hwndSender = HWNDFROMMP( mp1) ;

            if( hwnd != hwndSender)
                WinSendMsg( hwndSender, WM_ANSWERINSTANCE, 0L, 0L) ;
        }
            break ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
