// MSUDLL.DLL - A sample DLL
// Listing 10-05

// Stefano Maruzzi 1992

#define INCL_WIN
#define INCL_DOSSEMAPHORES
#define INCL_GPI

#include <os2.h>
#include "mie02.h"

// macros
#define HAB( x)     WinQueryAnchorBlock( x)
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define CTRL( x, y) WinWindowFromID( x, y)
#define SYS( x)     WinQuerySysValue( HWND_DESKTOP, x)

// global variables
HMODULE hmodDLL ;

ULONG _System _DLL_InitTerm( ULONG hmodule, ULONG flag)
{
    switch( flag)
    {
        case 0: // initilize
            hmodDLL = (HMODULE)hmodule ;
            break ;

        case 1: // terminate
            break ;
    }
    return 1L ;
}

COLOR EXPENTRY MieColorClient( HWND hwnd, HPS hps, PRECTL prc)
{
    static COLOR clr = 1 ;
    RECTL rc ;
    BOOL fRect ;

    WinQueryWindowRect( hwnd, &rc) ;
    fRect = WinEqualRect( HAB( hwnd), &rc, prc) ;

    if( !fRect)
    {
        WinValidateRect( hwnd, prc, TRUE) ;
        WinInvalidateRect( hwnd, NULL, TRUE) ;
        WinUpdateWindow( hwnd) ;
        return ( clr - 1) ;
    }

    WinFillRect( hps, prc, clr) ;
    WinInflateRect( HAB( hwnd), prc, -5, -5) ;
    WinDrawBorder( hps, prc, 2, 2, CLR_WHITE, clr, DB_STANDARD) ;

    // move to the next color
    if( clr <= 15)
        clr++ ;
    else
        clr = 1 ;

    return ( clr - 1) ;
}

MRESULT EXPENTRY MieCenterDlg(  HWND hwndClient,
                                HWND hwndDlg)
{
    RECTL rc1, rc2 ;
    POINTL ptl ;

    // get the owner size
    WinQueryWindowRect( hwndClient, &rc1) ;
    // get the dialog size
    WinQueryWindowRect( hwndDlg, &rc2) ;

    // calculate the dialog position
    ptl.x = (( rc1.xRight - rc1.xLeft) - ( rc2.xRight - rc2.xLeft)) / 2 ;
    ptl.y = (( rc1.yTop - rc1.yBottom) - ( rc2.yTop - rc2.yBottom)) / 2 ;

    // center the dialog window
    if( ptl.x > 0 && ptl.y > 0)
    {
        WinMapWindowPoints( hwndClient, HWND_DESKTOP, &ptl, 1) ;

        WinSetWindowPos(    hwndDlg, HWND_TOP,
                            ptl.x, ptl.y, 0L, 0L,
                            SWP_MOVE) ;
        return (MRESULT)TRUE ;
    }
    return (MRESULT)FALSE ;
}

HMODULE EXPENTRY InitTweny( void)
{
    CHAR szClassName[ 15] ;

    // load class name
    WinLoadString( NULLHANDLE, hmodDLL, ST_CLASSNAME, sizeof( szClassName), szClassName) ;

    // register a new window class
    if( !WinRegisterClass(  NULLHANDLE, szClassName,
                            TwenyWndProc,
                            CS_SIZEREDRAW | CS_SYNCPAINT, 4L))
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        return FALSE ;
    }

    return hmodDLL ;
}

