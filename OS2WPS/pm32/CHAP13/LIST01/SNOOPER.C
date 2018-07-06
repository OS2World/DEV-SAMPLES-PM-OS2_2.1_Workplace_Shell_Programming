// SNOOPER.C - SNOOPING OTHER WINDOWS
// Listing 13-01

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "snooper.h"

// definitions
#define ITEMHEIGHT 15L

#define WINDOW_INFO 16      // # of static window in each panel
#define TEXTHEIGHT  16
#define LINESPACE   16
#define WINDOWCX    200
#define WINDOWCY    350
#define WINTREEHEIGHT   220
#define LABEL       WINDOWCX / 10 * 9
#define INFOWINDOW  10000
#define SNOOPNEXT   6           // # of windows in each info panel
#define SNOOPEROBJ  8           // # of windows in SNOOPER
#define DISPLAYWINDOW 5000

#define WM_PASSPROC     WM_USER
#define WM_WHICH        WM_USER + 1

#define PARENTBTN   DISPLAYWINDOW + WINDOW_INFO
#define OWNERBTN    DISPLAYWINDOW + WINDOW_INFO + 1

#define FCF_WPS     FCF_HIDEBUTTON | FCF_STANDARD & ~FCF_MINMAX

#define CT_SNOOP    1000
#define CT_DRAGINFO 1001
#define CT_WINTREE  1002

// macros
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define OWNER( x)   WinQueryWindow( x, QW_OWNER)
#define CTRL( x, y) WinWindowFromID( x, y)
#define HAB( x)     WinQueryAnchorBlock( x)
#define SYS( x)     WinQuerySysValue( HWND_DESKTOP, x)

// function prototypes
int main( void) ;
MRESULT EXPENTRY DropInfoWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY DropPanelWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY TitlebarWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY FrameWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY SnoopWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ButtonWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
BOOL CreateSnoopWindow( HWND hwndPapa, HWND hwndSnoop) ;
PAPPREC CountWindows( HWND hwnd, HWND hwndCnr, PAPPREC papprec) ;
PAPPREC CreateAnotherObj( HWND hwnd, HWND hwndFound, short sObjNum, PAPPREC precParent) ;
BOOL SetConditionalMenu( HWND hmenu, USHORT msg, ULONG ulSetDef) ;
BOOL CheckWindow( HWND hwnd, HWND hwndSnoop) ;
HWND CreateWinTree( HWND hwndClient) ;
BOOL SaveText( HWND hwndClient, PSZ pszFullFile) ;
BOOL GetFullPathName( PSZ pszFullPathName, ULONG ulBuffSize) ;



