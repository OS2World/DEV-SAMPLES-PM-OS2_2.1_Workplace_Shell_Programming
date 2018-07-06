// SLICED.C - Slider
// Listing 07-12

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "slider.h"

// macro definitions
#define PAPA( x)        WinQueryWindow( hwnd, QW_PARENT)
#define MENU( x)        WinWindowFromID( hwnd, FID_MENU)
#define HAB( x)         WinQueryAnchorBlock( x)
#define CTRL( x, y)     WinWindowFromID( x, y)

#define CT_SLIDER       1000
#define CT_PROGRESS     1001
#define MAXSCALE1       16
#define MAXSCALE2       5

#define STARTX          10
#define STARTY          5

#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 15] ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load class name from resource file
    WinLoadString(  hab, NULLHANDLE,
                    ST_CLASSNAME,
                    sizeof( szClassName), szClassName) ;
    WinLoadString(  hab, NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof( szWindowTitle), szWindowTitle) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW | CS_SYNCPAINT, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName, szWindowTitle,
                                    WS_CLIPCHILDREN,
                                    NULLHANDLE, RS_ALL,
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
    static COLOR clr ;

    switch( msg)
    {
        case WM_CREATE:
        {
            HWND hwndSlider ;
            SLDCDATA sldcd ;
            LONG i ;
            CHAR szString[ 10] ;

            sldcd.cbSize = sizeof sldcd ;
            sldcd.usScale1Increments = MAXSCALE1 ;
            sldcd.usScale1Spacing = 0 ;             // provided by WC_SLIDER
            sldcd.usScale2Increments = MAXSCALE2 ;
            sldcd.usScale2Spacing = 0 ;             // provided by WC_SLIDER

            hwndSlider = WinCreateWindow(   hwnd, WC_SLIDER,
                                            NULL,
                                            SLS_BUTTONSRIGHT | SLS_BOTTOM |
                                            SLS_HORIZONTAL | SLS_RIBBONSTRIP |
                                            SLS_SNAPTOINCREMENT | WS_VISIBLE,
                                            STARTX, STARTY, 0, 0,
                                            hwnd, HWND_TOP, CT_SLIDER,
                                            (PVOID)&sldcd, NULL) ;
            WinSendMsg( hwndSlider, SLM_SETTICKSIZE,
                        MPFROM2SHORT( SMA_SETALLTICKS, 10), (MPARAM)0L) ;
            for( i = 0; i < MAXSCALE1; i++)
            {
                sprintf( szString, "%ld", i) ;
                WinSendMsg( hwndSlider, SLM_SETSCALETEXT,
                            MPFROMSHORT( i), MPFROMP( szString)) ;
            }

            sldcd.cbSize = sizeof sldcd ;
            sldcd.usScale1Increments = MAXSCALE1 ;
            sldcd.usScale1Spacing = 0 ;             // provided by WC_SLIDER
            sldcd.usScale2Increments = 0 ;
            sldcd.usScale2Spacing = 0 ;             // provided by WC_SLIDER

            hwndSlider = WinCreateWindow(   hwnd, WC_SLIDER,
                                            NULL,
                                            SLS_CENTER | SLS_HORIZONTAL | SLS_RIBBONSTRIP |
                                            SLS_HORIZONTAL | SLS_OWNERDRAW |
                                            SLS_SNAPTOINCREMENT | SLS_READONLY | SLS_BUTTONSRIGHT,
                                            STARTX, STARTY * 14, 0, 80,
                                            hwnd, HWND_TOP, CT_PROGRESS,
                                            (PVOID)&sldcd, NULL) ;
        }
            break ;

        case WM_SIZE:
        {
            HWND hwndSlider ;
            SHORT cx ;

            hwndSlider = WinWindowFromID( hwnd, CT_SLIDER) ;
            cx = SHORT1FROMMP( mp2) - STARTX * 2 ;

            WinSetWindowPos(    hwndSlider, HWND_TOP,
                                0, 0, cx, 55,
                                SWP_SIZE) ;
            hwndSlider = WinWindowFromID( hwnd, CT_PROGRESS) ;
            WinSetWindowPos(    hwndSlider, HWND_TOP,
                                0, 0, cx, 80,
                                SWP_SHOW | SWP_SIZE) ;
        }
            break ;

        case WM_DRAWITEM:
        {
            POWNERITEM poi = (POWNERITEM)mp2 ;

            switch( LONGFROMMP( mp1))
            {
                case CT_PROGRESS:
                {
                    switch( poi -> idItem)
                    {
                        case SDA_SLIDERSHAFT:
break ;
                            WinFillRect( poi -> hps, &poi -> rclItem, CLR_DARKGRAY) ;
                            return (MRESULT)TRUE ;

                        case SDA_RIBBONSTRIP:
                        {
                            COLOR clr ;
                            RECTL rc ;

                            clr = (COLOR)WinSendMsg(poi -> hwnd, SLM_QUERYSLIDERINFO,
                                                    MPFROM2SHORT( SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE),
                                                    0L) ;

                            rc.xRight = rc.yBottom = 0L ;
                            rc.xLeft = poi -> rclItem.xLeft ;
                            rc.yTop = poi -> rclItem.yTop ;

                            WinFillRect( poi -> hps, &rc, clr) ;
                            WinFillRect( poi -> hps, &poi -> rclItem, clr) ;
                        }
                            return (MRESULT)TRUE ;

                        case SDA_SLIDERARM:
                            break ;

                        case SDA_BACKGROUND:
                            break ;
                            WinFillRect( poi -> hps, &poi -> rclItem, CLR_PALEGRAY) ;
                            return (MRESULT)TRUE ;
                    }
                }
                    break ;

                default:
                    break ;
            }
        }
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, 0, &rc) ;
            WinFillRect( hps, &rc, clr) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                    break ;
            }
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1 ))
            {
                case CT_SLIDER:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case SLN_CHANGE:
                        {
                            clr = (COLOR)WinSendMsg(CTRL( hwnd, CT_SLIDER), SLM_QUERYSLIDERINFO,
                                                    MPFROM2SHORT( SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE),
                                                    0L) ;
                            WinInvalidateRect( hwnd, NULL, FALSE) ;

                            // show bar in the CT_PROGRESS slider
                            WinSendMsg( CTRL( hwnd, CT_PROGRESS), SLM_SETSLIDERINFO,
                                        MPFROM2SHORT( SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE),
                                        MPFROMSHORT( clr)) ;
                        }
                            break ;
                    }
                    break;
            }
            break;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
