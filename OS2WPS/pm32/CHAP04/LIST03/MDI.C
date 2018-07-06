// MDI.C - Two windows created with WinCreateWindow()
// Listing 04-03

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>


// definitions and macros
#define PAPA( x) WinQueryWindow( x, QW_PARENT)

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc1( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ClientWndProc2( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    CHAR szClassName1[] = "Class1" ;
    CHAR szClassName2[] = "Class2" ;
    CHAR szWindowTitle1[] = "Parent" ;
    CHAR szWindowTitle2[] = "Child" ;
    HMQ hmq ;
    HWND hwndFrame1, hwndClient1, hwndFrame2, hwndClient2 ;
    QMSG qmsg ;
    FRAMECDATA fcData ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinRegisterClass(   hab, szClassName1,
                        ClientWndProc1,
                        CS_SIZEREDRAW, 0L) ;

    WinRegisterClass(   hab, szClassName2,
                        ClientWndProc2,
                        CS_SIZEREDRAW, 0L) ;

    fcData.cb = sizeof fcData ;
    fcData.flCreateFlags = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_ICON & ~FCF_SHELLPOSITION ;
    fcData.hmodResources = NULLHANDLE ;
    fcData.idResources = 0 ;

    hwndFrame1 = WinCreateWindow(   HWND_DESKTOP, WC_FRAME,
                                    szWindowTitle1,
                                    WS_VISIBLE | WS_ANIMATE,
                                    0L, 0L, 0L, 0L,
                                    HWND_DESKTOP, HWND_TOP,
                                    0L,
                                    &fcData, NULL) ;

    hwndClient1 = WinCreateWindow(  hwndFrame1, szClassName1,
                                    NULL,
                                    WS_VISIBLE | WS_CLIPCHILDREN,
                                    0L, 0L, 0L, 0L,
                                    hwndFrame1, HWND_TOP,
                                    FID_CLIENT,
                                    NULL, NULL) ;

    WinSetWindowPos(    hwndFrame1, HWND_DESKTOP,
                        90, 90, 390, 300,
                        SWP_SIZE | SWP_MOVE | SWP_ACTIVATE) ;

    hwndFrame2 = WinCreateWindow(   hwndClient1, WC_FRAME,
                                    szWindowTitle2,
                                    WS_VISIBLE,
                                    0L, 0L, 0L, 0L,
                                    hwndClient1, HWND_TOP,
                                    0L,
                                    &fcData, NULL) ;

    hwndClient2 = WinCreateWindow(  hwndFrame2, szClassName2,
                                    NULL,
                                    WS_VISIBLE,
                                    0L, 0L, 0L, 0L,
                                    hwndFrame2, HWND_TOP,
                                    FID_CLIENT,
                                    NULL, NULL) ;

    WinSetWindowPos(    hwndFrame2, HWND_DESKTOP,
                        20, 20, 190, 90,
                        SWP_SIZE | SWP_MOVE | SWP_ACTIVATE) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    WinDestroyWindow( hwndFrame1) ;
    WinDestroyWindow( hwndFrame2) ;
    WinDestroyMsgQueue( hmq) ;
    WinTerminate( hab) ;

    return 0 ;
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

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL);
            GpiErase( hps) ;
            WinEndPaint( hps);
        }
            break;

        default:
            break;
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

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL);
            GpiErase( hps) ;
            WinEndPaint( hps);
        }
            break;

       case WM_CLOSE:
       {
           // prevent child window to post WM_QUIT message
           WinDestroyWindow( PAPA( hwnd)) ;
           return 0L ;

       }
           break ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