int main( void)
{
    CHAR szClassName[ 20] ;
    CHAR szSnoopClass[ 20] ;
    CHAR szWindowTitle[ 20] ;
    CHAR szDropInfoClass[ 20] ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG ulFCFlags = FCF_STANDARD & ~FCF_MENU & ~FCF_ICON & ~FCF_ACCELTABLE & ~FCF_SYSMENU ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load class & title string
    WinLoadString( hab, NULLHANDLE, ST_CLASSNAME, sizeof( szClassName), szClassName) ;
    WinLoadString( hab, NULLHANDLE, ST_SNOOPCLASS, sizeof( szSnoopClass), szSnoopClass) ;
    WinLoadString( hab, NULLHANDLE, ST_DROPINFO, sizeof( szDropInfoClass), szDropInfoClass) ;
    WinLoadString( hab, NULLHANDLE, ST_WINDOWTITLE, sizeof( szWindowTitle), szWindowTitle) ;

    // main application class
    if( !WinRegisterClass(  hab, szClassName,
                            ClientWndProc, CS_SIZEREDRAW, sizeof( PVOID) + sizeof( ULONG)))
        return FALSE ;

    // snooping window
    if( !WinRegisterClass(  hab, szSnoopClass,
                            SnoopWndProc, CS_SIZEREDRAW, 0L))
        return FALSE ;

    // dropinfo window
    if( !WinRegisterClass(  hab, szDropInfoClass,
                            DropPanelWndProc, CS_SIZEREDRAW, 0L))
        return FALSE ;

    // main window
    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &ulFCFlags,
                                    szClassName,
                                    szWindowTitle,
                                    WS_CLIPCHILDREN,
                                    NULLHANDLE, 0L,
                                    &hwndClient) ;
 
    if( !hwndFrame)
        return FALSE ;

    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        355, 355, 270 + SYS( SV_CXSIZEBORDER) * 2, 120,
                        SWP_ZORDER | SWP_SHOW | SWP_MOVE | SWP_SIZE | SWP_ACTIVATE) ;

    while( WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
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
    static BOOL fSnoop = FALSE ;
    static HWND hpopup ;
    static RECTL rc ;

    switch( msg)
    {
        case WM_CREATE:
        {
            APIRET apiret ;
            PHWND pMem ;
            HWND hwndCtl, hwndClient ;
            ULONG ulCnt, ulStyle ;
            MENUITEM mi ;
            HWND hmenu ; 
            BTNCDATA btncdata ;
            COLOR clr = CLR_WHITE ;

            // loading the titlebar menu
            hmenu = WinLoadMenu( PAPA( hwnd), NULLHANDLE, RS_NEWSYS) ;

            // modify to become the frame window sysmenu
            ulStyle = WinQueryWindowULong( hmenu, QWL_STYLE) ;
            WinSetWindowULong( hmenu, QWL_STYLE, ulStyle | MS_TITLEBUTTON) ;

            SetConditionalMenu( hmenu, MN_OPEN, 0L) ;
            SetConditionalMenu( hmenu, MN_HELP, 0L) ;
            SetConditionalMenu( hmenu, MN_CREATEANOTHER, 0L) ;

            // now it is a sysmenu
            WinSetWindowUShort( hmenu, QWS_ID, FID_SYSMENU) ;

            // update the frame window
            WinSendMsg( PAPA( hwnd), WM_UPDATEFRAME, MPFROMLONG( FCF_SYSMENU), 0L) ;

            // loading the popup menu from its template
            hpopup = WinLoadMenu( HWND_DESKTOP, NULLHANDLE, RS_NEWSYS) ;

            // assign the FID_SYSMENU ID
            WinSetWindowUShort( hpopup, QWS_ID, FID_SYSMENU) ;
            // query the menu
            WinSendMsg( hpopup, MM_QUERYITEM, MPFROM2SHORT( FID_SYSMENU, TRUE), MPFROMP( &mi)) ;
            // take just the drop-down
            hpopup = mi.hwndSubMenu ;
            // add the mini-pushbuttons
            SetConditionalMenu( hpopup, MN_OPEN, 0L) ;
            SetConditionalMenu( hpopup, MN_HELP, 0L) ;
            SetConditionalMenu( hpopup, MN_CREATEANOTHER, 0L) ;

            // allocate room for the data structures
            apiret = DosAllocMem( (PPVOID)&pMem, sizeof( HWND) * 1024, PAG_COMMIT | PAG_WRITE | PAG_READ) ;
            if( apiret)
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
            }
            // store the pointer
            if( !WinSetWindowPtr( hwnd, 0, pMem))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // fill the first elements
            *pMem++ = PAPA( hwnd) ;                         // application frame window
            *pMem++ = hwnd ;                                // application client window
            *pMem++ = CTRL( PAPA( hwnd), FID_TITLEBAR) ;    // application title bar
            *pMem++ = CTRL( PAPA( hwnd), FID_SYSMENU) ;     // application sysmenu
            *pMem++ = CTRL( PAPA( hwnd), FID_MINMAX) ;      // application minmax
            // update the counter
            ulCnt = 5 ;

            // Snoop button
            hwndCtl = WinCreateWindow(  hwnd, WC_BUTTON,
                                        "Snoop",
                                        WS_VISIBLE | BS_PUSHBUTTON,
                                        0, 5, 90, 65,
                                        hwnd, HWND_TOP,
                                        CT_SNOOP,
                                        NULL, NULL) ;

            *pMem++ = hwndCtl ;                             // application Snoop button
            ulCnt++ ;

            // load the context menu
            hmenu = WinLoadMenu( hwndCtl, NULLHANDLE, RS_SNOOP) ;
            // store it in the window QWL_USER area
            WinSetWindowULong( hwndCtl, QWL_USER, hmenu) ;

            // subclass it
            WinSendMsg( hwndCtl, WM_PASSPROC, MPFROMP( WinSubclassWindow( hwndCtl, ButtonWndProc)), 0L) ;

            // Draginfo button
            btncdata.cb = (USHORT)sizeof( BTNCDATA) ;
            btncdata.fsCheckState = TRUE ;
            btncdata.fsHiliteState = TRUE ;
            btncdata.hImage = (LHANDLE)WinLoadPointer( HWND_DESKTOP, NULLHANDLE, RS_ICON) ;

            hwndCtl = WinCreateWindow(  hwnd, WC_BUTTON,
                                        "Steve",
                                        WS_VISIBLE | BS_PUSHBUTTON | BS_AUTOSIZE | BS_ICON,
                                        110, 5, 0, 0,
                                        hwnd, HWND_TOP,
                                        CT_DRAGINFO,
                                        &btncdata, NULL) ;
            WinSetPresParam( hwndCtl, PP_BACKGROUNDCOLORINDEX, sizeof( clr), &clr) ;

            // subclass it
            WinSendMsg( hwndCtl, WM_PASSPROC, MPFROMP( WinSubclassWindow( hwndCtl, DropInfoWndProc)), 0L) ;

            *pMem++ = hwndCtl ;                             // application DRAGINFO button
            ulCnt++ ;

            // WinTree button
            hwndCtl = WinCreateWindow(  hwnd, WC_BUTTON,
                                        "WinTree",
                                        WS_VISIBLE | BS_PUSHBUTTON,
                                        180, 5, 90, 65,
                                        hwnd, HWND_TOP,
                                        CT_WINTREE,
                                        NULL,NULL) ;

            *pMem++ = hwndCtl ;                             // application WinTree button
            ulCnt++ ;

            // load the context menu
            hmenu = WinLoadMenu( hwndCtl, NULLHANDLE, RS_WINTREE) ;
            SetConditionalMenu( hmenu, MN_DEFTREE, MN_DESKTOP) ;
            // store it in the window QWL_USER area
            WinSetWindowULong( hwndCtl, QWL_USER, hmenu) ;

            // subclass it
            WinSendMsg( hwndCtl, WM_PASSPROC, MPFROMP( WinSubclassWindow( hwndCtl, ButtonWndProc)), 0L) ;

            // store the counter
            WinSetWindowULong( hwnd, sizeof( PHWND), ulCnt) ;

        }
            break ;

        case WM_CLOSE:
        {
            PHWND phwnd ;
            APIRET apiret ;

            // pointer to the HWND memory area
            phwnd = (PHWND)WinQueryWindowPtr( hwnd, 0L) ;
            // destroy the memory block
            if( ( apiret = DosFreeMem( (PVOID)phwnd)))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        }
            break ;

        case WM_BUTTON1DOWN:
        {
            if( WinQueryCapture( HWND_DESKTOP) == hwnd)
                WinSendMsg( hwnd, WM_COMMAND, MPFROMSHORT( CT_SNOOP), 0L) ;
        }
            break ;

        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1))
            {
                case MN_SETTINGS:
                    WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                    break ;

                case MN_DEFAULT:
                {
                    APIRET apiret ;
                    RESULTCODES rescode ;

                    apiret = DosExecPgm(    NULL, 0L,
                                        EXEC_ASYNC,
                                        NULL, NULL,
                                        &rescode,
                                        "snooper.exe") ;

                    if( apiret)
                        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                }
                    break ;

                case CT_SNOOP:
                {
                    if( !fSnoop)
                    {

                        // capturing the mouse
                        WinSetCapture( HWND_DESKTOP, hwnd) ;
                        // set the flag
                        fSnoop = TRUE ;
                    }
                    else
                    {
                        // releasing the mouse
                        WinSetCapture( HWND_DESKTOP, NULLHANDLE) ;
                        // release the flag
                        fSnoop = FALSE ;
                    }
                    // modify the controls status
                    WinSetWindowText( CTRL( hwnd, CT_SNOOP), ( fSnoop) ? "Stop" : "Snoop") ;
                }
                    break ;

                case CT_DRAGINFO:
                {
                }
                    break ;

                case CT_WINTREE:
                {
                    HWND hwndWhich, hwndCnr, hmenu ;
                    MENUITEM mi ;

                    // create a container window in TREE - NAME view
                    hwndCnr = CreateWinTree( hwnd) ;

                    // get the menu handle
                    hmenu = (HWND)WinQueryWindowULong( CTRL( hwnd, CT_WINTREE), QWL_USER) ;
                    WinSendMsg( hmenu, MM_QUERYITEM, MPFROM2SHORT( MN_DEFTREE, TRUE), MPFROMP( &mi)) ;

                    // HWND_DESKTOP or HWND_OBJECT ?
                    hwndWhich = (HWND)( (LONG)WinSendMsg( mi.hwndSubMenu, MM_QUERYDEFAULTITEMID, 0L, 0L) - 502) ;

                    CountWindows( hwndWhich, hwndCnr, NULL) ;
                }
                    break ;

                default:
                    break ;
            }
            break ;

        case WM_MOUSEMOVE:
        {
            POINTL pt ;
            HWND hwndSnoop ;

            // skip if the user hasn't pressed the Snoop button yet
            if( !fSnoop)
                break ;

            pt.x = SHORT1FROMMP( mp1) ;
            pt.y = SHORT2FROMMP( mp1) ;

            // map the points for the DESKTOP
            WinMapWindowPoints( hwnd, HWND_DESKTOP, &pt, 1) ;
            // determine the selected window
            hwndSnoop = WinWindowFromPoint( HWND_DESKTOP, &pt, TRUE) ;

            // check the memory area and create the info panel
            CheckWindow( hwnd, hwndSnoop) ;
        }
            break ;

        case WM_SIZE:
            rc.xLeft = 1L ;
            rc.yBottom = 1 ;
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

       case WM_CONTEXTMENU:
       {
           POINTL pt ;

           // mapping the cursor position
           pt.x = SHORT1FROMMP( mp1) ;
           pt.y = SHORT2FROMMP( mp1) ;
           WinMapWindowPoints( hwnd, HWND_DESKTOP, &pt, 1L) ;

           // showing the window context menu
           WinPopupMenu(   HWND_DESKTOP,
                           PAPA( hwnd),     // frame window
                           hpopup,
                           pt.x, pt.y,
                           0,
                           PU_HCONSTRAIN | PU_VCONSTRAIN | PU_NONE |
                           PU_MOUSEBUTTON2 | PU_MOUSEBUTTON1 | PU_KEYBOARD) ;
       }
           return 0L ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
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
            pfnwp = (PFNWP)mp1 ;
            return (MRESULT)0L ;

        case WM_SYSCOMMAND:
            switch( SHORT1FROMMP( mp1))
            {
                case SC_CLOSE:
                {
                    HWND hwndClient ;

                    // application client window
                    hwndClient = OWNER( hwnd) ;
                    // hide the container window
                    WinShowWindow( hwnd, FALSE) ;
                    // modify the controls status
                    WinEnableWindow( CTRL( hwndClient, CT_WINTREE), TRUE) ;
                }
                    return (MRESULT)FALSE ;
            }
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case FID_CLIENT:
                    switch( SHORT2FROMMP( mp1))
                    {
                        static PAPPREC pappsource ;

                        case CN_INITDRAG:
                        {
                            PCNRDRAGINIT pcnrdraginit = (PCNRDRAGINIT)mp2 ;
                            PDRAGINFO pdrginfo ;
                            DRAGITEM drgitem, *pdrgitem ;
                            DRAGIMAGE drgimage ;
                            HWND hwndTarget ;
                            PAPPREC papprec ;

                            // record info
                            papprec = (PAPPREC)pcnrdraginit -> pRecord ;
                            pappsource = papprec ;

                            // allocate a DRAGINFO structure
                            pdrginfo = DrgAllocDraginfo( 1L) ;

                            pdrginfo -> hwndSource = pcnrdraginit -> hwndCnr ;
                            //pdrginfo -> usOperation = DO_MOVE ;

                            // initialize the DRAGITEM structure
                            drgitem.hwndItem = pcnrdraginit -> hwndCnr ;
                            drgitem.ulItemID = 100L ;
                            drgitem.hstrType = DrgAddStrHandle( DRT_TEXT) ;
                            drgitem.hstrRMF = DrgAddStrHandle( "<DRM_RESERVED,DRF_TEXT>") ;
                            drgitem.hstrContainerName = DrgAddStrHandle( pcnrdraginit -> pRecord -> pszText) ;
                            drgitem.hstrSourceName = DrgAddStrHandle( pcnrdraginit -> pRecord -> pszText) ;
                            drgitem.hstrTargetName = DrgAddStrHandle( pcnrdraginit -> pRecord -> pszText) ;
                            drgitem.cxOffset = 0 ;
                            drgitem.cyOffset = 0 ;
                            drgitem.fsControl = DC_REMOVEABLEMEDIA ;
                            drgitem.fsSupportedOps = DO_COPYABLE | DO_MOVEABLE ;

                            if( !DrgSetDragitem( pdrginfo, &drgitem, sizeof( DRAGITEM), 0L))
                                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

                            // allocate DRAGIMAGE structure
                            drgimage.cb = sizeof( DRAGIMAGE) ;
                            drgimage.cptl = 0 ;
                            drgimage.hImage = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, RS_ICON) ;
                            drgimage.sizlStretch.cx = 0 ;
                            drgimage.sizlStretch.cy = 0 ;
                            drgimage.fl = DRG_ICON ;
                            drgimage.cxOffset = 0 ;
                            drgimage.cyOffset = 0 ;

                            hwndTarget = DrgDrag(   pcnrdraginit -> hwndCnr,
                                                    pdrginfo,
                                                    &drgimage,
                                                    1L,
                                                    VK_ENDDRAG,
                                                    NULL) ;
                        }
                            break ;

                        case CN_DRAGOVER:
                            return MRFROM2SHORT( DOR_NEVERDROP, 0x0000) ;

                        case CN_ENTER:
                        {
                            PCHAR p ;
                            CHAR szWindow[ 20] ;
                            PNOTIFYRECORDENTER pNotRecEnter ;
                            PRECORDCORE prec ;
                            HWND hwndClient, hwndSelected ;

                            pNotRecEnter = (PNOTIFYRECORDENTER)mp2 ;
                            // skip if the user double clicked on an empty spot
                            if( !pNotRecEnter -> pRecord)
                                break ;

                            prec = (PRECORDCORE)pNotRecEnter -> pRecord ;
                            // application client window
                            hwndClient = OWNER( hwnd) ;

                            strcpy( szWindow, prec -> pszText) ;
                            p = strchr( szWindow, ' ') ;
                            *p = '\0' ;
                            // trasform the HEX string into an HWND
                            sscanf( szWindow, "%lx", (PHWND)&hwndSelected) ;
                            // create the info panel
                            CheckWindow( hwndClient, hwndSelected) ;
                        }
                            break ;
                    }
                    break ;
            }
            break ;

        default:
            break ;
    }
    return ( *pfnwp)( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY ButtonWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static PFNWP pfnwp ;
    static BOOL fDraw ;
    static HWND hwndWhich = HWND_DESKTOP ;

    switch( msg)
    {
        case WM_PASSPROC:
            pfnwp = (PFNWP)mp1 ;
            return (MRESULT)0L ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_CLOSEALL:
                {
                    PHWND phwnd ;
                    HWND hwndClient, hmenu ;
                    ULONG ulCnt ;

                    // application client window
                    hwndClient = PAPA( hwnd) ;
                    // HWND memory block
                    phwnd = (PHWND)WinQueryWindowPtr( hwndClient, 0) ;
                    ulCnt = WinQueryWindowULong( hwndClient, sizeof( PHWND)) ;

                    // move to the first snooped window
                    phwnd += SNOOPEROBJ ;

                    // close every info panel
                    while( *phwnd)
                    {
                        WinDestroyWindow( *( phwnd + 1)) ;
                        phwnd += SNOOPNEXT ;
                        ulCnt -= SNOOPNEXT ;
                    }
                    // reset the counter to SNOOPEROBJ, windows in the main application
                    WinSetWindowULong( hwndClient, sizeof( PHWND), SNOOPEROBJ) ;

                    // get the window context menu
                    hmenu = WinQueryWindowULong( hwnd, QWL_USER) ;

                    // disable the MN_CLOSEALL menuitem
                    WinSendMsg( hmenu, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_CLOSEALL, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;
                    // disable the MN_MINIMIZEALL menuitem
                    WinSendMsg( hmenu, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_MINIMIZEALL, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;
                }
                    break ;

                case MN_MINIMIZEALL:
                    break ;

                case MN_DESKTOP:
                case MN_OBJECT:
                {
                    HWND hmenu ;

                    // get the menu handle
                    hmenu = WinQueryWindowULong( hwnd, QWL_USER) ;

                    // set the checkmark
                    WinSendMsg( hmenu, MM_SETITEMATTR,
                                MPFROM2SHORT( COMMANDMSG( &msg) -> cmd, TRUE),
                                MPFROM2SHORT( MIA_CHECKED, MIA_CHECKED)) ;

                    switch( COMMANDMSG( &msg) -> cmd)
                    {
                        MENUITEM mi ;

                        case MN_DESKTOP:
                            // remove the checkmark from the other menuitem
                            WinSendMsg( hmenu, MM_SETITEMATTR,
                                        MPFROM2SHORT( MN_OBJECT, TRUE),
                                        MPFROM2SHORT( MIA_CHECKED, ~MIA_CHECKED)) ;

                            // query the menuitem
                            WinSendMsg( hmenu, MM_QUERYITEM, MPFROM2SHORT( MN_DEFTREE, TRUE), MPFROMP( &mi)) ;
                            // set default item ID MM_SETDEFAULTITEMID
                            WinSendMsg( mi.hwndSubMenu, MM_SETDEFAULTITEMID, MPFROMSHORT( MN_DESKTOP), 0L) ;
                            // set the root window to HWND_DESKTOP
                            hwndWhich = HWND_DESKTOP ;
                            break ;

                        case MN_OBJECT:
                            // remove the checkmark from the other menuitem
                            WinSendMsg( hmenu, MM_SETITEMATTR,
                                        MPFROM2SHORT( MN_DESKTOP, TRUE),
                                        MPFROM2SHORT( MIA_CHECKED, ~MIA_CHECKED)) ;
                            // query the menuitem
                            WinSendMsg( hmenu, MM_QUERYITEM, MPFROM2SHORT( MN_DEFTREE, TRUE), MPFROMP( &mi)) ;
                            // set default item ID MM_SETDEFAULTITEMID
                            WinSendMsg( mi.hwndSubMenu, MM_SETDEFAULTITEMID, MPFROMSHORT( MN_OBJECT), 0L) ;
                            // set the root window to HWND_OBJECT
                            hwndWhich = HWND_OBJECT ;
                            break ;
                    }
                    // simulate the pressure on the WinTree button
                    WinSendMsg( PAPA( hwnd), WM_COMMAND, MPFROMSHORT( CT_WINTREE), 0L) ;
                }
                    break ;
            }
            break ;

        case WM_WHICH:
            return MRFROMLONG( hwndWhich) ;

        case DM_DRAGOVER:
        {
            PDRAGINFO pdrginfo ;
            PDRAGITEM pdrgitem ;
            HPS hps ;
            RECTL rc ;

            // pointer to the DRAGINFO structure
            pdrginfo = (PDRAGINFO)mp1 ;

            if( !DrgAccessDraginfo( pdrginfo))
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                break ;
            }

            WinQueryWindowRect( hwnd, &rc) ;
            WinInflateRect( HAB( hwnd), &rc, -5L, -5L) ;

            if( !fDraw)
            {
                // emphasized the button
                hps = DrgGetPS( hwnd) ;
                GpiSetMix( hps, FM_INVERT) ;
                GpiSetColor( hps, CLR_BLACK) ;
                GpiSetCurrentPosition( hps, (PPOINTL)&rc + 1) ;
                GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rc, 10L, 10L) ;
                DrgReleasePS( hps) ;
                fDraw = TRUE ;
            }

            // access the first DRAGITEM object
            pdrgitem = DrgQueryDragitemPtr( pdrginfo, 0L) ;

            // check if it is coming from a different window
            if( pdrginfo -> hwndSource == hwnd)
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                DrgFreeDraginfo( pdrginfo) ;
                break ;
            }

            // verify the type
            if( !DrgVerifyType( pdrgitem, DRT_TEXT))
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                DrgFreeDraginfo( pdrginfo) ;
                break ;
            }
            // verify the mechanism and the format

            if( !DrgVerifyRMF( pdrgitem, "DRM_RESERVED", "DRF_TEXT"))
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                DrgFreeDraginfo( pdrginfo) ;
                break ;
            }

            // free the info block
            DrgFreeDraginfo( pdrginfo) ;

            // return value
            return MRFROM2SHORT( DOR_DROP, DO_MOVE) ;
        }
            break ;
 
        case DM_DRAGLEAVE:
        {
            HPS hps ;
            RECTL rc ;

            // remove the emphasis
            hps = DrgGetPS( hwnd) ;
            WinQueryWindowRect( hwnd, &rc) ;
            WinInflateRect( HAB( hwnd), &rc, -5L, -5L) ;
            GpiSetMix( hps, FM_INVERT) ;
            GpiSetColor( hps, CLR_PALEGRAY) ;
            GpiSetCurrentPosition( hps, (PPOINTL)&rc + 1) ;
            GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rc, 10L, 10L) ;
            DrgReleasePS( hps) ;

            // release the flag
            fDraw = FALSE ;
        }
            break ;

        case DM_DROP:
        {
            PDRAGINFO pdrginfo = (PDRAGINFO)mp1;
            PDRAGITEM pdrgitem ;
            CHAR szContainer[ 260] ;
            PCHAR p ;
            HWND hwndDrop ;
            HPS hps ;
            RECTL rc ;

            if( !DrgAccessDraginfo( pdrginfo))
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                break ;
            }
            // retrieve the DRAGITEM pointer
            pdrgitem = DrgQueryDragitemPtr( pdrginfo, 0L) ;

            // retrieve the source container name
            DrgQueryStrName(    pdrgitem -> hstrContainerName,
                                sizeof( szContainer), szContainer) ;
            // limit to the handle value
            p = strchr( szContainer, ' ') ;
            *p = '\0' ;
            // convert it into a HWND datum
            sscanf( szContainer, "%lx", (PHWND)&hwndDrop) ;

            switch( WinQueryWindowUShort( hwnd, QWS_ID))
            {
                case CT_SNOOP:
                    // check it and then create an info panel
                    CheckWindow( PAPA( hwnd), hwndDrop) ;
                    break ;

                case CT_WINTREE:
                {
                    HWND hwndCnr ;

                    // create a container window in TREE - NAME view
                    hwndCnr = CreateWinTree( PAPA( hwnd)) ;
                    CountWindows( hwndDrop, hwndCnr, NULL) ;
                }
                    break ;

                case CT_DRAGINFO:
                {
                }
                    break ;
            }



            // remove the emphasis
            hps = DrgGetPS( hwnd) ;
            WinQueryWindowRect( hwnd, &rc) ;
            WinInflateRect( HAB( hwnd), &rc, -5L, -5L) ;
            GpiSetMix( hps, FM_INVERT) ;
            GpiSetColor( hps, CLR_PALEGRAY) ;
            GpiSetCurrentPosition( hps, (PPOINTL)&rc + 1) ;
            GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rc, 10L, 10L) ;
            DrgReleasePS( hps) ;

            // release the flag
            fDraw = FALSE ;

            // free the info block
            DrgFreeDraginfo( pdrginfo) ;
        }
            break ;

        case WM_CONTEXTMENU:
        {
           POINTL pt ;

           // mapping the cursor position
           pt.x = SHORT1FROMMP( mp1) ;
           pt.y = SHORT2FROMMP( mp1) ;
           WinMapWindowPoints( hwnd, HWND_DESKTOP, &pt, 1L) ;

           // showing the window context menu
           WinPopupMenu(   HWND_DESKTOP,
                           hwnd,     // frame window
                           WinQueryWindowULong( hwnd, QWL_USER),
                           pt.x, pt.y,
                           0,
                           PU_HCONSTRAIN | PU_VCONSTRAIN | PU_NONE |
                           PU_MOUSEBUTTON2 | PU_MOUSEBUTTON1 | PU_KEYBOARD) ;
        }
            break ;

        default:
            break ;
    }
    return ( *pfnwp)( hwnd, msg, mp1, mp2) ;
}


