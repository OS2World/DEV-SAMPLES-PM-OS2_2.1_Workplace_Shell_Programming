// DROP.C - Drop
// Listing 12-02

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "drop.h"
#include "mie01.h"


// definitions & macros
#define CTRL( x, y) WinWindowFromID( x, y)
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)

#define WM_PASSPROC WM_USER + 0

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX
#define CT_MLE      1000


// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY FrameWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "DROP" ;
    CHAR szWindowTitle[] = "Drop" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    PFNWP pfnwp ;
    ULONG ulFCFrame = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // OWNER class
    if( !WinRegisterClass(  hab, szClassName,
                            ClientWndProc,
                            CS_SIZEREDRAW | CS_SYNCPAINT, 0L))
        WinAlarm( HWND_DESKTOP, WA_NOTE) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &ulFCFrame,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE,
                                    RS_OBJ,
                                    &hwndClient) ;
    // position the window
    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        500, 500, 200, 150,
                        SWP_SHOW | SWP_SIZE | SWP_MOVE) ;

    // suclass the frame window
    pfnwp = WinSubclassWindow( hwndFrame, FrameWndProc) ;
    WinSendMsg( hwndFrame, WM_PASSPROC, (MPARAM)pfnwp, 0L) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg( hab, &qmsg) ;

    WinDestroyWindow( hwndFrame) ;
    WinDestroyMsgQueue( hmq) ;
    WinTerminate( hab) ;

    return 0L ;
}

MRESULT EXPENTRY FrameWndProc(  HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static PFNWP pfnwp ;

    switch( msg)
    {
        case WM_PASSPROC:
            pfnwp = (PFNWP) mp1 ;
            return (MPARAM) 0L ;

        default:
            break ;
    }
    return (* pfnwp)( hwnd, msg, mp1, mp2) ;
}



