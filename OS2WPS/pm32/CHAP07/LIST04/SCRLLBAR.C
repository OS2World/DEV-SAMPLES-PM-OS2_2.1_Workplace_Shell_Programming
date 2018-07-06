// SCRLLBAR.C - Changing the client window color using a scrollbar
// Listing 07-04

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include "scrllbar.h"


// definitions
#define CT_SCROLLBAR    1
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
    CHAR szWindowTitle[ 40] ;
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
                        CS_SYNCPAINT | CS_SIZEREDRAW, 0L) ;

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
    static HWND hwndScroll ;
    static COLOR clr ;

    switch( msg)
    {
        case WM_CREATE:
        {
            SBCDATA sbcdata ;

            sbcdata.cb = (USHORT)sizeof( SBCDATA) ;
            sbcdata.sHilite = 0 ;
            sbcdata.posFirst = 0 ;
            sbcdata.posLast = 15 ;
            sbcdata.posThumb = 0 ;
            sbcdata.cVisible = 1 ;
            sbcdata.cTotal = 15 ;

            hwndScroll = WinCreateWindow(   hwnd, WC_SCROLLBAR,
                                            NULL,
                                            SBS_HORZ | SBS_AUTOSIZE | SBS_THUMBSIZE | WS_VISIBLE,
                                            10, 10, 0, 0,
                                            hwnd, HWND_TOP,
                                            CT_SCROLLBAR,
                                            &sbcdata, NULL) ;
         }
            break ;

        case WM_SIZE:
            WinSetWindowPos(    hwndScroll, HWND_TOP,
                                0, 0, SHORT1FROMMP( mp2) - 20, 14,
                                SWP_SIZE) ;
            break ;

        case WM_HSCROLL:
            switch( SHORT2FROMMP( mp2))
            {
                case SB_LINELEFT:
                    clr -= 1 ;
                    break ;

                case SB_LINERIGHT:
                    clr += 1 ;
                    break ;

                case SB_PAGELEFT:
                    clr -= 3 ;
                    break ;

                case SB_PAGERIGHT:
                    clr += 3 ;
                    break ;

                case SB_SLIDERPOSITION:
                    clr = SHORT1FROMMP( mp2) ;
                    break ;

                case SB_SLIDERTRACK:
                    break ;

                case SB_ENDSCROLL:
                    break ;
            }
            WinSendMsg( hwndScroll,SBM_SETPOS,
                        MPFROMSHORT( clr), 0L) ;
            WinInvalidateRect( hwnd, NULL, FALSE) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            WinFillRect( hps, &rc, clr) ;
            WinEndPaint( hps) ;
        }
            break ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