MRESULT EXPENTRY TitlebarWndProc(   HWND hwnd,
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

        case WM_BUTTON2DOWN:
            if( WinQueryActiveWindow( HWND_DESKTOP) != PAPA( hwnd))
                WinSetActiveWindow( HWND_DESKTOP, PAPA( hwnd)) ;
            return (MPARAM)0L ;

        case WM_BUTTON2MOTIONSTART:
            break ;

        case WM_BEGINDRAG:
        {
            PDRAGINFO pdrginfo ;
            DRAGITEM drgitem, *pdrgitem ;
            DRAGIMAGE drgimage ;
            PSZ pszFullFile = "DRAG.DOC" ;
            HWND hwndTarget, hwndClient, hwndFrame, hwndPanel ;
            CHAR szWindowTitle[ 20], szFullPathName[ 260] ;

            // panel info frame window
            hwndFrame = PAPA( hwnd) ;

            // panel info client window
            hwndPanel = CTRL( hwndFrame, FID_CLIENT) ;

            // application client window
            hwndClient = OWNER( hwndFrame) ;

            // query window title
            WinQueryWindowText( hwndFrame, sizeof( szWindowTitle), szWindowTitle) ;

            // get the full path name
            if( !GetFullPathName( szFullPathName, sizeof( szFullPathName)))
                return FALSE ;
 
            // save text
            SaveText( hwndPanel, szWindowTitle) ;

            // allocate a DRAGINFO structure
            pdrginfo = DrgAllocDraginfo( 1L) ;
            pdrginfo -> hwndSource = hwndClient ;

            // initialize the DRAGITEM structure
            drgitem.hwndItem = hwndClient ;
            drgitem.ulItemID = 100L ;
            drgitem.hstrType = DrgAddStrHandle( DRT_TEXT) ;
            drgitem.hstrRMF = DrgAddStrHandle( "<DRM_OS2FILE,DRF_TEXT>") ;
            drgitem.hstrContainerName = DrgAddStrHandle( szFullPathName) ;
            drgitem.hstrSourceName = DrgAddStrHandle( szWindowTitle) ;
            drgitem.hstrTargetName = DrgAddStrHandle( szWindowTitle) ;
            drgitem.cxOffset = 0 ;
            drgitem.cyOffset = 0 ;
            drgitem.fsControl = DC_REMOVEABLEMEDIA ;
            drgitem.fsSupportedOps = DO_MOVEABLE | DO_COPYABLE ;

            pdrgitem = (PDRAGITEM)( pdrginfo + 1) ;

            if( !DrgSetDragitem( pdrginfo, &drgitem, sizeof( DRAGITEM), 0L))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // allocate DRAGIMAGE structure
            drgimage.cb = sizeof( DRAGIMAGE) ;
            drgimage.cptl = 0 ;
            drgimage.hImage = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, RS_ICON) ;
            drgimage.sizlStretch.cx = 0L ;
            drgimage.sizlStretch.cy = 0L ;
            drgimage.fl = DRG_ICON ;
            drgimage.cxOffset = 0 ;
            drgimage.cyOffset = 0 ;

            hwndTarget = DrgDrag(   hwndClient,
                                    pdrginfo,
                                    &drgimage,
                                    1L,
                                    VK_ENDDRAG,
                                    NULL) ;
        }
            break ;

        case WM_BUTTON2MOTIONEND:
            break ;

        case WM_ENDDRAG:
            break ;

        default:
            break ;
    }
    return (* pfnwp)( hwnd, msg, mp1, mp2) ;
}

