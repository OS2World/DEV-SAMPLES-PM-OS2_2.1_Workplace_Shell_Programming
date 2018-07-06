// CREATE.C - Intercepting WM_CREATE
// Listing 03-05

// Stefano Maruzzi - 1993


// header files
#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>

#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
int main( void) ;


int main( void)
{
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ICON & ~FCF_ACCELTABLE & ~FCF_SHELLPOSITION ;
    CHAR szClassName[] = "CREATE" ;
    CHAR szWindowTitle[] = "Intercepting WM_CREATE" ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0) ;

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

    WinSetWindowPos(    hwndFrame, HWND_DESKTOP,
                        50, 50,
                        430, 200,
                        SWP_ACTIVATE | SWP_SIZE | SWP_MOVE | SWP_SHOW) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    WinDestroyMsgQueue( hmq) ;
    WinDestroyWindow( hwndFrame) ;
    WinTerminate( hab) ;

     return 0L ;
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

        case WM_CREATE:
        {
            PCREATESTRUCT pcrst ;
            PVOID pCtlData ;
 
            pcrst = (PCREATESTRUCT) PVOIDFROMMP( mp2) ;
            pCtlData = (PVOID) PVOIDFROMMP( mp1) ; 
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