MRESULT EXPENTRY ClientWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static RECTL rc ;
    static HWND hwndMLE ;

    switch( msg)
    {
        case WM_CREATE:
        {
            // create the mle window
            hwndMLE = WinCreateWindow(  hwnd, WC_MLE,
                                        NULL,
                                        MLS_VSCROLL | MLS_HSCROLL,
                                        0L, 0L, 0L, 0L,
                                        hwnd, HWND_TOP,
                                        CT_MLE,
                                        NULL, NULL) ;
            WinSetFocus( HWND_DESKTOP, hwndMLE) ;
        }
            break ;

        case WM_SIZE:
            WinSetWindowPos(    hwndMLE, HWND_TOP,
                                0L, 0L,
                                SHORT1FROMMP( mp2), SHORT2FROMMP( mp2),
                                SWP_SIZE) ;

            rc.xLeft = 1L ;
            rc.yBottom = 1L ;
            rc.xRight = SHORT1FROMMP( mp2) - 1 ;
            rc.yTop = SHORT2FROMMP( mp2) - 1 ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        case DM_DRAGOVER:
        {
            PDRAGINFO pdrginfo ;
            PDRAGITEM pdrgitem ;
            USHORT usDOR, usDO ;
            HPS hps ;

            // pointer to the DRAGINFO structure
            pdrginfo = (PDRAGINFO)mp1 ;

            if( !DrgAccessDraginfo( pdrginfo))
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                // free the info block
                DrgFreeDraginfo( pdrginfo) ;
                break ;
            }
            // access the first DRAGITEM object
            pdrgitem = DrgQueryDragitemPtr( pdrginfo, 0L) ;

            // check if it is coming from a different window
            if( pdrginfo -> hwndSource == hwnd)
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                // free the info block
                DrgFreeDraginfo( pdrginfo) ;
                break ;
            }

            // verify the type
            if( !DrgVerifyType( pdrgitem, DRT_TEXT))
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                // free the info block
                DrgFreeDraginfo( pdrginfo) ;
                break ;
            }
            // verify the mechanism and the format

            if( !DrgVerifyRMF( pdrgitem, "DRM_OS2FILE", "DRF_TEXT"))
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                // free the info block
                DrgFreeDraginfo( pdrginfo) ;
                break ;
            }

            switch( pdrginfo -> usOperation)
            {
                case DO_DEFAULT:
                    usDOR = DOR_DROP ;
                    usDO = DO_MOVE ;
                    break ;

                case DO_UNKNOWN:
                    usDOR = DOR_DROP ;
                    usDO = DO_COPY ;
                    break ;

                case DO_COPY:
                    usDOR = DOR_DROP ;
                    usDO = DO_COPY ;
                    break ;

                case DO_MOVE:
                    usDOR = DOR_DROP ;
                    usDO = DO_MOVE ;
                    break ;

                case DO_LINK:
                    usDOR = DOR_DROP ;
                    usDO = DO_LINK ;
                    break ;

                case DO_CREATE:
                    usDOR = DOR_NODROPOP ;
                    usDO = 0x0000 ;
                    break ;
            }

            // get the presentation space handle
            hps = DrgGetPS( hwnd) ;

            if( usDOR == DOR_DROP)
            {
                MieEmphasizeWindow( hwnd, hps) ;
            }
            else
            {
                GpiErase( hps) ;
            }
            // release the handle
            DrgReleasePS( hps) ;

            // free the info block
            DrgFreeDraginfo( pdrginfo) ;

            // return value
            return MRFROM2SHORT( usDOR, usDO) ;
        }
            break ;

        case DM_DRAGLEAVE:
        {
            HPS hps ;

            // de-enphasize the target
            hps = DrgGetPS( hwnd) ;
            GpiErase( hps) ;
            DrgReleasePS( hps) ;
        }
            return (MRESULT)NULL ;

        case DM_DROP:
        {
            PDRAGINFO pdrginfo = (PDRAGINFO)mp1;
            PDRAGITEM pdrgitem ;
            ULONG ulItem, ulBytesRead, ulAction ;
            HPS hps ;
            CHAR szContainer[ 260], szFileName[ 80], szSourceFile[ 80] ;
            APIRET rc ;
            HWND hwndMLE = CTRL( hwnd, CT_MLE) ;
            FILESTATUS3 fs3 ;
            LONG lOffset = 0L ;
            HFILE hfile ;
            PCHAR pBuffer ;

            // remove the enphatization border
            hps = DrgGetPS( hwnd) ;
            GpiErase( hps) ;
            DrgReleasePS( hps) ;

            if( !DrgAccessDraginfo( pdrginfo))
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                break ;
            }

            // #of DRAGITEM structures
            ulItem = DrgQueryDragitemCount( pdrginfo) ;
            pdrgitem = DrgQueryDragitemPtr( pdrginfo, 0L) ;

            // retrieve the source container name
            DrgQueryStrName(    pdrgitem -> hstrContainerName,
                                sizeof( szContainer), szContainer) ;

            // retrieve the source file name
            DrgQueryStrName(    pdrgitem -> hstrSourceName,
                                sizeof( szSourceFile), szSourceFile) ;

            // retrieve the file name
            DrgQueryStrName(    pdrgitem -> hstrTargetName,
                                sizeof( szFileName), szFileName) ;

            // create full pathname
            strcat( szContainer, szSourceFile) ;

            // open the source file           
            if( rc = DosOpen(   szContainer, &hfile,
                                &ulAction,
                                0L,
                                FILE_NORMAL,
                                OPEN_ACTION_OPEN_IF_EXISTS,
                                OPEN_ACCESS_READONLY | OPEN_SHARE_DENYREAD | OPEN_FLAGS_SEQUENTIAL,
                                NULL))
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                return FALSE ;
            }

            // query file info
            rc = DosQueryFileInfo(  hfile, FIL_STANDARD,
                                    (PVOID)&fs3, sizeof( fs3)) ;
            if( rc || !fs3.cbFileAlloc)
                return FALSE ;
                                       
            rc = DosAllocMem( (PPVOID)&pBuffer, fs3.cbFileAlloc, PAG_WRITE | PAG_COMMIT) ;

            if( rc)
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                DosClose( hfile) ;
                return FALSE ;
            }

            // read the file
            if( rc = DosRead(   hfile, (PVOID)pBuffer,
                                fs3.cbFile, &ulBytesRead))
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                DosFreeMem( pBuffer) ;
                DosClose( hfile) ;
                return FALSE ;
            }

            // se the import buffer
            WinSendMsg( hwndMLE, MLM_SETIMPORTEXPORT,
                        MPFROMP( pBuffer),
                        MPFROMLONG( fs3.cbFile)) ;

            WinSendMsg( hwndMLE, MLM_IMPORT,
                        MPFROMP( (PIPT)&lOffset),
                        MPFROMP( (PULONG)&fs3.cbFile)) ;

            // free memory and close the file
            DosFreeMem( pBuffer) ;
            DosClose( hfile) ;

            // show the MLE
            WinShowWindow( hwndMLE, TRUE) ;

            // delete string handles
            DrgDeleteDraginfoStrHandles( pdrginfo) ;

            // free the info block
            DrgFreeDraginfo( pdrginfo) ;
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