BOOL SaveText( HWND hwndClient, PSZ pszFullFile)
{
    ULONG ulBufferSize = 0, ulAction, ulBytesWritten ;
    PCHAR pBuffer ;
    LONG l, i ;
    HFILE hfile ;
    APIRET apiret ;
    CHAR szWindowTitle[ 90] ;

    // save it on a file
    if( apiret = DosOpen(   pszFullFile, &hfile,
                        &ulAction,
                        ulBufferSize,
                        FILE_NORMAL,
                        OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                        OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE| OPEN_FLAGS_SEQUENTIAL,
                        NULL))
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        DosFreeMem( pBuffer) ;
        return FALSE ;
    }

    for( i = 0; i < WINDOW_INFO + 2; i++)
    {
        // get the text in each WC_STATIC window
        l = WinQueryWindowText( CTRL( hwndClient, i + DISPLAYWINDOW), sizeof( szWindowTitle), szWindowTitle) ;
        // place the \n CHAR at the end of each string
        strcpy( szWindowTitle + l, "\n") ;

        // writing the file
        if( apiret = DosWrite(  hfile, (PVOID)szWindowTitle, l + 1, &ulBytesWritten))
        {
            WinAlarm( HWND_DESKTOP, WA_ERROR) ;
            DosClose( hfile) ;
            return FALSE ;
        }
    }

    // close file
    DosClose( hfile) ;

    return TRUE ;
}

