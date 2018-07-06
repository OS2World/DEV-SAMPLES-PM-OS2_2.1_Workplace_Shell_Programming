// MACHINE.C - A basci PM application
// Listing 02-09

// Stefano Maruzzi 1993

#define INCL_WIN

#include <os2.h>

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags =  FCF_STANDARD & ~FCF_MENU & ~FCF_ICON & ~FCF_ACCELTABLE ;
    CHAR szClassName[] = "Machine" ;
    CHAR szWindowTitle[] = "Basic PM application" ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW,  0) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE,
                                    0L,
                                    &hwndClient) ;

    WinShowWindow( hwndFrame, TRUE) ;

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
    switch( msg)
    {
        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
