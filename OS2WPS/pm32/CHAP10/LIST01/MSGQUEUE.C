// MSGQUEUE.C - Exploring the message queue
// Listing 10-01

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "msgqueue.h"

// macros
#define HAB( x) WinQueryAnchorBlock( x)
#define CT_LIST1        1000

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE ;
    CHAR szClassName[] = "MSGQUEUE" ;
    CHAR szWindowTitle[] = "Exploring the message queue" ;

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
    static char pid[ 50], tid[ 50], cbmsg[ 50] ;
    static HWND hwndList1 ;

    switch( msg)
    {
        case WM_CREATE:
            hwndList1 = WinCreateWindow(    hwnd, WC_LISTBOX,
                                            NULL,
                                            LS_NOADJUSTPOS | WS_VISIBLE,
                                            20, 100, 200, 80,
                                            hwnd, HWND_TOP,
                                            CT_LIST1,
                                            NULL, NULL) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            POINTL ptl ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            ptl.x = ptl.y = 20L ;
            GpiCharStringAt( hps, &ptl, strlen( pid), pid) ;
            ptl.y = 40L ;
            GpiCharStringAt( hps, &ptl, strlen( tid), tid) ;
            ptl.y = 60L ;
            GpiCharStringAt( hps, &ptl, strlen( cbmsg), cbmsg) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_BUTTON1DOWN:
        {
            MQINFO mqinfo ;
            ULONG ulResult ;
            USHORT usResult ;
            USHORT i = 1 ;
            char buffer[ 40] ;
            QMSG qmsg ;

            WinPostMsg( hwnd, WM_CHAR, 0L, 0L) ;
            WinQueryQueueInfo( HMQ_CURRENT, &mqinfo, sizeof( mqinfo)) ;
            sprintf( pid, "PID = %d", mqinfo.pid) ;
            sprintf( tid, "TID = %d", mqinfo.tid) ;
            sprintf( cbmsg, "MQ = %d", mqinfo.cmsgs) ;

            WinInvalidateRect( hwnd, NULL, FALSE) ;

            ulResult = WinQueryQueueStatus( HWND_DESKTOP) ;
            usResult = HIUSHORT( ulResult) ;
            WinSendMsg( hwndList1, LM_DELETEALL, 0L, 0L) ;

            while( usResult)
            {
                if( usResult & i)
                {
                    usResult &= ~i ;

                    // load string from resource file
                    WinLoadString(  HAB( hwnd), NULLHANDLE, i,
                                    sizeof( buffer), buffer) ;

                    WinSendMsg( hwndList1, LM_INSERTITEM,
                                MPFROMSHORT( LIT_END), MPFROMP( (PSZ)buffer)) ;

                    // WinPeekMsg()
                    WinPeekMsg( HAB( hwnd), &qmsg, NULLHANDLE, 0, 0, PM_NOREMOVE) ;
                }
                i *= 2 ;
            }
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
