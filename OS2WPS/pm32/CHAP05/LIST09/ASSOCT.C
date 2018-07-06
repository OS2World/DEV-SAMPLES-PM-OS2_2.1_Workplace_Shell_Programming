// ASSOCT.C - Assoctable
// Listing 05-09

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include "assoct.h"

// definitions & macros
#define HAB( x) WinQueryAnchorBlock( x)
#define SYSVAL( x) WinQuerySysValue( HWND_DESKTOP, x)

// function prototypes
int main(void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "ASSOCTABLE" ;
    CHAR szWindowTitle[] = "Assoctable" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_MENU & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE,
                                    RS_ICON,
                                    &hwndClient) ;

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
        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_BUTTON2DOWN:
        {
            DENA2 dena2 ;
            ULONG ulEnumCount = - 1;

            DosEnumAttribute(       ENUMEA_REFTYPE_PATH,
                                                "H:\\PM32\\CHAP05\\LIST09\\MACHINE.EXE",
                                                1,
                                                &dena2,
                                                sizeof( dena2),
                                                &ulEnumCount,
                                                ENUMEA_LEVEL_NO_VALUE) ;
        }
            break ;

        default:
        break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