BOOL GetFullPathName( PSZ pszFullPathName, ULONG ulBuffSize)
{
    APIRET rc ;
    ULONG ulDisk, ulDriveMap ;

    rc = DosQueryCurrentDisk( &ulDisk, &ulDriveMap) ;
    if( rc)
        return FALSE ;
    // store the disk name
    sprintf( pszFullPathName, "%c:\\", (CHAR)( ulDisk + 64)) ;

    // store the path name
    rc = DosQueryCurrentDir( 0l, pszFullPathName + 3, &ulBuffSize) ;
    if( rc)
        return FALSE ;

    return TRUE ;
}


MRESULT EXPENTRY SnoopWndProc(  HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static RECTL rc ;
    static SNOOP snoop ;

    switch( msg)
    {
        case WM_CREATE:
        {
            LONG cy, i ;
            HWND hwndCtl ;
            CHAR szFont[] = "8.Helv" ;
            PPRESPARAMS ppresp ;
            PPARAM pparam ;
            PCHAR pszTitle = (PCHAR)mp1, p ;
            LONG lLen, lDelta ;
            APIRET rc ;

            // allocation of a chunck of memory for the presparams
            rc = DosAllocMem( (PPVOID)&ppresp, 4096, PAG_COMMIT | PAG_WRITE) ;
            if( rc)
                break ;

            // presentation params for the Parent & Owner pushbuttons
            pparam = (PPARAM)(PVOID) ( ppresp -> aparam) ;
            p = (PCHAR)pparam ;

            // set the values
            ppresp -> aparam -> id = PP_FONTNAMESIZE ;
            strcpy( pparam -> ab, szFont) ;
            pparam -> cb = sizeof( szFont) ;
            lDelta = sizeof( szFont) + sizeof( ULONG) * 2 ;
            ppresp -> cb += lDelta ;

            // move the pointer at the end of the first presparam
            p += lDelta ;
            pparam = (PPARAM)p ;

            // set the values
            pparam -> id = PP_FOREGROUNDCOLORINDEX ;
            pparam -> cb = sizeof( COLOR) ;
            *((COLOR *)pparam -> ab) = CLR_WHITE ;
            lDelta = sizeof( COLOR) + sizeof( ULONG) * 2 ;
            ppresp -> cb += lDelta ;

            // move the pointer at the end of the second presparam
            p += lDelta ;
            pparam = (PPARAM)p ;

            // set the values
            pparam -> id = PP_BACKGROUNDCOLORINDEX ;
            pparam -> cb = sizeof( COLOR) ;
            *((COLOR *)pparam -> ab) = CLR_RED ;
            lDelta = sizeof( COLOR) + sizeof( ULONG) * 2 ;
            ppresp -> cb += lDelta ;

            // determine the total window height
            cy = ((PCREATESTRUCT)mp2) -> cy - 30 ;

            // creation of display WC_STATIC windows
            for( i = 0; i < WINDOW_INFO; i++)
            {
                hwndCtl = WinCreateWindow(  hwnd, WC_STATIC,
                                            pszTitle,
                                            WS_VISIBLE | SS_TEXT | DT_LEFT,
                                            2, cy - 5 - i * LINESPACE,
                                            LABEL, TEXTHEIGHT,
                                            hwnd, HWND_TOP,
                                            DISPLAYWINDOW + i,
                                            NULL, NULL) ;

                WinSetPresParam( hwndCtl, PP_FONTNAMESIZE, sizeof( szFont), szFont) ;
                lLen = strlen( pszTitle) + 1 ;
                pszTitle += lLen ;
            }
                // parent button
                WinCreateWindow(    hwnd, WC_BUTTON,
                                    pszTitle,
                                    WS_VISIBLE | BS_PUSHBUTTON,
                                    2, cy - 5 - i * LINESPACE,
                                    LABEL, TEXTHEIGHT + 4,
                                    hwnd, HWND_TOP,
                                    DISPLAYWINDOW + i,
                                    NULL, ppresp) ;

                lLen = strlen( pszTitle) + 1 ;
                pszTitle += lLen ;
                i++ ;
                // owner button
                WinCreateWindow(    hwnd, WC_BUTTON,
                                    pszTitle,
                                    WS_VISIBLE | BS_PUSHBUTTON,
                                    2, cy - 5 - i * LINESPACE,
                                    LABEL, TEXTHEIGHT + 4 ,
                                    hwnd, HWND_TOP,
                                    DISPLAYWINDOW + i,
                                    NULL, (PVOID)ppresp) ;

                // free the chunck of memory containing the pres params
                DosFreeMem( ppresp) ;
            }
                break ;

        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1))
            {
                // Parent button
                case PARENTBTN:
                {
                    HWND hwndParent, hwndClient, hwndButton ;

                    // this button hwnd
                    hwndButton = WinWindowFromID( hwnd, PARENTBTN) ;
                    // parent window to snoop
                    hwndParent = (HWND)WinQueryWindowULong( hwndButton, QWL_USER) ;
                    // application client window, that is the owner of the panel info window
                    hwndClient = OWNER( PAPA( hwnd)) ;
                    // create a new info panel
                    CheckWindow( hwndClient, hwndParent) ;
                }
                    break ;

                // Owner button
                case OWNERBTN:
                {
                    HWND hwndOwner, hwndClient, hwndButton ;

                    // this button hwnd
                    hwndButton = WinWindowFromID( hwnd, OWNERBTN) ;
                    // owner window to snoop
                    hwndOwner = (HWND)WinQueryWindowULong( hwndButton, QWL_USER) ;
                    // application client window, that is the owner of the panel info window
                    hwndClient = OWNER( PAPA( hwnd)) ;
                    // create a new info panel
                    CheckWindow( hwndClient, hwndOwner) ;
                }
                    break ;
            }
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CLOSE:
        {
            HWND hwndClient ;
            USHORT usID ;
            PHWND phwnd ;

            // retrieve the owner, tha application client window
            hwndClient = OWNER( PAPA( hwnd)) ;
            // retrieve the pointer to the hwnd list
            phwnd = WinQueryWindowPtr( hwndClient, 0) ;
            // retrieve the frame window ID
            usID = WinQueryWindowUShort( PAPA( hwnd), QWS_ID) ;
            // erase the snooped handle
            phwnd += ( usID - INFOWINDOW) ;
            *phwnd = NULLHANDLE ;

            // destroy the info panel
            WinDestroyWindow( *( phwnd + 1)) ;
        }
            return (MRESULT)TRUE ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

