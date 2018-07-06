
// BORDER.C - Using functions provided by a DLL
// Listing 10-04

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_DOSPROCESS

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include "border.h"
#include "mie01.h"

// definitions
#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    CHAR szClassName[] = "BORDER" ;
    CHAR szWindowTitle[] = "Using a function from a DLL" ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG ulFCFrame = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &ulFCFrame,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE,
                                    RS_ICON,
                                    &hwndClient) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0, 0))
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
            RECTL rc ;
            CHAR buffer[ 40] ;
            COLOR clr ;
            POINTL pt ;

            hps = WinBeginPaint( hwnd, 0L, &rc) ;
            GpiErase( hps) ;
            // new API in MIE01.DLL
            clr = MieColorClient( hwnd, hps, &rc) ;
            sprintf( buffer, "Color %3ld", clr) ;
            pt.x = 20L ;
            pt.y = 10L ;
            GpiCharStringAt( hps, &pt, strlen( buffer), buffer) ;
            WinEndPaint( hps) ;
        }
            break ;
 
        case WM_ACTIVATE:
            if( SHORT1FROMMP( mp1))
            {
                WinInvalidateRect( hwnd, NULL, TRUE) ;
                WinUpdateWindow( hwnd) ;
            }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
