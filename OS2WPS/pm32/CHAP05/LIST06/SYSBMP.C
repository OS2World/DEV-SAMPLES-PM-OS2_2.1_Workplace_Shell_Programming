// SYSBMP.C - A Program that shows sys bitmaps
// Listing 05-06

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_GPIBITMAPS

#include <os2.h>
#include <stdio.h>
#include "sysbmp.h"

// definitions
#define HEIGHT 19
#define LIMIT 25

// macros
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define HAB( x)         WinQueryAnchorBlock( x)
#define SYS( x)         WinQuerySysValue( HWND_DESKTOP, x)

// function prototypes
int main(void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
void ShowPointers( HPS hps) ;
void ShowIcons( HPS hps, HWND hwnd) ;


int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "SYSBMP" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_TASKLIST ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &flFrameFlags,
                                    szClassName,
                                    NULL,
                                    0L,
                                    NULLHANDLE, 0L,
                                    &hwndClient) ;

    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        0, 0, 0, 0,
                        SWP_MAXIMIZE | SWP_SHOW | SWP_ACTIVATE) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

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
        case WM_BUTTON1DOWN:
            WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            ShowPointers( hps) ;
            ShowIcons( hps, hwnd) ;
            WinEndPaint( hps) ;
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}


void ShowPointers( HPS hps)
{
    LONG j, k = 0, lLen, lSize = SYS( SV_CXPOINTER) + 2, y =SYS( SV_CYSCREEN) - 40 ;
    HPOINTER hptr ;
    POINTL pt ;
    CHAR szBuffer[ 50] ;

    // string Y position
    pt.y = y - 35 ;

    for( j = 1; j < 23; j++)
    {
        if( !( hptr = WinQuerySysPointer( HWND_DESKTOP, j, FALSE)))
            continue ;

        WinDrawPointer( hps, 2 + k * lSize, y, hptr, DP_NORMAL) ;
        pt.x = lSize / 2 + lSize * k++ ;
        lLen = sprintf( szBuffer, "%d", j) ;
        GpiCharStringAt( hps, &pt, lLen, szBuffer) ;
    }
}


void ShowIcons( HPS hps, HWND hwnd)
{
    CHAR szBuffer[ 70], szString[ 70] ;
    LONG lLen, i, k = 0, xHalf, yStart ;
    HBITMAP hbm ;
    POINTL pt ;

    yStart = SYS( SV_CYSCREEN) - SYS( SV_CYSCREEN) / 6 ;
    xHalf = SYS( SV_CXSCREEN) / 2 ;

    for( i = 1; i < 49; i++)
    {
        if( !WinLoadString( HAB( hwnd), NULLHANDLE, i, sizeof( szBuffer), szBuffer))
            continue ;

        if( i == SBMP_CHECKBOXES | i == SBMP_OLD_CHECKBOXES)
            k++ ;

        lLen = sprintf( szString, "%3ld %s", i, szBuffer) ;
        pt.x = 2L + ( i > LIMIT) * xHalf ;
        pt.y = yStart - k * HEIGHT ;
        GpiCharStringAt( hps, &pt, lLen, szString) ;

        hbm = WinGetSysBitmap( HWND_DESKTOP, i) ;
        pt.x = xHalf - xHalf / 5 + ( i > LIMIT) * xHalf ;
        WinDrawBitmap( hps, hbm, NULL, &pt, CLR_BLACK, CLR_WHITE, DBM_NORMAL) ;
        if( i == LIMIT)
            k = -1 ;
        k++ ;
    }
}