HWND CreateWinTree( HWND hwndClient)
{
    CNRINFO cnrinfo ;
    HWND hwndTree, hwndCnr ;
    static CHAR szWindowTitle[] = "Window Tree" ;
    static ULONG ulFCFlags = (FCF_NOMOVEWITHOWNER | FCF_STANDARD) & ~FCF_MENU & ~FCF_ACCELTABLE ;

    // create the view window
    hwndTree = WinCreateStdWindow(  HWND_DESKTOP,
                                    0L,
                                    &ulFCFlags,
                                    WC_CONTAINER,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE, RS_ICON,
                                    &hwndCnr) ;
    // set the client application window as the frame owner
    WinSetOwner( hwndCnr, hwndTree) ;
    // set the frame owner
    WinSetOwner( hwndTree, hwndClient) ;

    // subclass the frame window
    WinSendMsg( hwndTree, WM_PASSPROC, MPFROMP( WinSubclassWindow( hwndTree, FrameWndProc)), 0L) ;

    // filling the CNRINFO structure
    cnrinfo.cb = sizeof( cnrinfo) ;
    cnrinfo.pszCnrTitle = "Window tree" ;
    cnrinfo.flWindowAttr = CV_TREE | CV_TEXT | CA_TITLESEPARATOR | CA_CONTAINERTITLE | CA_TREELINE ;

    // passing the data to the container window
    if( !WinSendMsg(    hwndCnr, CM_SETCNRINFO,
                        (MPARAM)&cnrinfo,
                        MPFROMLONG( CMA_CNRTITLE | CMA_FLWINDOWATTR)))
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;

    return hwndCnr ;
}

PAPPREC CountWindows( HWND hwnd, HWND hwndCnr, PAPPREC papprec)
{
    static LONG lCnt = 0 ; 
    HWND hwndt ;
    HENUM henum ;

    henum = WinBeginEnumWindows( hwnd) ;

    papprec = CreateAnotherObj( hwndCnr, hwnd, 1, papprec) ;

    while( ( hwndt = WinGetNextWindow( henum)))
    {
        CreateAnotherObj( hwndCnr, hwndt, 1, papprec) ;
    }
    WinEndEnumWindows( henum) ;

    // position the window
    WinSetWindowPos(    PAPA( hwndCnr), HWND_TOP,
                        lCnt++ * WINDOWCX / 5, SYS( SV_CYSCREEN) - WINTREEHEIGHT,
                        WINDOWCX * 2, WINTREEHEIGHT,
                        SWP_ZORDER | SWP_SHOW | SWP_MOVE | SWP_SIZE | SWP_ACTIVATE) ;

    return papprec ;
}


PAPPREC CreateAnotherObj( HWND hwnd, HWND hwndFound, short sObjNum, PAPPREC precParent)
{
    PAPPREC papprec ;
    RECORDINSERT recins ;
    PRECORDCORE prec, precstart ;
    LONG j = 0, inc = 0 ;
    CHAR szClassName[ 30] ;

    // allocate a RECORDCORE structure
    papprec = (PAPPREC)WinSendMsg(  hwnd, CM_ALLOCRECORD,
                                    MPFROMLONG( sizeof( APPREC) - sizeof( RECORDCORE)),
                                    MPFROMSHORT( sObjNum)) ;

    prec = precstart = (PRECORDCORE)papprec ;

    WinQueryClassName( hwndFound, sizeof( szClassName), szClassName) ;
    sprintf( papprec -> szString, "%lx - %s", hwndFound, szClassName) ;

    // filling the RECORDINSERT structure
    recins.cb = sizeof( RECORDINSERT) ;
    recins.pRecordParent = (PRECORDCORE)precParent ;

    // root objs don't have a parent
    if( precParent)
    {
        recins.pRecordOrder = (PRECORDCORE)CMA_END ;
        recins.zOrder = CMA_BOTTOM ;
    }
    else
    {
        recins.pRecordOrder = (PRECORDCORE)CMA_FIRST ;
        recins.zOrder = CMA_TOP ;
    }
    recins.fInvalidateRecord = TRUE ;
    recins.cRecordsInsert = sObjNum ;

    inc = ( precParent) ? 1 : 0 ;

    papprec -> lType = j + inc ;

    // filling the RECORDCORE structure
    prec -> flRecordAttr = CRA_CURSORED | CRA_DROPONABLE ;
    prec -> ptlIcon.x = 0L ;
    prec -> ptlIcon.y = 0L ;
    prec -> pszIcon = papprec -> szString ;
    prec -> hptrIcon = NULLHANDLE ;
    prec -> hptrMiniIcon = NULLHANDLE ;
    prec -> hbmBitmap = NULLHANDLE ;
    prec -> hbmMiniBitmap = NULLHANDLE ;
    prec -> pTreeItemDesc = NULL ;
    prec -> pszText = papprec -> szString ;
    prec -> pszName = papprec -> szString ;
    prec -> pszTree = papprec -> szString ;

    // store the parent pointer
    papprec -> precParent = (PRECORDCORE)precParent ;

    // increase the counter
    j++ ;

    // pass all the data to the container window
    if( !WinSendMsg(    hwnd, CM_INSERTRECORD,
                        (MPARAM)precstart, (MPARAM)&recins))
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;

    return papprec ;
}

