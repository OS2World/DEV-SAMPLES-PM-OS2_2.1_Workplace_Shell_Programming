// BUTTONS.C - Changing the client window color
// Listing 07-03

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include "buttons.h"


// definitions
#define CT_DEFPUSHBUTTON 1
#define CX              100
#define CY              30

// a more appropriate look
#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    HAB hab ;
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 35] ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load strings
    WinLoadString( hab, NULLHANDLE, ST_CLASSNAME, sizeof( szClassName), szClassName) ;
    WinLoadString( hab, NULLHANDLE, ST_WINDOWTITLE, sizeof( szWindowTitle), szWindowTitle) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    WS_CLIPCHILDREN,
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
    static HWND hwndButton ;
    static char *szClr[] = { "Green", "Blue", "Red"} ;
    static LONG nCnt ;
    static SHORT clr[] = { 2, 4, 1} ;

    switch( msg)
    {
        case WM_CREATE:
        {
            hwndButton = WinCreateWindow(   hwnd, WC_BUTTON,
                                            szClr[ nCnt],
                                            WS_VISIBLE | BS_PUSHBUTTON | BS_DEFAULT,
                                            5, 5, CX, CY,
                                            hwnd, HWND_TOP,
                                            CT_DEFPUSHBUTTON,
                                            NULL, NULL) ;
         }
            break ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> source)
            {
                case CMDSRC_MENU:
                    switch( COMMANDMSG( &msg) -> cmd)
                    {
                        default:
                            break ;
                    }
                    break ;

                case CMDSRC_PUSHBUTTON:
                    switch( COMMANDMSG( &msg) -> cmd)
                    {
                        case CT_DEFPUSHBUTTON:
                            ( nCnt == 2) ? nCnt = 0 : ++nCnt ;

                            WinSetWindowText( hwndButton, szClr[ nCnt]) ;
                            WinInvalidateRect( hwnd, NULL, FALSE) ;
                            break ;

                        default:
                            break ;
                    }
                    break ;
            }
                break ;

        case WM_SIZE:
            WinSetWindowPos(    hwndButton, HWND_TOP,
                                5, 5, CX, CY,
                                SWP_SHOW | SWP_MOVE | SWP_SIZE) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            WinFillRect( hps, &rc, clr[ nCnt]) ;
            WinEndPaint( hps) ;
        }
            break ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
