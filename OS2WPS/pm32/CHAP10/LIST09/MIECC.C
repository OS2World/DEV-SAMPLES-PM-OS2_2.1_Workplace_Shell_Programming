// MIECC.C
// LIST 10-09

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_DOS
#define INCL_GPI

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "miecc.h"


ULONG _System _DLL_InitTerm( ULONG hmodule, ULONG flag)
{
    switch( flag)
    {
        case 0: // initilize
            if( !WinRegisterClass(  NULLHANDLE, "TWENY",
                                    TwenyWndProc,
                                    CS_SIZEREDRAW | CS_PUBLIC, sizeof( ULONG)))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
            break ;

        case 1: // terminate
            break ;
    }
    return 1L ;
}


MRESULT EXPENTRY TwenyWndProc(  HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    HWND hwndOwner ;
    SHORT sID ;

    // owner handle
    hwndOwner = WinQueryWindow( hwnd, QW_OWNER) ;

    // window ID
    sID = WinQueryWindowUShort( hwnd, QWS_ID) ;

    switch( msg)
    {
        case WM_CREATE:
            // store the initial color
            WinSetWindowULong( hwnd, QWL_USER, 0x00FFFFFF) ;
            break ;

        case WM_PRESPARAMCHANGED:
        {
            ULONG pp = (ULONG)HWNDFROMMP( mp1) ;
            COLOR clr ;

            WinQueryPresParam(  hwnd,
                                pp,
                                0L,
                                NULL,
                                sizeof( COLOR),
                                (PULONG)&clr,
                                QPF_NOINHERIT) ;

            WinSetWindowULong( hwnd, QWL_USER, clr) ;
            WinInvalidateRect( hwnd, NULL, TRUE) ;
        }
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;
            COLOR clr ;

            clr = (COLOR)WinQueryWindowULong( hwnd, 0L) ;
            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            GpiCreateLogColorTable( hps, LCOL_RESET, LCOLF_RGB, 0L, 0L, (PLONG)NULL) ;
            WinFillRect( hps, &rc, clr) ;
            WinEndPaint( hps ) ;

            // notify to the owner
            if( hwndOwner)
                WinSendMsg( hwndOwner, WM_CONTROL,
                            MPFROM2SHORT( sID, TN_CHANGE),
                            MPFROMHWND( hwnd)) ;
        }
            break ;

        case WM_BUTTON1DOWN:
        {
            COLOR clr ;

            // get the stored color
            clr = (COLOR)WinQueryWindowULong( hwnd, 0) ;
            ( clr > 14) ? clr = 1 : clr++ ;

            // store the new color
            WinSetWindowULong( hwnd, 0, clr) ;
            WinInvalidateRect( hwnd, NULL, TRUE) ;
            WinUpdateWindow( hwnd) ;
        }
            break ;

        case TM_QUERYCOLOR:
            return (MRESULT)WinQueryWindowULong( hwnd, 0L) ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
