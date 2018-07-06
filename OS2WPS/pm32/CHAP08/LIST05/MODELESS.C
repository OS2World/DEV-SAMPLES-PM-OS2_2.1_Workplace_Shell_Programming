// MODELESS.C - Modeless dialog window
// Listing 08-04

// Stefano Maruzzi 1993


#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include "modeless.h"
#include "dialog.h"

// definitions
#define SCROLLPAGE          2
#define WM_COLOR            WM_USER

// macros
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define MENU( x)    WinWindowFromID( x, FID_MENU)

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY Color( HWND hwnd , ULONG msg, MPARAM mp1, MPARAM mp2) ;
int main( void) ;

int main( void)
{
    CHAR szClassName[] = "MODELESS" ;
    CHAR szWindowTitle[] = "Modeless dialog" ;
    HWND hwndFrame, hwndClient ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_TASKLIST ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0) ;

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
                                    RS_ALL,
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
    static HWND hwndMdl ;
    static COLOR clr = CLR_WHITE ;

    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc);
            WinFillRect( hps, &rc, clr) ;
            WinEndPaint( hps);
        }
            break ;

        case WM_DESTROY:
            WinDestroyWindow( hwndMdl) ;
            break ;

        case WM_COLOR:
            clr = LONGFROMMP( mp1) ;
            WinInvalidateRect( hwnd, NULL, TRUE) ;
            break ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg )->cmd )
            {
                case MN_CHOOSECOLOR:
                    if( !hwndMdl)
                        hwndMdl = WinLoadDlg(   HWND_DESKTOP, hwnd,
                                                Color, NULLHANDLE, 256, (PVOID)&hwnd) ;
                    else
                    {
                        if(!WinIsWindowVisible( hwndMdl))
                        {
                            WinShowWindow( hwndMdl, TRUE) ;
                            WinSetActiveWindow( HWND_DESKTOP, hwndMdl) ;
                        }
                    }
                    break ;

                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                    break ;
            }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY Color( HWND hdlg,
                        ULONG msg,
                        MPARAM mp1,
                        MPARAM mp2)
{
    static HWND hwndOwner ;
    static SHORT sPos ;

    switch( msg)
    {
        case WM_INITDLG:
        {
            USHORT usScrollTop = 15 ;

            WinSendDlgItemMsg(  hdlg, DL_SCROLL, SBM_SETSCROLLBAR,
                                MPFROMSHORT( 0),
                                MPFROM2SHORT( 1, usScrollTop)) ;
            WinSetDlgItemShort( hdlg, DL_STATIC, usScrollTop, FALSE) ;
            hwndOwner = ( *(PHWND)mp2) ;
        }
            break ;

        case WM_HSCROLL:
        {
            USHORT usScrollTop = 15 ;

            switch( SHORT2FROMMP( mp2))
            {
                case SB_LINELEFT:
                    sPos-- ;
                    break ;

                case SB_LINERIGHT:
                    sPos++ ;
                    break ;

                case SB_PAGERIGHT:
                    sPos += SCROLLPAGE ;
                    break ;

                case SB_PAGELEFT:
                    sPos -= SCROLLPAGE ;
                    break ;

                case SB_SLIDERTRACK:
                case SB_SLIDERPOSITION:
                    sPos = SHORT1FROMMP( mp2) ;
                    break ;

                default:
                break ;
            }

            // check the values
            if( sPos < 0)
                sPos = 0 ;
            if( sPos > (SHORT)usScrollTop)
                sPos = (SHORT)usScrollTop ;

            WinSendDlgItemMsg(  hdlg, DL_SCROLL, SBM_SETPOS,
                                MPFROMSHORT( sPos), 0L) ;
            WinSetDlgItemShort( hdlg, DL_SHOW, sPos, TRUE) ;
            WinSendMsg( hwndOwner, WM_COLOR,
                        MPFROMSHORT( sPos), 0L) ;
        }
            break ;

        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1))
            {
                case DID_OK:
                    WinShowWindow( hdlg, FALSE) ;
                    break ;
            }
            break ;

        default:
            break ;
    }
    return WinDefDlgProc( hdlg, msg, mp1, mp2) ;
}