BOOL SetConditionalMenu( HWND hmenu, USHORT msg, ULONG ulSetDef)
{
    MENUITEM mi ;

    // query the menuitem to get the attributes
    WinSendMsg( hmenu, MM_QUERYITEM, MPFROM2SHORT( msg, TRUE), MPFROMP( &mi)) ;

    // check if it is a submenu
    if( !mi.hwndSubMenu)
        return FALSE ;

    // modify to become a conditional cascade menu
    WinSetWindowBits( mi.hwndSubMenu, QWL_STYLE, MS_CONDITIONALCASCADE, MS_CONDITIONALCASCADE) ;

    if( ulSetDef)
    {
        WinSendMsg( mi.hwndSubMenu, MM_SETDEFAULTITEMID, MPFROMLONG( ulSetDef), 0L) ;
    }

    // everything is fine
    return TRUE ;
}

BOOL CheckWindow(   HWND hwndClient,    // application client window
                    HWND hwndSnoop)     // window chosen by the user
{
    LONG i ;
    USHORT usID ;
    PHWND phwnd ;
    ULONG ulCnt ;
    HWND hmenu ;

    // query the window ID
    usID = WinQueryWindowUShort( hwndSnoop, QWS_ID) ;
    // skip if it is a window inside an info panel
    if( ( usID >= DISPLAYWINDOW) && ( usID <= DISPLAYWINDOW + WINDOW_INFO + 2))
        return FALSE ;

    // retrieve the pointer to the hwnd list
    phwnd = (PHWND)WinQueryWindowPtr( hwndClient, 0) ;
    // get the number of window in the block memory area
    ulCnt = WinQueryWindowULong( hwndClient, sizeof( PHWND)) ;

    // skip if we are not on a different window or on the desktop
    for( i = 0; i < ulCnt; )
    {
        if( !*phwnd)
        {
            phwnd += SNOOPNEXT ;
            i += SNOOPNEXT ;
            continue ;
        }

        if( hwndSnoop == ((HWND)*phwnd++))
            return FALSE ;

        i++ ;
    }

    // modify the popup menuitems
    hmenu = WinQueryWindowULong( CTRL( hwndClient, CT_SNOOP), QWL_USER) ;
    WinSendMsg( hmenu, MM_SETITEMATTR,
                MPFROM2SHORT( MN_CLOSEALL, TRUE),
                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;
    WinSendMsg( hmenu, MM_SETITEMATTR,
                MPFROM2SHORT( MN_MINIMIZEALL, TRUE),
                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;

    // hwndClient   -> main window client window
    // hwndSnoop    -> window spotted by the user
    return CreateSnoopWindow( hwndClient, hwndSnoop) ;
}

BOOL CreateSnoopWindow( HWND hwndClient, HWND hwndSnoop)
{
    FRAMECDATA fcd ;
    SNOOP snoop ;
    CHAR szTmp[ 40], szString[ 70], szSnoopClass[ 20], szFont[] = "8.Helv" ;
    HWND hwndFrame, hwndButton, hwndPanel, hwndTitlebar ;
    LONG lLen ;
    PHWND pMem ;
    ULONG ulCnt, ulPos ;
    APIRET rc ;
    PCHAR pchString, pStart ;
    PTIB ptib ;
    PPIB ppib ;

    // load document class name
    WinLoadString( HAB( hwndClient), NULLHANDLE, ST_SNOOPCLASS, sizeof( szSnoopClass), szSnoopClass) ;

    // allocate room for the info to output
    rc = DosAllocMem( (PPVOID)&pStart, 4096, PAG_COMMIT | PAG_WRITE) ;
    if( rc)
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        WinPostMsg( hwndClient, WM_QUIT, 0L, 0L) ;
    }
    pchString = pStart ;

    // prepare info to display
    snoop.hwnd = hwndSnoop ;
    *snoop.szClass = '\0' ;
    // retrieve class name
    lLen = WinQueryClassName( hwndSnoop, sizeof( snoop.szClass), snoop.szClass) ;
    // retrieve class info
    WinQueryClassInfo( HAB( hwndSnoop), snoop.szClass, &snoop.clsi) ;
    // window procedure address (bug in the API)
    if( !( snoop.pfnWndProc = (PFNWP)WinQueryWindowPtr( snoop.hwnd, QWP_PFNWP)))
        snoop.pfnWndProc = snoop.clsi.pfnWindowProc ;

    if( lLen < 4 && *snoop.szClass == '#')
    {
        int iClass ;

        if( ( iClass = atoi( snoop.szClass + 1)) > 0x28)
            iClass = 100 ;
        WinLoadString(  HAB( hwndClient), NULLHANDLE, iClass,
                        sizeof szTmp, szTmp) ;
        strcat( snoop.szClass, szTmp) ;
    }

    snoop.hab = HAB( hwndSnoop) ;
    snoop.hmq = WinQueryWindowULong( hwndSnoop, QWL_HMQ) ;
    snoop.hwndParent = WinQueryWindow( hwndSnoop, QW_PARENT) ;

    *snoop.szParentClass = '\0' ;
    lLen = WinQueryClassName( snoop.hwndParent, sizeof snoop.szParentClass, snoop.szParentClass) ;
    if( lLen < 4 && *snoop.szParentClass == '#')
    {
        int iClass ;

        if( (iClass = atoi( snoop.szParentClass + 1)) > 0x28)
            iClass = 100 ;
        WinLoadString(  HAB( hwndClient), NULLHANDLE, iClass,
                        sizeof szTmp, szTmp) ;
        strcat( snoop.szParentClass, szTmp) ;
    }

    snoop.hwndOwner = WinQueryWindow( hwndSnoop, QW_OWNER) ;

    *snoop.szOwnerClass = '\0' ;
    lLen = WinQueryClassName( snoop.hwndOwner, sizeof snoop.szOwnerClass, snoop.szOwnerClass) ;
    if( lLen < 4 && *snoop.szOwnerClass == '#')
    {
        WinLoadString(  HAB( hwndClient), NULLHANDLE,
                        atoi( snoop.szOwnerClass + 1),
                        sizeof szTmp, szTmp) ;
        strcat( snoop.szOwnerClass, szTmp) ;
    }

    snoop.usID = WinQueryWindowUShort( hwndSnoop, QWS_ID) ;
    snoop.ulStyles = WinQueryWindowULong( hwndSnoop, QWL_STYLE) ;

    WinQueryWindowProcess( hwndSnoop, &snoop.pid, &snoop.tid) ;

    WinQueryWindowText( snoop.hwnd, sizeof( snoop.szText), snoop.szText) ;
    // window position & origin
    WinQueryWindowPos( hwndSnoop, &snoop.swp) ;
    snoop.ptl.x = snoop.swp.x ;
    snoop.ptl.y = snoop.swp.y ;
    WinMapWindowPoints( hwndSnoop, HWND_DESKTOP, &snoop.ptl, 1L) ;
    // query the window model
    strcpy( snoop.szModel, (WinQueryWindowModel( hwndSnoop)) ? "32bit" : "16bit") ;

    // object handle
    DosGetInfoBlocks( &ptib, &ppib) ;
    strcpy( snoop.szhobj, ppib -> pib_pchenv) ;

    // display info
    lLen = (LONG) sprintf( pchString, "hwnd: 0x%lx", snoop.hwnd) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "ID: 0x%x", snoop.usID) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "Styles: 0x%lx", snoop.ulStyles) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "Window wndproc: %p", snoop.pfnWndProc) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "Window words: 0x%lx", snoop.clsi.cbWindowData) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "Caption: %s", snoop.szText) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "Size: %ld x %ld", snoop.swp.cx, snoop.swp.cy) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "Position: (%ld,%ld)-(%ld,%ld)", snoop.swp.x, snoop.swp.y,
                                                                       snoop.swp.x + snoop.swp.cx - 1,
                                                                       snoop.swp.y + snoop.swp.cy - 1) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "Class: %s", snoop.szClass) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "ClassStyle: 0x%lx", snoop.clsi.flClassStyle) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "Class wndproc: %p", snoop.clsi.pfnWindowProc) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "hab: 0x%lx", snoop.hab) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "hmq: 0x%lx", snoop.hmq) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "PID:%ld - TID:%ld", snoop.pid, snoop.tid) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "Window model: %s", snoop.szModel) ;
    pchString += lLen + 1 ;

    // hobject
    lLen = (LONG) sprintf( pchString, "%s", snoop.szhobj) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "Parent: 0x%lx", snoop.hwndParent) ;
    pchString += lLen + 1 ;
    lLen = (LONG) sprintf( pchString, "Owner: 0x%lx", snoop.hwndOwner) ;
    pchString += lLen + 1 ;


    // frame window info
    fcd.cb = (SHORT)sizeof fcd ;
    fcd.flCreateFlags = ( FCF_NOMOVEWITHOWNER | FCF_STANDARD) & ~FCF_MENU & ~FCF_ACCELTABLE ;
    fcd.hmodResources = NULLHANDLE ;
    fcd.idResources = RS_ICON ;

    // retrieve the info
    pMem = (PHWND)WinQueryWindowPtr( hwndClient, 0) ;
    ulCnt = WinQueryWindowULong( hwndClient, sizeof( PHWND)) ;

    // find the first free position if any
    pMem += SNOOPEROBJ ;
    ulPos = SNOOPEROBJ ;

    while( *pMem && ulPos < ulCnt)
    {
        pMem += SNOOPNEXT ;
        ulPos += SNOOPNEXT ;
    }

    // create the window to document the info
    sprintf( szString, "0x%lx", snoop.hwnd) ;
    hwndFrame = WinCreateWindow(    HWND_DESKTOP, WC_FRAME,
                                    szString,
                                    WS_CLIPCHILDREN,
                                    0L, 0L, 0L, 0L,
                                    hwndClient, HWND_TOP, INFOWINDOW + ulPos,
                                    (PVOID)&fcd, NULL) ;

    // 10.Helv in the titlebar
    WinSetPresParam( CTRL( hwndFrame, FID_TITLEBAR), PP_FONTNAMESIZE, sizeof( szFont), szFont) ;

    // create the client window. OWNER is the application client window
    hwndPanel = WinCreateWindow(    hwndFrame, szSnoopClass,
                                    NULL,
                                    0L,
                                    0, 0, 0, WINDOWCY - 30,
                                    hwndFrame, HWND_TOP, FID_CLIENT,
                                    pStart, NULL) ;

    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        ( ulCnt - SNOOPEROBJ) / SNOOPNEXT * WINDOWCX / 5, 0, WINDOWCX, WINDOWCY,
                        SWP_ZORDER | SWP_SHOW | SWP_MOVE | SWP_SIZE | SWP_ACTIVATE) ;

    // add the new hwnd and update the pointer
    *pMem++ = hwndSnoop ;
    *pMem++ = hwndFrame ;
    *pMem++ = hwndPanel ;
    *pMem++ = CTRL( hwndFrame, FID_TITLEBAR) ;    // application title bar
    *pMem++ = CTRL( hwndFrame, FID_SYSMENU) ;     // application sysmenu
    *pMem++ = CTRL( hwndFrame, FID_MINMAX) ;      // application minmax

    // update the window counter
    if( ulPos >= ulCnt)
        ulCnt += SNOOPNEXT ;

    // store the counter
    WinSetWindowULong( hwndClient, sizeof( PHWND), ulCnt) ;

    // store the parent handle in the Parent button reserved area
    hwndButton = WinWindowFromID( hwndPanel, PARENTBTN) ;
    if( snoop.hwndParent)
        WinSetWindowULong( hwndButton, QWL_USER, (ULONG)snoop.hwndParent) ;
    else
        WinEnableWindow( hwndButton, FALSE) ;
    // store the owner handle in the Owner button reserved area
    hwndButton = WinWindowFromID( hwndPanel, OWNERBTN) ;
    if( snoop.hwndOwner)
        WinSetWindowULong( hwndButton, QWL_USER, (ULONG)snoop.hwndOwner) ;
    else
        WinEnableWindow( hwndButton, FALSE) ;

    // free chunck of memory with snooped window info
    DosFreeMem( pStart) ;

    // titlebar subclassing
    hwndTitlebar = CTRL( hwndFrame, FID_SYSMENU) ;
    WinSendMsg( hwndTitlebar, WM_PASSPROC, MPFROMP( WinSubclassWindow( hwndTitlebar, TitlebarWndProc)), 0L) ;


    return TRUE ;
}


