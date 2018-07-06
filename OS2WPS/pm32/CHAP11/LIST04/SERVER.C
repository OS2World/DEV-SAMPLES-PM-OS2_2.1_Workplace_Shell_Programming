// SERVER.C - A DDE server application
// Listing 11-04

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL
#define INCL_DOSRESOURCES
#define INCL_DOSMEMMGR

#include <os2.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "ddedata.h"

// macros
#define PAPA( x)            WinQueryWindow( x, QW_PARENT)
#define HAB( x)             WinQueryAnchorBlock( x)
#define GETRNDM( min, max ) ((rand() % (int)(((max)+1) - (min))) + (min))

#define WM_PASSPROC         WM_USER + 0

#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

#define PMSERVER    "PMSERVER"
#define PMCLIENT    "PMCLIENT"


// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY FrameWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
PDDESTRUCT WinAllocDdeBlock(    HWND hwndDest,
                                PSZ pszItemName,
                                USHORT fsStatus,
                                USHORT usFormat,
                                PVOID pabData,
                                ULONG ulDataLen) ;

int main( void)
{
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_SHELLPOSITION ;
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 25] ;
    PFNWP pfnwp ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load class & title
    WinLoadString( hab, NULLHANDLE, ST_CLASSNAME, sizeof( szClassName), szClassName) ;
    WinLoadString( hab, NULLHANDLE, ST_WINDOWTITLE, sizeof( szWindowTitle), szWindowTitle) ;

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
                                    RS_ICON,
                                    &hwndClient) ;

    // subclass the frame window
    pfnwp = WinSubclassWindow( hwndFrame, FrameWndProc) ;
    WinSendMsg( hwndFrame, WM_PASSPROC, MPFROMP( pfnwp), 0L) ;

    // position it
    WinSetWindowPos( hwndFrame, HWND_TOP, 10, 10, 200, 200, SWP_ZORDER | SWP_SIZE | SWP_MOVE) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
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
    static HWND hwndClient ;
    static USHORT sTitleID, usMin, usMax ;
    static PFNWP pfnwp ;

    switch( msg)
    {
        case WM_PASSPROC:
            pfnwp = (PFNWP)mp1 ;
            return 0L ;

        case WM_TIMER:
        {
            STOCKS stocks ;
            CHAR szTitleID[ 10] ;
            PDDESTRUCT pddes ;

            // define random number
            stocks.rValue = (float)GETRNDM( usMin, usMax) / (float)FACTOR ;
            stocks.usMin = GETRNDM( 890, 978) ;
            stocks.usMax = GETRNDM( 1070, 1125) ;

            sprintf( szTitleID, "%d", sTitleID) ;

            // prepare the memory block to be sent to the client
            pddes = WinAllocDdeBlock(   hwndClient, szTitleID,
                                        0, DDEFMT_TEXT,
                                        &stocks, sizeof( STOCKS)) ;

            // post the handle to the client
            WinDdePostMsg( hwndClient, hwnd, WM_DDE_DATA, pddes, TRUE) ;
        }
            break ;

        case WM_CLOSE:
            if( hwndClient)
            {
                WinDdePostMsg( hwndClient, hwnd, WM_DDE_TERMINATE, NULL, TRUE) ;
                hwndClient = NULLHANDLE ;
            }
            break ;

        case WM_DDE_TERMINATE:
            if( hwndClient)
                WinDdePostMsg( hwndClient, hwnd, WM_DDE_TERMINATE, NULL, TRUE) ;
            break ;

        case WM_DDE_INITIATE:
        {
            PDDEINIT pddei ;
            CONVCONTEXT conv ;

            conv.cb = sizeof( CONVCONTEXT) ;
            conv.fsContext = DDECTXT_CASESENSITIVE ;
            conv.idCountry = 39 ;
            conv.usCodepage = 437 ;
            conv.usLangID = 0 ;
            conv.usSubLangID = 0 ;

            // skip if we are already connected
            if( hwndClient)
                break ;

            // get the dde pointer
            pddei = (PDDEINIT)mp2 ;

// TMP
WinSetWindowText( hwnd, pddei -> pszAppName) ;

            // store the client HWND
            if( !strcmpi( pddei -> pszAppName, PMSERVER) && !hwndClient)
                hwndClient = HWNDFROMMP( mp1) ;

            // start the conversation
            WinDdeRespond( hwndClient, hwnd, PMCLIENT, pddei -> pszTopic, &conv) ;
        }
            break ;

        case WM_DDE_REQUEST:
        {
            PSZ p, pszString ;
            ULONG ulSize ;
            PDDESTRUCT pddes ;

            // retrieve the stock names from the resource file
            if( DosGetResource( NULLHANDLE, RS_STOCKS, ID_STOCKS, (PPVOID)&pszString))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // get the size
            ulSize = strlen( pszString) ;

            // extract the pddes pointer
            pddes = (PDDESTRUCT)mp2 ;

            // extract the pointer to the item name
            p = DDES_PSZITEMNAME( pddes) ;

            // check if we are requested the handle to the atom table
            if( strcmpi( "stocks", p) == 0 && pddes -> usFormat == DDEFMT_TEXT)
            {
                // prepare the memory block to be sent to the client
                pddes = WinAllocDdeBlock(   hwndClient, "stocks",
                                            0, DDEFMT_TEXT,
                                            pszString, ulSize) ;

                // post the handle to the client
                WinDdePostMsg( hwndClient, hwnd, WM_DDE_DATA, pddes, TRUE) ;

                DosFreeMem( pddes) ;
                DosFreeResource( pszString) ;
            }
        }
            break ;

        case WM_DDE_ADVISE:
        {
            PSZ pszString, p ;
            ULONG ulSize, i ;
            PDDESTRUCT pddes ;

            // skip if a permanent link is already established
            if( sTitleID)
                break ;

            pddes = (PDDESTRUCT) mp2 ;

            // retrieve the stock ID
            sTitleID = atoi( DDES_PSZITEMNAME( pddes)) ;

            // retrieve the stock values from the resource file
            if( DosGetResource( NULLHANDLE, RS_VALUES, ID_VALUES, (PPVOID)&pszString))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // get the segment size
            ulSize = strlen( pszString) ;

            p = pszString ;

            // extract min-max
            for( i = 0; i < sTitleID - 1; i++)
            {
                p = strchr( p, ',') + 1 ;
            }
            usMin = atoi( p) * FACTOR ;
            p = strchr( p, '-') + 1 ;
            usMax = atoi( p) * FACTOR ;

            // free the segment
            //DosFreeSeg( sel) ;

            // define the random base
            srand( (unsigned)time( NULL)) ;

            pddes = WinAllocDdeBlock(   hwndClient, "connected",
                                        DDE_FACK,
                                        DDEFMT_TEXT,
                                        NULL, 0) ;

            // post the handle to the client
            WinDdePostMsg( hwndClient, hwnd, WM_DDE_ACK, pddes, TRUE) ;

            // start timer for the receiving stock
            WinStartTimer(  HAB( hwnd), hwnd,
                            ID_TIMER,
                            1000) ;

            DosFreeMem( (PDDESTRUCT)mp2) ;
            DosFreeResource( pszString) ;
        }
            break ;

        case WM_DDE_UNADVISE:
        {
            sTitleID = 0 ;
            WinStopTimer( HAB( hwnd), hwnd, ID_TIMER) ;
            DosFreeMem( (PDDESTRUCT)mp2) ;
        }
            break ;

        default:
            break ;
    }
    return ( *pfnwp)( hwnd, msg, mp1, mp2) ;
}