MRESULT EXPENTRY TwenyWndProc(  HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static COLOR clr = 1 ;

    switch( msg)
    {
        case WM_CREATE:
            // store the initial color
            WinSetWindowULong( hwnd, 0, CLR_RED );
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, 0L, &rc) ;
            WinFillRect( hps, &rc, clr) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_BUTTON1DBLCLK:
        case WM_BUTTON1DOWN:
            ( clr > 15) ? clr = 2 : clr++ ;
            WinInvalidateRect( hwnd, NULL, TRUE) ;
            break ;

        case WM_CLOSE:
            WinDestroyWindow( PAPA( hwnd)) ;
            return FALSE ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

ULONG EXPENTRY MieDlgWnd( HWND hwndOwner, PDLGDATA pDlgData)
{
    return WinDlgBox( HWND_DESKTOP, hwndOwner, ProdInfoDlgProc, hmodDLL, DL_PRODINFO, (PVOID)pDlgData) ;
}

MRESULT EXPENTRY ProdInfoDlgProc(   HWND hwnd,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
    static PDLGDATA pDlgData ;

    switch( msg)
    {
        case WM_INITDLG:
        {
            // access the data
            pDlgData = (PDLGDATA)mp2 ;

            // center the dialog
            MieCenterDlg( hwnd, pDlgData -> hwndOwner) ;

            // set the title
            WinSetWindowText( hwnd, pDlgData -> szWindowTitle) ;
        }
            break ;

        case WM_BUTTON2DBLCLK:
        {
            PRECTL prc ;
            LONG cy = SYS( SV_CYTITLEBAR) + SYS( SV_CYDLGFRAME) * 2 + 2 ;
            APIRET apiret ;

            // allocate a page
            apiret = DosAllocMem( (PPVOID)&prc, 4096, PAG_READ | PAG_WRITE | PAG_COMMIT) ;
            if( apiret)
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // query the dialog QWL_USER area
            if( !WinQueryWindowULong( hwnd, QWL_USER))
            {
                SWP swp ;

                // query the window rectabgle
                WinQueryWindowPos( hwnd, &swp) ;

                WinSetWindowPos(    hwnd, HWND_TOP,
                                    swp.x, swp.y + swp.cy - cy,
                                    swp.cx, cy,
                                    SWP_MOVE | SWP_SIZE) ;

                // store the counter value in the dialog QWL_USER area
                WinSetWindowULong( hwnd, QWL_USER, (ULONG)prc) ;

                // move data to the block of memory
                prc -> xLeft = swp.x ;
                prc -> xRight = swp.cx ;
                prc -> yBottom = swp.y ;
                prc -> yTop = swp.cy ;
            }
            else
            {
                SWP swp ;
 
                // get the offset in the memory block
                prc = (PRECTL)WinQueryWindowULong( hwnd, QWL_USER) ;

                // get the current position
                WinQueryWindowPos( hwnd, &swp) ;

                WinSetWindowPos(    hwnd, HWND_TOP,
                                    swp.x,
                                    swp.y - prc -> yTop + swp.cy,
                                    prc -> xRight,
                                    prc -> yTop,
                                    SWP_MOVE | SWP_SIZE) ;
            }
         }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case DID_OK:
                case DID_CANCEL:
                    WinDismissDlg( hwnd, TRUE) ;
                    break ;

                default:
                    break ;
            }
            break ;

        default:
            break ;
    }
    return WinDefDlgProc( hwnd, msg, mp1, mp2) ;
}

void EXPENTRY MieEmphasizeWindow( HWND hwnd, HPS hps, PRECTL prc)
{
    RECTL rc ;
    LONG rndx, rndy ;
    BOOL fHPS ;

    if( !prc)
    {
        // get the window rectangle
        WinQueryWindowRect( hwnd, &rc) ;
        prc = &rc ;
    }

    // inflate it by 4 on both axis
    WinInflateRect( HAB( hwnd), prc, -4L, -4L) ;

    // calculate rounded corner (10% of each length)
    rndx = ( prc -> xRight - prc -> xLeft) / 10 ;
    rndy = ( prc -> yTop - prc -> yBottom) / 10 ;

    // if hps is NULLHANDLE, calculate it
    if( !hps)
    {
        hps = WinGetPS( hwnd) ;
        fHPS = TRUE ;
    }

    // set appropriate output mode
    GpiSetMix( hps, FM_INVERT) ;
    // set the color
    GpiSetColor( hps, CLR_BLACK) ;
    // set current position
    GpiSetCurrentPosition( hps, (PPOINTL)prc + 1) ;
    // draw border
    GpiBox( hps, DRO_OUTLINE, (PPOINTL)prc, rndx, rndy) ;

    // release the hps
    if( fHPS)
        WinReleasePS( hps) ;

    // enlarge it by 4 on both axis
    WinInflateRect( HAB( hwnd), prc, 4L, 4L) ;

    return ;
}
