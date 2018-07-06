// MIE01.DLL - Extending the OS/2 PM API
// Listing 10-05

// Stefano Maruzzi 1993


#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include "mie01.h"

// macros
#define HAB( x) WinQueryAnchorBlock( x)


ULONG _System _DLL_InitTerm( ULONG hmodule, ULONG flag)
{
    switch( flag)
    {
        case 0: // initilize
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
    WinInflateRect( HAB( hwnd), prc, -25L, -25L) ;
    WinDrawBorder(  hps, prc, 2, 2, CLR_WHITE, clr, DB_STANDARD) ;

    // move to the next color
    if( clr <= 15)
        clr++ ;
    else
        clr = 1 ;

    return ( clr - 1) ;
}


void EXPENTRY MieEmphasizeWindow( HWND hwnd, HPS hps)
{
    RECTL rc ;
    LONG rndx, rndy ;

    // get the window rectangle
    WinQueryWindowRect( hwnd, &rc) ;

    // inflate it by 4 on both axis
    WinInflateRect( HAB( hwnd), &rc, - 4L, -4L) ;

    // calculate rounded corner (10% of each length)
    rndx = ( rc.xRight - rc.xLeft) / 10 ;
    rndy = ( rc.yTop -rc.yBottom) / 10 ;

    // set appropriate output mode
    GpiSetMix( hps, FM_INVERT) ;
    // set the color
    GpiSetColor( hps, CLR_BLACK) ;
    // set current position
    GpiSetCurrentPosition( hps, (PPOINTL)&rc + 1) ;
    // draw border
    GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rc, rndx, rndy) ;

    return ;
}