MRESULT EXPENTRY ClientWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static HWND hwndClient ;
    static USHORT sTitleID, usMin, usMax ;

    switch( msg)
    {
        case WM_TIMER:
        {
            STOCKS stocks ;
            CHAR szTitleID[ 10] ;
            PDDESTRUCT pddes ;

            // define random number
            stocks.rValue = (float)GETRNDM( usMin, usMax) / (float)FACTOR ;
            stocks.usMin = GETRNDM( 890, 978) ;
            stocks.usMax = GETRNDM( 1070, 1125) ;

            sprintf( szTitleID, "%d", sTitleID) ;

            // prepare the memory block to be sent to the client
            pddes = WinAllocDdeBlock(   hwndClient, szTitleID,
                                        0,
                                        DDEFMT_TEXT,
                                        &stocks,
                                        sizeof( STOCKS)) ;

            // post the handle to the client
            WinDdePostMsg( hwndClient, hwnd, WM_DDE_DATA, pddes, TRUE) ;
        }
            break ;

        case WM_CLOSE:
            if( hwndClient)
            {
                WinDdePostMsg( hwndClient, hwnd, WM_DDE_TERMINATE, NULL, TRUE) ;
                hwndClient = NULLHANDLE ;
            }
            break ;

        case WM_CREATE:
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

PDDESTRUCT WinAllocDdeBlock(    HWND hwndDest,
                                PSZ pszItemName,
                                USHORT fsStatus,
                                USHORT usFormat,
                                PVOID pabData,
                                ULONG ulDataLen)
{
    ULONG ulItemLen ;
    PDDESTRUCT pddes ;

    if( pszItemName != 0L)
        ulItemLen = strlen( pszItemName) + 1 ;
    else
       ulItemLen = 0 ;

    if( !DosAllocSharedMem( (PPVOID)&pddes,
                            NULL,
                            sizeof( DDESTRUCT) + ulItemLen + ulDataLen,
                            PAG_COMMIT | PAG_READ | PAG_WRITE | OBJ_GIVEABLE))
    {
        PID pid ;
        TID tid ;

        pddes -> cbData = ulDataLen - 1 ;
        pddes -> fsStatus = fsStatus ;
        pddes -> usFormat = usFormat ;
        pddes -> offszItemName = sizeof( DDESTRUCT) ;

        if( ulDataLen && pabData)
            pddes->offabData = sizeof( DDESTRUCT) + ulItemLen ;
        else
            pddes -> offabData = sizeof( DDESTRUCT) ;

        if( pszItemName != NULL)
            strcpy( DDES_PSZITEMNAME( pddes) , pszItemName) ;

        memcpy( DDES_PABDATA( pddes), pabData, ulDataLen) ;

        // determine the client PID
        WinQueryWindowProcess( hwndDest, &pid, &tid) ;
        DosGiveSharedMem( (PVOID)pddes, pid, PAG_READ | PAG_WRITE) ;

        return pddes ;
    }
    return NULL ;
}
