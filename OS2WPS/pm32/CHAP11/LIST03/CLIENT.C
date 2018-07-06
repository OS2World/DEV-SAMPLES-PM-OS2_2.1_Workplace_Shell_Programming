// CLIENT.C - Client application in a DDE conversation
// Listing 11-03

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_WINDDE
#define INCL_GPI
#define INCL_DOS

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "client.h"
#include "ddedata.h"
#include "dialog.h"


// MACROS
#define HAB( x)             WinQueryAnchorBlock( x)
#define CTRL( x, y)         WinWindowFromID( x, y)
#define PAPA( x)            WinQueryWindow( x, QW_PARENT)
#define MENU( x)            WinWindowFromID( x, FID_MENU)
#define MIN( x, y)          ( (x) < (y)) ? (x) : (y)
#define MAX( x, y)          ( (x) < (y)) ? (y) : (x)

#define WM_PASSPROC         WM_USER + 0
#define WM_PASSSERVER       WM_USER + 1
#define WM_SHOWDATA         WM_USER + 2

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

#define PMSERVER        "PMSERVER"
#define PMCLIENT        "PMCLIENT"
#define NASDAQ          "NASDAQ"

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd,ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY FrameWndProc( HWND hwnd,ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY StockWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
PDDESTRUCT WinAllocDdeBlock(    HWND hwndDest, PSZ pszItemName, USHORT fsStatus,
                                USHORT usFormat, PVOID pabData, USHORT usDataLen) ;

int main( void)
{
    CHAR szClassName[ 15] ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load class name from resource file
    WinLoadString(  hab, NULLHANDLE, ST_CLASSNAME, sizeof( szClassName), szClassName) ;

    // register class
    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW | CS_SYNCPAINT, 0L) ;

    // create main window
    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName,
                                    "Client app",
                                    WS_CLIPCHILDREN,
                                    NULLHANDLE, RS_ALL,
                                    &hwndClient) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    // destroy resources
    WinDestroyWindow( hwndFrame) ;
    WinDestroyMsgQueue( hmq) ;
    WinTerminate( hab) ;

    return 0 ;
}