MRESULT EXPENTRY DropInfoWndProc(   HWND hwnd,
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

            // free the info block
            DrgFreeDraginfo( pdrginfo) ;

            // return value
            return MRFROM2SHORT( usDOR, usDO) ;
        }
            break ;

        case DM_DRAGLEAVE:
        {
        }
            return (MRESULT)NULL ;

        case DM_DROP:
        {
            PDRAGINFO pdrginfo = (PDRAGINFO)mp1;
            PDRAGITEM pdrgitem ;
            ULONG ulTimes = 0, ulItem ;
            LONG i = 0 ;
            HPS hps ;
            CHAR szContainer[ 260], szFileName[ 80], szSourceFile[ 80] ;
            CHAR szType[ 100], szRMF[ 100] ;
            APIRET rc ;

            if( !DrgAccessDraginfo( pdrginfo))
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                break ;
            }

            // #of DRAGITEM structures
            ulItem = DrgQueryDragitemCount( pdrginfo) ;

            while( ulTimes < ulItem)
            {
                pdrgitem = DrgQueryDragitemPtr( pdrginfo, ulTimes++) ;

                // retrieve the type
                DrgQueryStrName( pdrgitem -> hstrType, sizeof( szType), szType) ;

                // retrieve the source container name
                DrgQueryStrName( pdrgitem -> hstrContainerName, sizeof( szContainer), szContainer) ;

                // retrieve the RMF
                DrgQueryStrName( pdrgitem -> hstrRMF, sizeof( szRMF), szRMF) ;

                // retrieve the source file name
                DrgQueryStrName( pdrgitem -> hstrSourceName, sizeof( szSourceFile), szSourceFile) ;

                // retrieve the file name
                DrgQueryStrName( pdrgitem -> hstrTargetName, sizeof( szFileName), szFileName) ;

                // delete the string handles
                DrgDeleteDraginfoStrHandles( pdrginfo) ;

                // create a drop panel window
// FIX

            }

            // free the info block
            DrgFreeDraginfo( pdrginfo) ;
        }
            break ;

        default:
            break ;
    }
    return ( *pfnwp)( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY DropPanelWndProc(  HWND hwnd,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
    switch( msg)
    {
        case WM_CREATE:
            break ;

    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

