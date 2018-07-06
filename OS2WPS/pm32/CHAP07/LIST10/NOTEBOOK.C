// NOTEBOOK.C - Notebook
// Listing  07-10

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include "notebook.h"

#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define MENU( x)        WinWindowFromID( x, FID_MENU)
#define HAB( x)         WinQueryAnchorBlock( x)

#define CX_TAB     80
#define CY_TAB     60

#define WM_PASSPROC     WM_USER + 0
#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
ULONG InsertPage(   HWND hwndNotebook, HWND hwndPage, PCHAR pszStatusLine,
                    LONG bkaValue, PVOID pObject, LONG bkaType) ;
int main( void) ;

int main( void)
{
    CHAR szWindowTitle[ 25] ;
    HWND hwndFrame, hwndClient ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE ;
    PFNWP pfnwp ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinLoadString( hab, NULLHANDLE, ST_WINDOWTITLE, sizeof( szWindowTitle), szWindowTitle) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &flFrameFlags,
                                    WC_NOTEBOOK,
                                    szWindowTitle,
                                    BKS_BACKPAGESBR | BKS_MAJORTABRIGHT |
                                    BKS_SQUARETABS | BKS_STATUSTEXTLEFT | BKS_SPIRALBIND,
                                    NULLHANDLE, RS_ICON,
                                    &hwndClient) ;
    // set the notebook owner
    WinSetOwner( hwndClient, hwndFrame) ;

    //  subclass the notebook
    pfnwp = WinSubclassWindow( hwndClient, ClientWndProc) ;
    WinSendMsg( hwndClient, WM_PASSPROC, MPFROMP( pfnwp), 0L) ;

    WinShowWindow( hwndFrame, TRUE) ;

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
    static PFNWP pfnwp ;

    switch( msg)
    {
        case WM_PASSPROC:
        {
            LONG i, j ;
            HBITMAP hbm ;
            HPS hps ;
            CHAR szStatusLine[ 30] ;
            CHAR *szCountries[] = { "United Kingdom", "United States", "Italy", "Japan", "France"} ;

            pfnwp = (PFNWP)mp1 ;

            hps = WinGetPS( hwnd) ;
            for( i = 1; i < 6; i++)
            {
                // load bitmap for major tab
                hbm = GpiLoadBitmap( hps, NULLHANDLE, i + RS_FLAG, 0L, 0L) ;
                // prepare text
                sprintf( szStatusLine, " %s - Master page", szCountries[ i - 1]) ;
                // insert page
                InsertPage( hwnd, NULLHANDLE, szStatusLine, BKA_BITMAP, (PVOID)&hbm, BKA_MAJORTAB) ;
 
                for( j = 1; j < 12; j++)
                {
                    // load bitmap for minor tab
                    hbm = GpiLoadBitmap( hps, NULLHANDLE, i + RS_MAST, 0L, 0L) ;
                    // prepare text
                    sprintf( szStatusLine, "%s - Page %ld-%ld", szCountries[ i - 1], i, j) ;
                    // insert page
                    InsertPage( hwnd, NULLHANDLE, szStatusLine, BKA_BITMAP, (PVOID)&hbm, BKA_MINORTAB) ;
                }
            }
            WinReleasePS( hps) ;
        }
            return 0L ;

        case WM_CLOSE:
            WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
            return (MRESULT)TRUE ;

            default:
        break ;
    }
    return (pfnwp)( hwnd, msg, mp1, mp2) ;
}

ULONG InsertPage(   HWND hwndNotebook,
                    HWND hwndPage,
                    PCHAR pszStatusLine,
                    LONG bkaValue,
                    PVOID pObject,
                    LONG bkaType)
{
    ULONG ulPageID ;
    SHORT sStyle = BKA_AUTOPAGESIZE ;

    switch( bkaType)
    {
        case BKA_MINORTAB:
            sStyle |= BKA_MINOR ;
            break ;

        case BKA_MAJORTAB:
            sStyle |= BKA_MAJOR ;
            break ;
    }

    sStyle |= ( pszStatusLine) ? BKA_STATUSTEXTON : 0L ;

    // insert a page
    ulPageID =(ULONG) WinSendMsg(   hwndNotebook, BKM_INSERTPAGE,
                                    0L, MPFROM2SHORT( sStyle, BKA_LAST)) ;

    switch( bkaValue)
    {
        case BKA_BITMAP:
        {
            HBITMAP hbm ;

            hbm = *(PHBITMAP)pObject ;
            // Set the size of the tab text
            WinSendMsg( hwndNotebook, BKM_SETDIMENSIONS,
                        MPFROM2SHORT( CX_TAB, CY_TAB),
                        MPFROMSHORT( bkaType)) ;
            // Set the tab text
            WinSendMsg( hwndNotebook, BKM_SETTABBITMAP,
                        MPFROMP( ulPageID ), MPFROMLONG( hbm)) ;
        }
            break ;

        case BKA_TEXT:
            // Set the size of the tab text
            WinSendMsg( hwndNotebook, BKM_SETDIMENSIONS,
                        MPFROM2SHORT( CX_TAB, CY_TAB),
                        MPFROMSHORT( bkaType)) ;

            // Set the tab text
            WinSendMsg( hwndNotebook, BKM_SETTABTEXT,
                        MPFROMP( ulPageID ), MPFROMP( (PSZ)pObject)) ;
            break ;
    }

    if( pszStatusLine)
        // Set the status line text
        WinSendMsg( hwndNotebook, BKM_SETSTATUSLINETEXT,
                    MPFROMP( ulPageID ),
                    MPFROMP( pszStatusLine));

    if( hwndPage)
        // Associate the app page window with the notebook page
        WinSendMsg(     hwndNotebook, BKM_SETPAGEWINDOWHWND,
                        MPFROMP( ulPageID),
                        MPFROMLONG( hwndPage)) ;

    return ulPageID ;
}