MRESULT EXPENTRY FrameWndProc(  HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static HWND hwndServer ;
    static TOPICS Topics ;
    static HWND hwndlist, hwndbutton, hwndtext ;
    static PFNWP pfnwp ;
    static HWND hwndClient ;

    switch( msg)
    {
        case WM_PASSPROC:
        {
            CONVCONTEXT conv ;
            pfnwp = (PFNWP)mp1 ;

            // application client window
            hwndClient = HWNDFROMMP( mp2) ;

            conv.cb = sizeof( CONVCONTEXT) ;
            conv.fsContext = DDECTXT_CASESENSITIVE ;
            conv.idCountry = 39 ;
            conv.usCodepage = 437 ;
            conv.usLangID = 0 ;
            conv.usSubLangID = 0 ;

            // store the window handle
            Topics.hwnd = hwnd ;

            // looking for a server pm_server nasdaq
            if( !WinDdeInitiate( hwnd, "PMSERVER", "NASDAQ", &conv))
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                break ;
            }

            // destroy the window if there is no server
            if( !hwndServer)
                WinDestroyWindow( hwnd) ;
        }
            return 0L ;

        case WM_DDE_ACK:
        {
            PDDESTRUCT pddes ;

            pddes = (PDDESTRUCT) mp2 ;

            // show the listbox
            hwndlist = CTRL( hwndClient, ID_LIST) ;
            // button hwnd
            hwndbutton = CTRL( MENU( PAPA( hwndClient)), MN_SETLINK) ;

            if( stricmp( DDES_PSZITEMNAME( pddes), "connected") == 0)
            {
                WinShowWindow( hwndlist, TRUE) ;
                WinSetWindowText( hwndbutton, "Stop Link") ;
                WinSetWindowUShort( hwndbutton, QWS_ID, MN_STOPLINK) ;
            }

            DosFreeMem( pddes) ;
        }
            break ;

        case WM_DDE_INITIATEACK:
        {
            PDDESTRUCT pddes ;
            PDDEINIT pddei ;
            HWND hwndtext = CTRL( hwndClient, ID_TEXT)  ;

            // skip if a server has previously been detected
            if( hwndServer)
                break ;

            // store the server HWND
            hwndServer = HWNDFROMMP( mp1) ;

            // pass the server HWND to the client window procedure
            WinSendMsg( hwndClient, WM_PASSSERVER, MPFROMHWND( hwndServer), 0L) ;

            // get pointer to the DDEINIT struct
            pddei = (PDDEINIT)mp2 ;

            // compare client name
            if( !stricmp( PMCLIENT, pddei -> pszAppName))
            {
                // allocate room for the DDESTRUCT & the segment
                pddes = WinAllocDdeBlock(   hwndServer, "stocks",
                                            0, DDEFMT_TEXT,
                                            NULL, 0) ;

                // post a request for the atom table handle
                WinDdePostMsg(  hwndServer, hwnd, WM_DDE_REQUEST, pddes, TRUE) ;
            }
            DosFreeMem( pddei) ;

            // activate the Query Nasdaq menuitem
            WinSendMsg( MENU( PAPA( hwndClient)), MM_SETITEMATTR,
                        MPFROM2SHORT( MN_QUERYNASDAQ, TRUE),
                        MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;
            WinSetWindowText( hwndtext, "NASDAQ") ;

            // show the text window
            WinShowWindow( hwndtext, TRUE) ;

            // hide listbox
            WinShowWindow( hwndlist, FALSE) ;

            // reset listbox content
            WinSendMsg( hwndlist, LM_DELETEALL, 0L, 0L) ;
        }
            break ;

        case WM_DDE_DATA:
        {
            PSZ p, p1 ;
            PDDESTRUCT pddes ;

            pddes = (PDDESTRUCT) mp2 ;

            // retrieve data
            p = (PSZ) DDES_PABDATA( pddes) ;

            // handle the initial transfer of stock info
            if( stricmp( DDES_PSZITEMNAME( pddes), "stocks") == 0)
            {
                // allocate room for the stocks data
                DosAllocMem( (PPVOID)&p1, (USHORT)pddes -> cbData, PAG_WRITE | PAG_COMMIT) ;

                // make a local copy of the data
                memcpy( p1, p, (size_t)pddes -> cbData) ;

                // store the pointer
                Topics.ptr = p1 ;

                // store the Topics address in the frame window free area
                WinSetWindowPtr( hwnd, QWL_USER, &Topics) ;
            }
            else
            {
                WinSendMsg( hwndClient, WM_SHOWDATA, MPFROMP( (PSTOCKS) DDES_PABDATA( pddes)), 0L) ;
            }
            // free the DDE memory block
            DosFreeMem( pddes) ;
        }
            break ;

        case WM_DDE_TERMINATE:
            if( hwndServer)
            {
                WinDdePostMsg( hwndServer, hwnd, WM_DDE_TERMINATE, NULL, TRUE) ;
                WinSetWindowText( hwnd, "Client App") ;

                // disable the query menu
                WinSendMsg( MENU( PAPA( hwndClient)), MM_SETITEMATTR,
                            MPFROM2SHORT( MN_REQUESTDATA, TRUE),
                            MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;
                WinSendMsg( MENU( PAPA( hwndClient)), MM_SETITEMATTR,
                            MPFROM2SHORT( MN_STATS, TRUE),
                            MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;
                hwndServer = NULLHANDLE ;

                // erase market & stock info
                WinSetWindowText( hwndtext, NULL) ;
            }
            break ;

        case WM_CLOSE:
            WinDdePostMsg(  hwndServer, hwnd, WM_DDE_TERMINATE, NULL, TRUE) ;
            hwndServer =  NULLHANDLE ;
            break ;

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
    static HWND hwndServer ;
    static POINTL aptl[ 70] = { 100, 0} ;
    static HWND hwndlist, hwndbutton, hwndtext, hwndstats, hwndfix ;
    static USHORT usdata ;
    static ULONG cx ;
    static HWND hwndDDE ;

    switch( msg)
    {
        case WM_PASSSERVER:
            hwndServer = HWNDFROMMP( mp1) ;
            return 0L ;

        case WM_DDE_DATA:
            WinAlarm( HWND_DESKTOP, WA_NOTE) ;
            break ;

        case WM_SHOWDATA:
        {
            PSTOCKS pstocks ;
            CHAR szTitle[ 10] ;

            pstocks = (PSTOCKS)mp1 ;

            *szTitle = ' ' ;
            sprintf( ( pstocks -> rValue < 100) ? szTitle + 1 : szTitle, "%4.2f", pstocks -> rValue) ;

            // insert datum in listbox
            WinSendMsg( hwndlist, LM_INSERTITEM, MPFROMSHORT( 0), MPFROMP( (PSZ) szTitle)) ;

            // insert min/max
            WinSendMsg( hwndlist, LM_SETITEMHANDLE,
                        MPFROMSHORT( 0), MPFROM2SHORT( pstocks -> usMin, pstocks -> usMax)) ;

            // calculate points
            aptl[ usdata].x = GRAPHSTART + usdata * STEP ;
            aptl[ usdata].y = (ULONG)pstocks -> rValue ;

            // increase counter
            usdata++ ;
            if( usdata < ( cx / STEP))
                WinInvalidateRect( hwnd, NULL, TRUE) ;
        }
            break ;

        case WM_CREATE:
        {
            ULONG ulStyle ;
            CHAR szFontName[] = "8.Courier" ;
            COLOR color ;

            // add the WS_CLIPCHILDREN style to the action bar
            ulStyle = WinQueryWindowULong( MENU( PAPA( hwnd)), QWL_STYLE) ;
            WinSetWindowULong( MENU( PAPA( hwnd)), QWL_STYLE, ulStyle | WS_CLIPCHILDREN) ;

            hwndlist = WinCreateWindow( hwnd, WC_LISTBOX,
                                        NULL,
                                        LS_NOADJUSTPOS,
                                        0, 0, 0, 0,
                                        hwnd, HWND_TOP,
                                        ID_LIST,
                                        NULL, NULL) ;

            WinSetPresParam( hwndlist, PP_FONTNAMESIZE, sizeof( szFontName), szFontName) ;
            color = (LONG) CLR_RED ;
            WinSetPresParam( hwndlist, PP_FOREGROUNDCOLORINDEX, sizeof( color), &color) ;

            hwndbutton = WinCreateWindow(   MENU( PAPA( hwnd)), WC_BUTTON,
                                            "Start link",
                                            BS_PUSHBUTTON | WS_VISIBLE | WS_DISABLED,
                                            250, 0, 170, 20,
                                            hwnd, HWND_TOP,
                                            MN_SETLINK,
                                            NULL, NULL) ;

            hwndtext = WinCreateWindow( MENU( PAPA( hwnd)), WC_STATIC,
                                        NULL,
                                        SS_TEXT,
                                        230, 1, 200, WINDOWHEIGHT,
                                        hwnd, HWND_TOP,
                                        ID_TEXT,
                                        NULL, NULL) ;

            hwndstats = WinCreateWindow(    hwnd, WC_STATIC,
                                            NULL,
                                            SS_TEXT,
                                            0, 0, 0, 0,
                                            hwnd, HWND_TOP,
                                            ID_STATS,
                                            NULL, NULL) ;

            hwndfix = WinCreateWindow(  hwnd, WC_STATIC,
                                        NULL,
                                        SS_TEXT,
                                        GRAPHSTART, 0, 240, 18,
                                        hwnd, HWND_TOP,
                                        ID_FIX,
                                        NULL, NULL) ;
        }
            break ;

        case WM_SIZE:
            WinSetWindowPos(    hwndlist, HWND_TOP,
                                0, 0, LISTWIDTH, SHORT2FROMMP( mp2),
                                SWP_SIZE) ;

            cx = SHORT1FROMMP( mp2) - GRAPHSTART ;

            WinSetWindowPos(    hwndstats, HWND_TOP,
                                GRAPHSTART, SHORT2FROMMP( mp2) - WINDOWHEIGHT,
                                SHORT1FROMMP( mp2) - GRAPHSTART, WINDOWHEIGHT,
                                SWP_SIZE | SWP_MOVE) ;
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case ID_LIST:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case LN_ENTER:
                        {
                            SHORT sPos ;
                            ULONG ulRange ;
                            float rMin, rMax, rValue ;
                            CHAR szValue[ 80] ;

                            sPos = (short)WinSendMsg(   hwndlist, LM_QUERYSELECTION,
                                                        MPFROMSHORT( LIT_FIRST), 0L) ;

                            ulRange = (ULONG)WinSendMsg(    hwndlist, LM_QUERYITEMHANDLE,
                                                            MPFROMSHORT( sPos), 0L) ;
                            WinSendMsg( hwndlist, LM_QUERYITEMTEXT,
                                        MPFROM2SHORT( sPos, sizeof( szValue)),
                                        MPFROMP( szValue)) ;

                            // calculate min, max, value
                            rValue = (float)atof( szValue) ;
                            rMin = rValue * (float)LOUSHORT( ulRange) / (float)1000 ;
                            rMax = rValue * (float)HIUSHORT( ulRange) / (float)1000 ;

                            // prepare output string
                            sprintf( szValue, "Low: %3.2f Value: %3.2f High: %3.2f", rMin, rValue, rMax) ;

                            // show values
                            WinSetWindowText( hwndfix, szValue) ;
                            WinShowWindow( hwndfix, TRUE) ;
                        }
                            break ;
                    }
                        break ;
            }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_NASDAQ:
                {
                    static LONG iCnt ;
                    PFNWP pfnwp ;
 
                    hwndDDE = WinCreateWindow(  HWND_DESKTOP, WC_FRAME,
                                                NULL,
                                                0L,
                                                0L, 0L, 0L, 0L,
                                                PAPA( hwnd), HWND_TOP,
                                                iCnt++,
                                                NULL, NULL) ;

                    // subclass the frame window
                    pfnwp = WinSubclassWindow( hwndDDE, FrameWndProc) ;
                    WinSendMsg( hwndDDE, WM_PASSPROC, MPFROMP( pfnwp), MPFROMHWND( hwnd)) ;
                }
                    break ;

                case MN_LINK:
                    break ;

                case MN_AMEX:
                    break ;

                case MN_NYSE:
                    break ;

                case MN_PRINT:
                    break ;

                case MN_EXIT:
                    WinDdePostMsg( hwndServer, hwnd, WM_DDE_TERMINATE, NULL, TRUE) ;
                    hwndServer =  NULLHANDLE ;
                    WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
                    break ;

                case MN_REQUESTDATA:
                    break ;

                case MN_STATS:
                {
                    USHORT usItems, i ;
                    CHAR szItem[ 20], szStats[ 200] ;
                    float avr = 0, stddev = 0, min = 999, max = 0 ;

                    usItems = (USHORT) WinSendMsg( hwndlist, LM_QUERYITEMCOUNT, 0L, 0L) ;

                    // calculate average, min and max
                    for( i = 0; i < usItems; i++)
                    {
                        WinSendMsg( hwndlist, LM_QUERYITEMTEXT,
                                    MPFROM2SHORT( i, sizeof( szItem)),
                                    MPFROMP( szItem)) ;
                        avr += atof( szItem) / (float)usItems ;
                        min = MIN( min, atof( szItem)) ;
                        max = MAX( max, atof( szItem)) ;
                    }

                    if( usItems > 1)
                    {
                        // calculate std dev
                        for( i = 0; i < usItems; i++)
                        {
                            WinSendMsg( hwndlist, LM_QUERYITEMTEXT,
                                        MPFROM2SHORT( i, sizeof( szItem)),
                                        MPFROMP( szItem)) ;
                            stddev += ( atof( szItem) - avr) * ( atof( szItem) - avr);
                        }
                        stddev = sqrt( (double)stddev) / (float)( usItems - 1) ;
                    }
                    // prepare buffer for the stats window
                    sprintf( szStats, "Avr: %3.2f Min: %3.2f Max: %3.2f Std.Dev.: %3.2f", avr, min, max, stddev) ;
                    WinSetWindowText( hwndstats, szStats) ;
                    WinShowWindow( hwndstats, TRUE) ;
                }
                    break ;

                case MN_QUERYNASDAQ:
                {
                    PTOPICS pTopics ;

                    // get the address to the TOPICS structure
                    pTopics = (PTOPICS)WinQueryWindowPtr( hwndDDE, QWL_USER) ;

                    // choose the stock
                    if( !WinDlgBox( HWND_DESKTOP, hwnd, StockWndProc,
                                    NULLHANDLE, ID_STOCKPROC, (PVOID)pTopics))
                        break ;

                    // check if we found a server
                    if( !hwndServer)
                    {
                        WinMessageBox(  HWND_DESKTOP, hwnd,
                                        "Server not found",
                                        PMCLIENT, 0,
                                        MB_OK | MB_ICONASTERISK) ;
                        break ;
                    }
                    // hide the stats window
                    WinShowWindow( hwndstats, FALSE) ;

                    // set text in text window
                    {
                        CHAR szString[ 80], *p ;

                        WinQueryWindowText( hwndtext, sizeof( szString), szString) ;

                        // truncate after market name
                        p = strchr( szString, ':') ;
                        if( p)
                            *p = '\0' ;

                        sprintf( szString, "%s: %s", szString, pTopics -> szStock) ;
                        WinSetWindowText( hwndtext, szString) ;
                    }
                    // deactivate the STATS menuitem
                    WinSendMsg( MENU( PAPA( hwnd)), MM_SETITEMATTR,
                                MPFROM2SHORT( MN_STATS, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;

                    // deactivate the query menu
                    WinSendMsg( MENU( PAPA( hwnd)), MM_SETITEMATTR,
                                MPFROM2SHORT( MN_REQUESTDATA, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;

                    // reset listbox content
                    WinSendMsg( hwndlist, LM_DELETEALL, 0L, 0L) ;

                    // enable the START LINK button
                    WinEnableWindow( hwndbutton, TRUE) ;

                    // hide the fix window
                    WinShowWindow( hwndfix, FALSE) ;

                    // reset counter & clear graph
                    usdata = 0 ;
                    WinInvalidateRect( hwnd, NULL, TRUE) ;
                }
                    break ;

                case MN_SETLINK:
                {
                    CHAR szTitleID[ 10] ;
                    PDDESTRUCT pddes ;
                    HWND hmenu = MENU( PAPA( hwnd)) ;
                    PTOPICS pTopics ;

                    // get the address to the TOPICS structure
                    pTopics = (PTOPICS)WinQueryWindowPtr( hwndDDE, QWL_USER) ;

                    // allocate room for message WM_DDE_ADVISE
                    sprintf( szTitleID, "%d", pTopics -> sTitleID) ;
                    pddes = WinAllocDdeBlock(   hwndServer, szTitleID,
                                                DDE_FACKREQ, DDEFMT_TEXT,
                                                NULL, 0) ;

                    // post message to server
                    WinDdePostMsg(  hwndServer, hwndDDE,
                                    WM_DDE_ADVISE, MPFROMP( pddes), TRUE) ;

                    // deactivate the Query Nasdaq menuitem
                    WinSendMsg( hmenu, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_QUERYNASDAQ, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;

                    // deactivate the Request data menuitem
                    WinSendMsg( hmenu, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_REQUESTDATA, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;

                    // deactivate the Set Link menuitem
                    WinSendMsg( hmenu, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_STATS, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;

                    // hide the stats window
                    WinShowWindow( hwndstats, FALSE) ;

                    // erase the client window
                    usdata = 0 ;
                    aptl[ 0].x = 100 ;
                    aptl[ 0].y =0 ;
                    WinInvalidateRect( hwnd, NULL, TRUE) ;
                }
                    break ;

                case MN_STOPLINK:
                {
                    PDDESTRUCT pddes ;
                    HWND hmenu = MENU( PAPA( hwnd)) ;

                    WinSetWindowText( hwndbutton, "Start Link") ;
                    WinSetWindowUShort( hwndbutton, QWS_ID, MN_SETLINK) ;

                    // allocate room for the DDESTRUCT & the segment
                    pddes = WinAllocDdeBlock(   hwndServer, "stop",
                                                0, DDEFMT_TEXT,
                                                NULL, 0) ;

                    // post a request for the atom table handle
                    WinDdePostMsg(  hwndServer, PAPA( hwnd),
                                    WM_DDE_UNADVISE, pddes, TRUE) ;

                    // activate the Query Nasdaq menuitem
                    WinSendMsg( hmenu, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_QUERYNASDAQ, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;

                    // activate the Request data menuitem
                    WinSendMsg( hmenu, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_REQUESTDATA, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;

                    // activate the Set Link menuitem
                    WinSendMsg( hmenu, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_STATS, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;
                }
                    break ;
            }
                break ;

        case WM_CLOSE:
            WinDdePostMsg(  hwndServer, hwnd,
                            WM_DDE_TERMINATE, NULL, TRUE) ;
            hwndServer =  NULLHANDLE ;
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            GpiSetCurrentPosition( hps, aptl) ;
            GpiPolyLine( hps, usdata, aptl) ;
            WinEndPaint( hps) ;
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY StockWndProc(  HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static PTOPICS pTopics ;

    switch( msg)
    {
        case WM_INITDLG:
        {
            PSZ p ;
            CHAR szString[ 10] ;
            USHORT i, j = 0 ;

            pTopics = (PTOPICS)mp2 ;
            //WinCenterDlg( pTopics -> hwnd, hwnd) ;

            p = pTopics -> ptr ;

            // loop till the end of the segment
            while( *p != '\r')
            {
                // reset the counter
                i = 0 ;

                // exctract a stock name
                while( *p != ',')
                    *( szString + i++) = *p++ ;

                *( szString + i) = '\0' ;

                i = (short) WinSendMsg( CTRL( hwnd, DL_LIST), LM_INSERTITEM,
                                        MPFROMSHORT( LIT_SORTASCENDING),
                                        MPFROMP( (PSZ)szString)) ;

                // store the sequential number
                WinSendMsg( CTRL( hwnd, DL_LIST), LM_SETITEMHANDLE,
                            MPFROMSHORT( i), MPFROMLONG( (ULONG) ++j)) ;
                p++ ;
            }
        }
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case DL_LIST:
                    switch( SHORT2FROMMP( mp1))
                    {
                        SHORT sPos ;

                        case LN_ENTER:
                            WinSendMsg( hwnd, WM_COMMAND, MPFROMSHORT( DID_OK), 0L) ;
                            break ;

						case LN_SELECT:
							// retrieve currently selected stock
							sPos = (SHORT)WinSendMsg(	(HWND)mp2, LM_QUERYSELECTION,
											MPFROMSHORT( LIT_FIRST), (MPARAM)0L) ;

                            // store the stock ID
                            pTopics -> sTitleID = (SHORT)WinSendMsg(    (HWND)mp2, LM_QUERYITEMHANDLE,
                                                                        MPFROMSHORT( sPos), 0L) ;
                            // store the stock name
                            WinSendMsg( (HWND)mp2, LM_QUERYITEMTEXT,
                                        MPFROM2SHORT( sPos, sizeof( pTopics -> szStock)),
                                        MPFROMP( pTopics -> szStock)) ;

							WinEnableWindow( CTRL( hwnd, DID_OK), TRUE) ;
							break ;

						default:
							break ;
					}
					break ;

				default:
					break ;
			}
			break ;

		case WM_COMMAND:
			switch( COMMANDMSG( &msg) -> cmd)
			{
				case DID_OK:
					WinDismissDlg( hwnd, TRUE) ;
					return (MRESULT)TRUE ;

				case DID_CANCEL:
					WinDismissDlg( hwnd, FALSE) ;
					return (MRESULT)FALSE ;

				default:
					break ;
			}
			break ;

		default:
			break ;
	}
	return WinDefDlgProc( hwnd, msg, mp1, mp2) ;
}

PDDESTRUCT WinAllocDdeBlock(    HWND hwndDest,
                                PSZ pszItemName,
                                USHORT fsStatus,
                                USHORT usFormat,
                                PVOID pabData,
                                USHORT usDataLen)
{
    USHORT usItemLen ;
    PVOID pMem ;

    if( pszItemName != NULL)
        usItemLen = strlen( pszItemName) + 1 ;
    else
        usItemLen = 1 ;

    if( !DosAllocSharedMem( (PPVOID)&pMem, NULL,
                                sizeof( DDESTRUCT) + usItemLen + usDataLen,
                                PAG_READ | PAG_WRITE | PAG_COMMIT | OBJ_GIVEABLE))
    {
        PID pid ;
        TID tid ;
        PDDESTRUCT pddes = (PDDESTRUCT)pMem ;

        pddes -> cbData = usDataLen ;
        pddes -> fsStatus = fsStatus ;
        pddes -> usFormat = usFormat ;
        pddes -> offszItemName = sizeof( DDESTRUCT) ;

        if( usDataLen && pabData)
            pddes->offabData = sizeof( DDESTRUCT) + usItemLen ;
        else
            pddes -> offabData = sizeof( DDESTRUCT) ;

        if( pszItemName != NULL)
            strcpy( DDES_PSZITEMNAME( pddes) , pszItemName) ;

        memcpy( DDES_PABDATA( pddes), pabData, usDataLen) ;

        // determine the client PID
        WinQueryWindowProcess( hwndDest, &pid, &tid) ;
        DosGiveSharedMem( pMem, pid, PAG_READ | PAG_WRITE) ;

        return pddes ;
    }
    return NULL ;
}

