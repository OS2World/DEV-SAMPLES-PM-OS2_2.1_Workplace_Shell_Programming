// GOTO.C - Salto a una linea del documento in MDI EDIT
// Listing 13-04

// Stefano Maruzzi 1993

#define INCL_WIN

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include "pmedit.h"



MRESULT EXPENTRY Goto(  HWND hwnd,
                        ULONG msg,
                        MPARAM mp1,
                        MPARAM mp2)
{
    static HWND hwndMLE ;
    static ULONG ulLines, ulGoto ;
    static USHORT usMax, usPos ;
    CHAR szString[ 50] ;

    switch( msg)
    {
        case WM_INITDLG:
        {
            HWND hwndClient, hwndSpin ;
            LONG lRange ;

            // MDI AREA window handle
            hwndClient = *((PHWND)PVOIDFROMMP( mp2)) ;

            // center the dialog
            MieCenterDlg( hwndClient, hwnd) ;

            // line count
            ulLines = (ULONG)WinSendMsg( pdoc -> mle, MLM_QUERYLINECOUNT, 0L, 0L) ;

            // spinbutton handle
            hwndSpin = CTRL( hwnd, DL_LINE) ;

            // scrolling range
            WinSendMsg( hwndSpin, SPBM_SETLIMITS, MPFROMLONG( ulLines), MPFROMLONG( 1L)) ;
            WinSendMsg( hwndSpin, SPBM_SETCURRENTVALUE, MPFROMLONG(1L), 0L) ;

            // get the MLE window handle
            hwndMLE = pdoc -> mle ;

            // query the scrolling range
            lRange = (LONG)WinSendMsg( CTRL( hwndMLE, 0x0014), SBM_QUERYRANGE, 0L, 0L) ;

            // max scroll
            usMax = HIUSHORT( lRange) ;

            // always start from 1
            ulGoto = 1L ;
        }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) ->cmd)
            {
                case DID_OK:
                {
                    HWND hwndSpin, hwndTxt ;
                    ULONG ulData, ulGoto ;

                    hwndSpin = CTRL( hwnd, DL_LINE) ;

                    WinSendMsg( hwndSpin, SPBM_QUERYVALUE,
                                (MPARAM)&ulData,
                                MPFROM2SHORT( 0, SPBQ_UPDATEIFVALID)) ;

                    // determine where to go
                    ulGoto = (float)ulData / (float)ulLines * usMax ;

                    WinSendMsg( hwndMLE, WM_VSCROLL,
                                MPFROMSHORT( 0x0014),
                                MPFROM2SHORT( (USHORT)ulGoto, SB_SLIDERPOSITION)) ;

                    WinDismissDlg( hwnd, TRUE) ;
                }
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

