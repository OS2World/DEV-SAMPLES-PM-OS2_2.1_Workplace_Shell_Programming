// PMEDIT.C - PM EDITOR
// Listing 13-04
 
// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "pmedit.h"
#include "mie02.h"

// definitions and macros
#define WM_PASSPROC     WM_USER + 0
#define WM_RADIOBUTTON  WM_USER + 1

#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

#define CX_MAJOR_TAB     80
#define CY_MAJOR_TAB     25
#define ROUNDX          20L
#define ROUNDY          20L



#define CTRL( x, y)     WinWindowFromID( x, y)

 
// function prototypes
MRESULT EXPENTRY FrameWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY EditWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY TitlebarWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ValuesWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY SettingsWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY NotebookWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY NewMleWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY Goto( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY Search( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY TitleMenuWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
BOOL SetConditionalMenu( HWND hmenu, USHORT msg) ;
ULONG InsertPage( HWND hwndNotebook, HWND hwndPage, PCHAR pszStatusLine, PCHAR pszTabText) ;
HWND TrapObjectWindow( PCHAR pszText) ;
ULONG GetFullPathName( PSZ pszFullPathName, ULONG ulBuffSize) ;
BOOL SaveText( HWND hwndMLE, PSZ pszFullFile) ;
BOOL OpenFile( HWND hwndMLE, PSZ pszFileName) ;
int main( int argc, CHAR **argv) ;

int main( int argc, CHAR **argv)
{
    HWND hwndFrame, hwndMLE ;
    HMQ hmq ;
    HAB hab ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_TASKLIST & ~FCF_MENU & ~FCF_SYSMENU ;
    PID pid ;
    TID tid ;
    SWCNTRL swctrl ;
    HSWITCH hswitch ;
    PFNWP pfnwp ;
    COLOR clr = CLR_WHITE ;
    CHAR szKey[] = "PMEDIT", szWindowTitle[] = "SDI Editor" ;
    CHAR szFontName[ 100] ;
    PMLEDATA pmledata ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // application window
    hwndFrame = WinCreateStdWindow( HWND_DESKTOP, 0L,
                                   &flFrameFlags,
                                   WC_MLE,
                                   "Untitled",
                                   MLS_VSCROLL | MLS_HSCROLL,
                                   NULLHANDLE, RS_ALL,
                                   &hwndMLE) ;

    // set the owner
    WinSetOwner( hwndMLE, hwndFrame) ;

    // white background
    //WinSetPresParam( hwndMLE, PP_BACKGROUNDCOLORINDEX, sizeof( clr), &clr) ;

    // subclassing the frame window
    pfnwp = WinSubclassWindow( hwndFrame, FrameWndProc) ;
    WinSendMsg( hwndFrame, WM_PASSPROC, MPFROMP( pfnwp), 0L) ;

    // subclassing the WC_MLE window
    pfnwp = WinSubclassWindow( hwndMLE, NewMleWndProc) ;
    WinSendMsg( hwndMLE, WM_PASSPROC, MPFROMP( pfnwp), MPFROMHWND( hwndFrame)) ;

    // subclassing the Titlebar Menu window
    pfnwp = WinSubclassWindow( CTRL( hwndFrame, FID_SYSMENU), TitleMenuWndProc) ;
    WinSendMsg( CTRL( hwndFrame, FID_SYSMENU), WM_PASSPROC, MPFROMP( pfnwp), 0L) ;

    // subclassing the Titlebar window
    pfnwp = WinSubclassWindow( CTRL( hwndFrame, FID_TITLEBAR), TitlebarWndProc) ;
    WinSendMsg( CTRL( hwndFrame, FID_TITLEBAR), WM_PASSPROC, MPFROMP( pfnwp), 0L) ;

   // how many params on the command line?
   if( argc > 1)
   {
       OpenFile( hwndMLE, *++argv) ;
   }

    // set focus on the MLE
    WinSetFocus( HWND_DESKTOP, hwndMLE) ;

    // store information for Window List
    WinQueryWindowProcess( hwndFrame, &pid, &tid) ;
    swctrl.hwnd = hwndFrame ;
    swctrl.hwndIcon = NULLHANDLE ;
    swctrl.hprog = NULLHANDLE ;
    swctrl.idSession = 0L ;
    swctrl.bProgType = 0L ;
    swctrl.idProcess = pid ;
    swctrl.uchVisibility = SWL_VISIBLE | SWL_GRAYED ;
    swctrl.fbJump = SWL_NOTJUMPABLE ;
    strcpy( swctrl.szSwtitle, "PM Edit") ;
    hswitch = WinAddSwitchEntry( &swctrl) ;

    if( !WinRestoreWindowPos( szWindowTitle, szKey, hwndFrame))
    {
        SWP swp ;

        WinQueryTaskSizePos( hab, 0, &swp) ;
        WinSetWindowPos( hwndFrame, HWND_TOP,
                         swp.x, swp.y, swp.cx, swp.cy,
                         SWP_SIZE | SWP_MOVE) ;
    }

    WinQueryPresParam(  hwndMLE, PP_FONTNAMESIZE,
                        0L, NULL,
                        sizeof( szFontName),
                        (PULONG)szFontName,
                        QPF_NOINHERIT) ;

    // select the previous font
    WinSetPresParam(    hwndMLE, PP_FONTNAMESIZE,
                        sizeof( szFontName), szFontName) ;

    WinQueryPresParam(  hwndMLE, PP_BACKGROUNDCOLOR,
                        0L, NULL,
                        sizeof( clr),
                        (PULONG)&clr,
                        QPF_NOINHERIT) ;

    // select the previous font
    WinSetPresParam(    hwndMLE, PP_BACKGROUNDCOLOR,
                        sizeof( clr), &clr) ;

    // access the MLEDATA area
    pmledata = (PMLEDATA)WinQueryWindowULong( hwndMLE, QWL_USER) ;

    // set the initial colors
    pmledata -> pattrib -> clrOldBack = clr ;
    pmledata -> pattrib -> clrNewBack = clr ;

    WinQueryPresParam(  hwndMLE, PP_FOREGROUNDCOLOR,
                        0L, NULL,
                        sizeof( clr),
                        (PULONG)&clr,
                        QPF_NOINHERIT) ;

    // select the previous font
    WinSetPresParam(    hwndMLE, PP_FOREGROUNDCOLOR,
                        sizeof( clr), &clr) ;

    // set the new colors
    pmledata -> pattrib -> clrOldText = clr ;
    pmledata -> pattrib -> clrNewText = clr ;

    WinShowWindow( hwndFrame, TRUE) ;
    WinSetActiveWindow( HWND_DESKTOP, hwndFrame) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    WinShowWindow( hwndFrame, FALSE) ;
    WinStoreWindowPos( szWindowTitle, szKey, hwndFrame) ;

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
           return 0L ;

        case DM_DRAGOVER:
        case DM_DRAGLEAVE:
            return MRFROMLONG( 0L) ;

        case WM_SYSCOMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case SC_CLOSE:
                    WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
                    break ;
            }
            break ;
   }
   return (pfnwp)( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY TitleMenuWndProc(  HWND hwnd,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
    static PFNWP pfnwp ;

    switch( msg)
    {
        case WM_PASSPROC:
            pfnwp = (PFNWP)mp1 ;
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
            CHAR szFullPathName[ 250], szWindowTitle[ 80], szFileName[ 256] ;
            HWND hwndTarget, hwndFrame = PAPA( hwnd) ;
            HWND hwndMLE ;
            HOBJECT hobj ;

            // get the MLE window handle
            hwndMLE = CTRL( hwndFrame, FID_CLIENT) ;

            // query window title
            WinQueryWindowText( hwndFrame, sizeof( szWindowTitle), szWindowTitle) ;

            // get the full path name
            if( !GetFullPathName( szFullPathName, sizeof( szFullPathName)))
                return FALSE ;

            if( !SaveText( hwndMLE, szWindowTitle))
                break ;

            // copy the pathname
            strcpy( szFileName, szFullPathName) ;
            strcat( szFileName, szWindowTitle) ;

            // set the new attribute to the document
            hobj = WinQueryObject( szFileName) ;

            if( !hobj)
                    WinAlarm( HWND_DESKTOP, WA_ERROR) ;
            WinSetObjectData( hobj, "ASSOCTYPE=C Code") ;

            // allocate a DRAGINFO structure
            pdrginfo = DrgAllocDraginfo( 1L) ;

            // all drag operations refer to the titlebar menu icon window
            pdrginfo -> hwndSource = hwnd ;

            // initialize the DRAGITEM structure
            drgitem.hwndItem = hwnd ;               // titlebar icon
            drgitem.ulItemID = 100L ;
            drgitem.hstrType = DrgAddStrHandle( DRT_TEXT) ;
            drgitem.hstrRMF = DrgAddStrHandle( "(DRM_OS2FILE,DRM_PRINT)x(DRF_TEXT)") ; //"(DRM_OS2FILE)X(DRF_TEXT,DRF_UNKNOWN)") ;
            drgitem.hstrContainerName = DrgAddStrHandle( szFullPathName) ;
            drgitem.hstrSourceName = DrgAddStrHandle( szWindowTitle) ;
            drgitem.hstrTargetName = NULLHANDLE ; //DrgAddStrHandle( szWindowTitle) ;
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

            // source window il the MLE (the client)
            hwndTarget = DrgDrag(   hwnd, pdrginfo,
                                    &drgimage,
                                    1L, VK_ENDDRAG,
                                    NULL) ;

            // activate the frame window
            WinSetActiveWindow( HWND_DESKTOP, PAPA( hwnd)) ;
        }
            break ;

        case WM_BUTTON2MOTIONEND:
        {
            HOBJECT hobj ;

        }
            break ;

        case WM_ENDDRAG:
            WinAlarm( HWND_DESKTOP, WA_NOTE) ;
            break ;

        case DM_DRAGOVERNOTIFY:
        {
        }
            break ;

        case DM_ENDCONVERSATION:
            break ;

        default:
            break ;
    }
    return (* pfnwp)( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY TitlebarWndProc(   HWND hwnd,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
    static PFNWP pfnwp ;
    static HWND hwndEdit ;

    switch( msg)
    {
        case WM_PASSPROC:
        {
            pfnwp = (PFNWP) mp1 ;

            // create the edit window
            hwndEdit = WinCreateWindow( hwnd,
                                        WC_ENTRYFIELD,
                                        NULL,
                                        0L,
                                        0L, 0L, 0L, 0L,
                                        hwnd, HWND_TOP,
                                        CT_ENTRYFIELD, NULL, NULL) ;
            // subclassing the entryfieldwindow
            WinSendMsg( hwndEdit, WM_PASSPROC,
                        MPFROMP( WinSubclassWindow( hwndEdit, EditWndProc)), 0L) ;
        }
            return 0L ;

        case WM_TEXTEDIT:
        {
            RECTL rc ;
            CHAR szWindowTitle[ 200] ;

            // query the titlebar dimension
            WinQueryWindowRect( hwnd, &rc) ;

            // query the window title
            WinQueryWindowText( PAPA( hwnd), sizeof( szWindowTitle), szWindowTitle) ;

            // set the entryfield text
            WinSetWindowText( hwndEdit, szWindowTitle) ;

            // reposition the entryfield
            WinSetWindowPos(    hwndEdit, HWND_TOP,
                                0L, 0L,
                                ( rc.xRight - rc.xLeft), ( rc.yTop -rc.yBottom),
                                SWP_SHOW| SWP_ZORDER | SWP_SIZE) ;
            // set focus on the new window
            WinSetFocus( HWND_DESKTOP, hwndEdit) ;

            WinAlarm( HWND_DESKTOP, WA_NOTE) ;
            return 0L ;
        }
            break ;
   }
   return (pfnwp)( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY EditWndProc(   HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static PFNWP pfnwp ;

    switch( msg)
    {
        case WM_PASSPROC:
        {
            pfnwp = (PFNWP) mp1 ;
        }
            return 0L ;

        case WM_SETFOCUS:
            // check if the entryfiled is loosing the focus and it is still visible
            if( !SHORT1FROMMP( mp2) && WinIsWindowVisible( hwnd))
                // simulate a carriage return
                WinSendMsg( hwnd, WM_CHAR, 0L, MPFROM2SHORT( 0, VK_ENTER)) ;
            break ;

        case WM_CHAR:
            switch( CHARMSG( &msg) -> vkey)
            {
                case VK_ENTER:
                case VK_NEWLINE:        // carriage return
                {
                    CHAR szWindowTitle[ 200] ;

                    // get the entryfield text
                    WinQueryWindowText( hwnd, sizeof( szWindowTitle), szWindowTitle) ;

                    // set the new application title
                    WinSetWindowText( PAPA( PAPA( hwnd)), szWindowTitle) ;

                    // remove the entryfield window from the screen
                    WinShowWindow( hwnd, FALSE) ;
                }
                    break ;

                case VK_ESC:        // escape
                {
                    // remove the entryfield window from the screen
                    WinShowWindow( hwnd, FALSE) ;
                }
                    break ;
            }
            break ;
   }
   return (pfnwp)( hwnd, msg, mp1, mp2) ;
}


MRESULT EXPENTRY NewMleWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static PFNWP pfnwp ;
    static HWND hpopup ;
    static HWND hwndHelpInstance ;
    static BOOL fDraw ;
    static RECTL rc ;

    switch( msg)
    {
        case WM_PASSPROC:
        {
            MENUITEM mi ;
            ULONG ulStyle ;
            ULONG ulFCFrameFlags = FCF_NOMOVEWITHOWNER | FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE ;
            HWND hmenu, hwndPage, hwndFrame, hwndPapa, hwndNotebook, hwndLine ;
            CHAR szPresParam[] = "8.Helv" ;
            COLOR clr = CLR_WHITE ;
            APIRET apiret ;
            PNBKDATA pNbkData ;
            PMLEDATA pmledata ;

            // store the WC_MLE window procedure address
            pfnwp = (PFNWP)mp1 ;
            hwndPapa = HWNDFROMMP( mp2) ;

            // loading the window context menu
            hmenu = WinLoadMenu( hwndPapa, NULLHANDLE, RS_NEWSYS) ;

            // modify to become the frame window sysmenu
            ulStyle = WinQueryWindowULong( hmenu, QWL_STYLE) ;
            WinSetWindowULong( hmenu, QWL_STYLE, ulStyle | MS_TITLEBUTTON) ;

            SetConditionalMenu( hmenu, PM_OPEN) ;
            SetConditionalMenu( hmenu, PM_HELP) ;
            SetConditionalMenu( hmenu, PM_CREATEAN) ;

            // now it is a sysmenu
            WinSetWindowUShort( hmenu, QWS_ID, FID_SYSMENU) ;

            // update the frame window
            WinSendMsg( hwndPapa, WM_UPDATEFRAME, MPFROMLONG( FCF_SYSMENU), 0L) ;

            // loading the popup menu from its template
            hpopup = WinLoadMenu( HWND_DESKTOP, NULLHANDLE, RS_NEWSYS) ;

            // assign the FID_SYSMENU ID
            WinSetWindowUShort( hpopup, QWS_ID, FID_SYSMENU) ;
            // query the menu
            WinSendMsg( hpopup, MM_QUERYITEM, MPFROM2SHORT( FID_SYSMENU, TRUE), MPFROMP( &mi)) ;
            // take just the drop-down
            hpopup = mi.hwndSubMenu ;
            // add the mini-pushbuttons
            SetConditionalMenu( hpopup, PM_OPEN) ;
            SetConditionalMenu( hpopup, PM_HELP) ;
            SetConditionalMenu( hpopup, PM_CREATEAN) ;

            // Settings window
            hwndFrame = WinCreateStdWindow( HWND_DESKTOP, 0L,
                                            &ulFCFrameFlags,
                                            WC_NOTEBOOK,
                                            "Editor - Settings",
                                            BKS_BACKPAGESBR | BKS_MAJORTABRIGHT |
                                            BKS_SQUARETABS | BKS_STATUSTEXTLEFT | BKS_SPIRALBIND,
                                            NULLHANDLE, RS_ICON,
                                            &hwndNotebook) ;

            // set the Settings frame window as the notebook's owner
            WinSetOwner( hwndNotebook, hwndFrame) ;
            // set the MLE window as the Settings frame window's owner
            WinSetOwner( hwndFrame, hwnd) ;

            // allocate a block of memory
            if( DosAllocMem(    (PPVOID)&pmledata, sizeof( MLEDATA) + sizeof( ATTRIBUTES) + sizeof( VALUES),
                                PAG_READ | PAG_WRITE | PAG_COMMIT))
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                DosExit( EXIT_PROCESS, 1L) ;
            }

            // store the notebook's window handle in the MLEDATA area
            pmledata -> hwndNbk = hwndNotebook ;

            // update the internal references
            pmledata -> pattrib = (PATTRIBUTES)( pmledata + 1) ;
            pmledata -> pval = (PVALUES)( pmledata -> pattrib + 1) ;

            // store the pointer to the MLEDATA area in the MLE QWL_USER AREA
            WinSetWindowULong( hwnd, QWL_USER, (ULONG)pmledata) ;

            // setting the new font in the notebook window
            clr = CLR_PALEGRAY ;
            WinSetPresParam( hwndNotebook, PP_FONTNAMESIZE, sizeof( szPresParam), szPresParam) ;
            WinSetPresParam( hwndNotebook, PP_BACKGROUNDCOLORINDEX, sizeof( clr), &clr) ;

            // allocate room for a NBKDATA structure
            apiret = DosAllocMem( (PPVOID)&pNbkData, sizeof( NBKDATA), PAG_READ | PAG_WRITE | PAG_COMMIT) ;
            if( apiret)
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                DosExit( EXIT_PROCESS, 1) ;
            }
            // store the pointer
            WinSetWindowULong( hwndNotebook, QWL_USER, (ULONG)pNbkData) ;

            // subclassing the Notebook window to trap WM_CLOSE
            WinSendMsg( hwndNotebook, WM_PASSPROC,
                        MPFROMP( WinSubclassWindow( hwndNotebook, NotebookWndProc)), 0L) ;

            // pages definitions
            hwndPage = WinLoadDlg(  hwndNotebook, hwndNotebook, SettingsWndProc,
                                    NULLHANDLE, NB_SETTINGS_PAGE01, (PVOID)&hwndNotebook) ;
            InsertPage( hwndNotebook, hwndPage, "Document attributes", "Attributes") ;
            // loading Values page
            hwndPage = WinLoadDlg(  hwndNotebook, hwndNotebook, ValuesWndProc,
                                    NULLHANDLE, NB_SETTINGS_PAGE02, (PVOID)&hwndNotebook) ;

            InsertPage( hwndNotebook, hwndPage, "Editor values", "Values") ;
            InsertPage( hwndNotebook, NULLHANDLE, "Setting general values", "General") ;

            // add WS_CLIPCHILDREN to the titlebar
            WinSetWindowBits( CTRL( PAPA( hwnd), FID_TITLEBAR), QWL_STYLE, WS_CLIPCHILDREN, WS_CLIPCHILDREN) ;

            // line number window
            hwndLine = WinCreateWindow( CTRL( PAPA( hwnd), FID_TITLEBAR),
                                        WC_STATIC,
                                        NULL,
                                        0L,
                                        0L, 0L,
                                        100L, SYS( SV_CYTITLEBAR),
                                        CTRL( PAPA( hwnd), FID_TITLEBAR),
                                        HWND_TOP,
                                        CT_LINES,
                                        NULL, NULL) ;


/*
            // help
            {
                HELPINIT hinit ;

                hinit.cb = sizeof( HELPINIT) ;
                hinit.ulReturnCode = 0L ;
                hinit.pszTutorialName = NULL ;
                hinit.phtHelpTable = (PHELPTABLE)(0xFfff0000 | HT_MAIN) ;
                hinit.hmodHelpTableModule = (HMODULE) NULL ;
                hinit.hmodAccelActionBarModule = (HMODULE)NULL ;
                hinit.idAccelTable = 0 ;
                hinit.idActionBar = 0 ;
                hinit.pszHelpWindowTitle = "My first help" ;
                hinit.ulShowPanelId = CMIC_SHOW_PANEL_ID ;
                hinit.pszHelpLibraryName = "pmmdi.hlp" ;

                hwndHelpInstance = WinCreateHelpInstance( HAB( hwnd), &hinit) ;

                if( !hwndHelpInstance || hinit.ulReturnCode)
                {
                    if( hwndHelpInstance)
                        WinDestroyHelpInstance( hwndHelpInstance) ;
                    hwndHelpInstance = 0 ;
                }
                else
                    WinAssociateHelpInstance( hwndHelpInstance, PAPA( hwnd)) ;
            }
*/
        }
            return (MRESULT)0L ;

        case WM_CLOSE:
        {
            PNBKDATA pNbkData ;
            PMLEDATA pmledata ;
            HWND hwndNotebook ;

            // disassociazione dell'help
            //WinAssociateHelpInstance( hwndHelpInstance, NULL) ;
            //WinDestroyHelpInstance( hwndHelpInstance) ;

            // get the notebook window handle
            pmledata = (PMLEDATA)WinQueryWindowULong( hwnd, QWL_USER) ;
            hwndNotebook = pmledata -> hwndNbk ;

            // retrieve the pointer
            pNbkData = (PNBKDATA)WinQueryWindowULong( hwndNotebook, QWL_USER) ;

            // destroy the color palette
            if( pNbkData -> hwndClr)
                WinPostMsg( pNbkData -> hwndClr, WM_SYSCOMMAND, MPFROMSHORT( SC_CLOSE), 0L) ;

            // destroy the font palette
            if( pNbkData -> hwndFnt)
                WinPostMsg( pNbkData -> hwndFnt, WM_SYSCOMMAND, MPFROMSHORT( SC_CLOSE), 0L) ;

            // destroy the notebook
            WinDestroyWindow( PAPA( hwndNotebook)) ;

            // free the NBKDATA chunck of memory
            DosFreeMem( (PVOID)pNbkData) ;
 
            // terminate the app
            WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
        }
            break ;

       case WM_COMMAND:
           switch( COMMANDMSG( &msg) -> cmd)
           {
                case MN_EDITTITLE:
                    // simulate a WM_TEXTEDIT ON THE TITLEBAR
                    WinSendMsg( CTRL( PAPA( hwnd), FID_TITLEBAR), WM_TEXTEDIT, 0L, 0L) ;
                    break ;

               case MN_EDITOR:
               {
                    RESULTCODES resCodes ;
                    APIRET apiret ;
                    PPIB ppib ;
                    PTIB ptib ;

                    // get the file info
                    DosGetInfoBlocks( &ptib, &ppib) ;

                   // start a new copy
                   apiret = DosExecPgm( NULL, 0L,
                                        EXEC_ASYNC,
                                        ppib -> pib_pchcmd,
                                        NULL,
                                        &resCodes,
                                        ppib -> pib_pchcmd) ;
               }
                   break ;

                case MN_DEFAULT:
                {
                    PTIB ptib ;
                    PPIB ppib ;
                    CHAR szSetup[ 200] ;

                    // get the application information
                    DosGetInfoBlocks( &ptib, &ppib) ;
                    sprintf( szSetup, "EXENAME=%s", ppib -> pib_pchcmd) ;

                    WinCreateObject(    "WPProgram",
                                        "PM EDIT",
                                        szSetup,
                                        "<WP_DESKTOP>",
                                        CO_REPLACEIFEXISTS) ;
                }
                    break ;

                case MN_CLOSEDOC:
                    break ;

                case MN_EXIT:
                {
                    WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
                }
                    break ;

                case MN_FONTS:
                    break ;

                case PM_SENDIT:
                {
                    HOBJECT hobj ;
                    CHAR szFullPathName[ 200] ;
                    CHAR szWindowTitle[] = "MAILOBJ" ;

                    // get the full path name
                    if( !GetFullPathName( szFullPathName, sizeof( szFullPathName)))
                        return FALSE ;

                    if( !SaveText( hwnd, szWindowTitle))
                        break ;

                    // add a backslash if necessary
                    if( *( szFullPathName + strlen( szFullPathName) - 1) != '\\')
                        strcat( szFullPathName, "\\") ;
                    strcat( szFullPathName, szWindowTitle) ;
                    hobj = WinQueryObject( szFullPathName) ;
                    WinSetObjectData( hobj, "SENDIT=YES") ;
                }
                    break ;

                case MN_PRODINFO:
                {
                    DLGDATA dlgData ;

                    dlgData.hwndOwner = hwnd ;
                    strcpy( dlgData.szWindowTitle, "PM EDIT") ;
                    MieDlgWnd( hwnd, &dlgData) ;
                }
                    break;

                case MN_FIND:
                    // WinDlgBox( HWND_DESKTOP, hwnd, Search, 0, 307, (PVOID)&hwnd) ;
                    break ;

                case MN_GOTO:
                    WinDlgBox( HWND_DESKTOP, hwnd, Goto, 0, 259, (PVOID)&hwnd) ;
                    break ;

            case MN_SETTINGS:
            {
                PNBKDATA pNbkData ;
                HWND hwndNotebook ;
                PMLEDATA pmledata ;

                // get the notebook window handle
                pmledata = (PMLEDATA)WinQueryWindowULong( hwnd, QWL_USER) ;
                hwndNotebook = pmledata -> hwndNbk ;

                // check if the notebook's frame is visible, then show the notebook
                if( !WinIsWindowVisible( PAPA( hwndNotebook)))
                {
                    // show the notebook
                    WinSetWindowPos(    PAPA( hwndNotebook), HWND_TOP, 0L, 0L, 425L, 400L,
                                        SWP_SIZE | SWP_MOVE | SWP_ZORDER | SWP_ACTIVATE | SWP_SHOW) ;

                    // retrieve the pointer
                    pNbkData = (PNBKDATA)WinQueryWindowULong( hwndNotebook, QWL_USER) ;

                    // show the color palette
                    if( pNbkData -> hwndClr)
                        WinShowWindow( pNbkData -> hwndClr, TRUE) ;
                    // show the font palette
                    if( pNbkData -> hwndFnt)
                        WinShowWindow( pNbkData -> hwndClr, TRUE) ;
                }
            }
                break ;

                case MN_HELP1:
                case MN_HELP2:
                case MN_HELP3:
                case MN_HELP4:
                    WinAlarm( HWND_DESKTOP, WA_WARNING) ;
                    WinSendMsg( hwndHelpInstance, HM_DISPLAY_HELP, 0L, 0L) ;
                    break ;

                default:
                    break ;
        }
            break ;

        case WM_SIZE:
        {
            rc.xLeft = 3L ;
            rc.xRight = SHORT1FROMMP( mp2) - SYS( SV_CXVSCROLL) ;
            rc.yBottom = 3L + SYS( SV_CYHSCROLL) ;
            rc.yTop = SHORT2FROMMP( mp2) ;
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
            if( !DrgVerifyType( pdrgitem, "Plain Text,Document"))
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
            if( usDOR == DOR_DROP && !fDraw)
            {
                MieEmphasizeWindow( hwnd, hps, &rc) ;
                fDraw = TRUE ;
            }
            // release the PS handle
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
            MieEmphasizeWindow( hwnd, hps, &rc) ;
            fDraw = FALSE ;
            DrgReleasePS( hps) ;
        }
            return (MRESULT)NULL ;

        case DM_DROP:
        {
            ULONG ulItem ;
            HPS hps ;
            CHAR szContainer[ 260] , szFileName[ 80], szSourceFile[ 80], szWindowTitle[ 80] ;
            PDRAGINFO pdrginfo = (PDRAGINFO)mp1;
            PDRAGITEM pdrgitem ;
            APIRET apiret ;

            if( !DrgAccessDraginfo( pdrginfo))
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                // free the info block
                DrgFreeDraginfo( pdrginfo) ;
                break ;
            }

            // #of DRAGITEM structures
            ulItem = DrgQueryDragitemCount( pdrginfo) ;
            pdrgitem = DrgQueryDragitemPtr( pdrginfo, 0L) ;

            // retrieve the source container name
            DrgQueryStrName( pdrgitem -> hstrContainerName, sizeof( szContainer), szContainer) ;

            // retrieve the source file name
            DrgQueryStrName( pdrgitem -> hstrSourceName, sizeof( szSourceFile), szSourceFile) ;

            // retrieve the file name
            DrgQueryStrName( pdrgitem -> hstrTargetName, sizeof( szFileName), szFileName) ;

            // create full pathname
            strcat( szContainer, szSourceFile) ;

            // query the window title
            WinQueryWindowText( PAPA( hwnd), sizeof( szWindowTitle), szWindowTitle) ;

            // emphasize to FALSE
            fDraw = FALSE ;

            // remove the enphatization border
            hps = DrgGetPS( hwnd) ;
            MieEmphasizeWindow( hwnd, hps, &rc) ;
            DrgReleasePS( hps) ;

            // is the mle window empty?
            if( WinSendMsg( hwnd, MLM_QUERYTEXTLENGTH, 0L, 0L))
            {
                CHAR szFullPathName[ 200], szCmdLine[ 260] ;
                RESULTCODES resCodes ;

                // get the current path name
                GetFullPathName( szFullPathName, sizeof( szFullPathName)) ;
                sprintf( szCmdLine, "%s\\PMEDIT.EXE\0", szFullPathName) ;
                sprintf( szCmdLine + strlen( szCmdLine) + 1, "%c%s%c", '"', szContainer, '"') ;
                // start a new copy
                apiret = DosExecPgm(    NULL, 0L,
                                        EXEC_ASYNC,
                                        szCmdLine,
                                        NULL,
                                        &resCodes,
                                        "PMEDIT.EXE") ;

                // free the info block
                DrgFreeDraginfo( pdrginfo) ;
 
                // skip the rest
                break ;
            }
            // open the file
            OpenFile( hwnd, szContainer) ;

            // free the info block
            DrgFreeDraginfo( pdrginfo) ;
        }
            break ;

        case WM_PRESPARAMCHANGED:
        {
            ULONG ulAttrFound, ulValue ;
            PMLEDATA pmledata ;
            PNBKDATA pNbkData ;
            HWND hwndNotebook ;
            HWND hwndt ;
            LONG lPageID ;

            // get the notebook hwnd
            pmledata = (PMLEDATA)WinQueryWindowULong( hwnd, QWL_USER) ;
            hwndNotebook = pmledata -> hwndNbk ;
            // access NBKDATA data structure
            pNbkData = (PNBKDATA)WinQueryWindowPtr( hwndNotebook, 0L) ;
 
            // store the last action
            pmledata -> pattrib -> lAction = LONGFROMMP( mp1) ;

            // get the Setting page ID
            lPageID = (LONG)WinSendMsg( hwndNotebook, BKM_QUERYPAGEID,
                                        0L, MPFROM2SHORT( BKA_FIRST, BKA_MAJOR)) ;
            // get the Setting dialog window handle
            hwndt = WinSendMsg( hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                MPFROMLONG( lPageID), 0L) ;

            // get the changed presparam
            WinQueryPresParam(  hwnd, LONGFROMMP( mp1), 0L,
                                &ulAttrFound,
                                sizeof( ulValue), &ulValue, 0L) ;

            switch( LONGFROMMP( mp1))
            {
                case PP_BACKGROUNDCOLOR:
                    pmledata -> pattrib -> clrOldBack = pmledata -> pattrib -> clrNewBack ;
                    pmledata -> pattrib -> clrNewBack = ulValue ;

                    // change the example box
                    WinSetPresParam(    CTRL( hwndt, CT_BACKEX),
                                        PP_BACKGROUNDCOLOR, sizeof( ulValue), &ulValue) ;
                    break ;

                case PP_FOREGROUNDCOLOR:
                    // test if the old color is empty
                    pmledata -> pattrib -> clrOldText = pmledata -> pattrib -> clrNewText ;
                    pmledata -> pattrib -> clrNewText = ulValue ;

                    // change the example box
                    WinSetPresParam(    CTRL( hwndt, CT_TEXTEX),
                                        PP_BACKGROUNDCOLOR, sizeof( ulValue), &ulValue) ;
                    break ;

                case PP_FONTNAMESIZE:
                    break ;
            }
        }
            break ;

        case MLM_SETBACKCOLOR:
        {
            PNBKDATA pNbkData ;
            HWND hwndNotebook ;
            PMLEDATA pmledata ;

            // get the notebook hwnd
            pmledata = (PMLEDATA)WinQueryWindowULong( hwnd, QWL_USER) ;
            hwndNotebook = pmledata -> hwndNbk ;
            // access NBKDATA data structure
            pNbkData = (PNBKDATA)WinQueryWindowPtr( hwndNotebook, 0L) ;

/*
            // test if it is the text color
            if( pNbkData -> lWhich == CT_TEXTCOLOR)
            {
                // set the text color
                WinSendMsg( hwnd, MLM_SETTEXTCOLOR, mp1, 0L) ;

                return (MRESULT)TRUE ;
            }
*/
        }
            break ;

        case WM_CONTEXTMENU:
        {
            POINTL pt ;

            // mapping the cursor position
            pt.x = SHORT1FROMMP( mp1) ;
            pt.y = SHORT2FROMMP( mp1) ;
            WinMapWindowPoints( hwnd, HWND_DESKTOP, &pt, 1L) ;

            // emphasize the window
            MieEmphasizeWindow( hwnd, NULLHANDLE, &rc) ;
 
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

        case WM_SETFOCUS:
            if( HWNDFROMMP( mp1) == hpopup && SHORT1FROMMP( mp2))
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                MieEmphasizeWindow( hwnd, NULLHANDLE, &rc) ;
            }
            break ;
   }
   return ( * pfnwp)( hwnd, msg, mp1, mp2) ;
}


MRESULT EXPENTRY NotebookWndProc(   HWND hwnd,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
    static PFNWP pfnwp ;
    static USHORT usRadioBtn ;
    static HWND hwndRadio ;

   switch( msg)
   {
       case WM_PASSPROC:
           pfnwp = (PFNWP) mp1 ;
           return 0L ;

        case WM_CLOSE:
        {
            PNBKDATA pNbkData ;

            // retrieve the pointer
            pNbkData = (PNBKDATA)WinQueryWindowULong( hwnd, QWL_USER) ;

            // depress the last press radio button
            WinCheckButton( hwndRadio, usRadioBtn, FALSE) ;

            // destroy the color palette
            if( pNbkData -> hwndClr)
            {
                WinPostMsg( pNbkData -> hwndClr, WM_SYSCOMMAND, MPFROMSHORT( SC_CLOSE), 0L) ;
                pNbkData -> hwndClr = NULLHANDLE ;
            }
            // destroy the font palette
            if( pNbkData -> hwndFnt)
            {
                WinPostMsg( pNbkData -> hwndFnt, WM_SYSCOMMAND, MPFROMSHORT( SC_CLOSE), 0L) ;
                pNbkData -> hwndFnt = NULLHANDLE ;
            }
            // hide the notebook window
            WinShowWindow( PAPA( hwnd), FALSE) ;
        }
            return 0L ;

        case WM_RADIOBUTTON:
            usRadioBtn = SHORT1FROMMP( mp1) ;
            hwndRadio = HWNDFROMMP( mp2) ;
            return 0L ;

        case WM_COMMAND:
        {
           switch( COMMANDMSG( &msg) -> cmd)
           {

               case MN_SETTINGS:
               {
               }
                   break ;
           }
       }
           break ;
   }
   return (pfnwp)( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY SettingsWndProc(   HWND hwnd,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
    static HWND hwndNotebook ;

    switch( msg)
    {
        case WM_INITDLG:
        {
            COLOR clr = CLR_PALEGRAY ;
 
            // gray background
            WinSetPresParam( hwnd, PP_BACKGROUNDCOLORINDEX, sizeof( COLOR), &clr) ;
            // no radio button selected
            WinCheckButton( hwnd, CT_CLRPAL, FALSE) ;

            // get the notebook hwnd
            hwndNotebook = *((PHWND)PVOIDFROMMP( mp2)) ;
        }
            break ;

        case WM_CONTROL:
        {
            PNBKDATA pNbkData ;

            // get the pointer
            pNbkData = (PNBKDATA)WinQueryWindowULong( hwndNotebook, QWL_USER) ;

            switch( COMMANDMSG( &msg) -> cmd)
            {
                case CT_CLRPAL:
                {
                    CHAR szText[] = "Color Palette" ;

/*
                    // set the flag
                    pNbkData -> lWhich = CT_BACKCOLOR ;

                    if( COMMANDMSG( &msg) -> cmd == CT_TEXTCOLOR)
                        pNbkData -> lWhich = CT_TEXTCOLOR ;
*/
                    // check if a color palette is already available
                    if( !( pNbkData -> hwndClr || pNbkData -> hobjClr))
                    {
                        pNbkData -> hobjClr = WinQueryObject( "<WP_CLRPAL>") ;
                        if( !WinSetObjectData( pNbkData -> hobjClr, "OPEN=DEFAULT;TITLE=Color Palette"))
                            WinAlarm( HWND_DESKTOP, WA_ERROR) ;

                        // get the object's window handle
                        pNbkData -> hwndClr = TrapObjectWindow( szText) ;

                        // retrieve color palette hwnd
                        WinSetActiveWindow( HWND_DESKTOP, pNbkData -> hwndClr) ;
                    }
                    else
                    {
                        if( !WinIsWindowVisible( pNbkData -> hwndClr))
                        {
                            WinSetObjectData( pNbkData -> hobjClr, "OPEN=DEFAULT") ;
                            // get the object's window handle
                            pNbkData -> hwndClr = TrapObjectWindow( szText) ;
                        }
                        WinSetWindowPos(    pNbkData -> hwndClr, HWND_TOP,
                                            0L, 0L, 0L, 0L,
                                            SWP_ZORDER | SWP_ACTIVATE) ;
                    }
                }
                    break ;

                case CT_FNTPAL:
                {
                    CHAR szText[] = "Font Palette" ;

                    // check if a font palette is already available
                    if( !( pNbkData -> hwndFnt || pNbkData -> hobjFnt))
                    {
                        pNbkData -> hobjFnt = WinQueryObject( "<WP_FNTPAL>") ;
                        if( !WinSetObjectData( pNbkData -> hobjFnt, "OPEN=DEFAULT;TITLE=Font Palette"))
                            WinAlarm( HWND_DESKTOP, WA_ERROR) ;

                        // get the object's window handle
                        pNbkData -> hwndFnt = TrapObjectWindow( szText) ;

                        // activate the Font Palette
                        //WinSetActiveWindow( HWND_DESKTOP, pNbkData -> hwndFnt) ;
                    }
                    else
                    {
                        if( !WinIsWindowVisible( pNbkData -> hwndFnt))
                        {
                            WinSetObjectData( pNbkData -> hobjFnt, "OPEN=DEFAULT") ;
                            // get the object's window handle
                            pNbkData -> hwndFnt = TrapObjectWindow( szText) ;
                        }
                        WinSetWindowPos(    pNbkData -> hwndFnt, HWND_TOP,
                                            0L, 0L, 0L, 0L,
                                            SWP_ZORDER | SWP_ACTIVATE) ;
                    }
                }
                    break ;
            }
            // notify to the notebook window the pushed radio button
            WinSendMsg( hwndNotebook, WM_RADIOBUTTON,
                        MPFROMSHORT( COMMANDMSG( &msg) -> cmd),
                        MPFROMHWND( hwnd)) ;
        }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case CT_DEFAULT:
                {
                    PNBKDATA pNbkData ;
                    COLOR clr ;
                    HWND hwndMLE ;
                    PMLEDATA pmledata ;

                    // get the pointer
                    pNbkData = (PNBKDATA)WinQueryWindowULong( hwndNotebook, QWL_USER) ;
                    // get the MLE window handle
                    hwndMLE = OWNER( PAPA( hwndNotebook)) ;
                    // access the MLEDATA data structure
                    pmledata = (PMLEDATA)WinQueryWindowULong( hwndMLE, QWL_USER) ;

                    // query the background color
                    WinQueryPresParam(  hwndMLE, PP_BACKGROUNDCOLOR,
                                        0L, NULL,
                                        sizeof( clr), (PULONG)&clr,
                                        QPF_NOINHERIT) ;

                    // store the current background
                    pmledata -> pattrib -> clrOldBack = clr ;
                    pmledata -> pattrib -> clrNewBack = CLR_WHITE ;

                    WinSetPresParam(    OWNER( PAPA( PAPA( PAPA( hwnd)))),
                                        PP_BACKGROUNDCOLOR, sizeof( COLOR),
                                        &pmledata -> pattrib -> clrNewBack) ;

                    // query the foreground color
                    WinQueryPresParam(  hwndMLE, PP_FOREGROUNDCOLOR,
                                        0L, NULL,
                                        sizeof( clr), (PULONG)&clr,
                                        QPF_NOINHERIT) ;

                    // store the current foreground
                    pmledata -> pattrib -> clrOldText = clr ;
                    pmledata -> pattrib -> clrNewText = CLR_BLACK ;

                    WinSetPresParam(    OWNER( PAPA( PAPA( PAPA( hwnd)))),
                                        PP_FOREGROUNDCOLOR, sizeof( COLOR),
                                        &pmledata -> pattrib -> clrNewText) ;

                    // set the default font
                    WinSendMsg( OWNER( PAPA( PAPA( PAPA( hwnd)))),
                                MLM_SETFONT, MPFROMP( NULL), 0L) ;
                }
                    break ;

                case CT_UNDO:
                {
                    PNBKDATA pNbkData ;
                    PMLEDATA pmledata ;
                    HWND hwndNotebook ;

                    // access the MLEDATA data structure
                    pmledata = (PMLEDATA)WinQueryWindowULong( OWNER( PAPA( PAPA( PAPA( hwnd)))), QWL_USER) ;
                    // get the notebook hwnd
                    hwndNotebook = pmledata -> hwndNbk ;
                    // access NBKDATA data structure
                    pNbkData = (PNBKDATA)WinQueryWindowPtr( hwndNotebook, QWL_USER) ;

                    // undo the appropriate last action
                    switch( pmledata -> pattrib -> lAction)
                    {
                        case PP_FOREGROUNDCOLOR:
                            // undo the text color change
                            WinSendMsg( OWNER( PAPA( PAPA( PAPA( hwnd)))),
                                        MLM_SETTEXTCOLOR,
                                        MPFROMLONG( pmledata -> pattrib -> clrOldText), 0L) ;
                            break ;

                        case PP_BACKGROUNDCOLOR:
                            // undo the back color change
                            WinSendMsg( OWNER( PAPA( PAPA( PAPA( hwnd)))),
                                        MLM_SETBACKCOLOR,
                                        MPFROMLONG( pmledata -> pattrib -> clrOldBack), 0L) ;
                            break ;
                    }
                }
                    break ;

                case CT_HELP:
                    break ;
            }
            return 0L ;
    }
    return WinDefDlgProc( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY ValuesWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static HWND hwndNotebook ;

    switch( msg)
    {
        case WM_INITDLG:
        {
            COLOR clr = CLR_PALEGRAY ;

            // gray background
            WinSetPresParam( hwnd, PP_BACKGROUNDCOLORINDEX, sizeof( COLOR), &clr) ;
            // no radio button selected
            WinCheckButton( hwnd, CT_CLRPAL, FALSE) ;

            // get the notebook hwnd
            hwndNotebook = *((PHWND)PVOIDFROMMP( mp2)) ;
        }
            break ;

        case WM_CONTROL:
        {
            PNBKDATA pNbkData ;

            // get the pointer
            pNbkData = (PNBKDATA)WinQueryWindowULong( hwndNotebook, QWL_USER) ;

            switch( COMMANDMSG( &msg) -> cmd)
            {
                case CT_SENDIT:
                {
                    HWND hwndMLE ;
                    HOBJECT hobj ;
                    CHAR szFullPathName[ 200] ;
                    CHAR szWindowTitle[] = "MAILOBJ" ;

                    // get the MLE window handle
                    hwndMLE = OWNER( OWNER( hwndNotebook)) ;

                    // get the full path name
                    if( !GetFullPathName( szFullPathName, sizeof( szFullPathName)))
                        return FALSE ;

                    if( !SaveText( hwndMLE, szWindowTitle))
                        break ;

                    // add a backslash if necessary
                    if( *( szFullPathName + strlen( szFullPathName) - 1) != '\\')
                        strcat( szFullPathName, "\\") ;
                    strcat( szFullPathName, szWindowTitle) ;
                    hobj = WinQueryObject( szFullPathName) ;
                    WinSetObjectData( hobj, "SENDIT=YES") ;



                }
                    break ;
            }
        }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case CT_UNDO:
                    break ;

                case CT_DEFAULT:
                    break ;

                case CT_HELP:
                    break ;
            }
            return 0L ;
    }
    return WinDefDlgProc( hwnd, msg, mp1, mp2) ;
}


ULONG InsertPage(   HWND hwndNotebook,
                    HWND hwndPage,
                    PCHAR pszStatusLine,
                    PCHAR pszTabText)
{
    ULONG ulPageID ;
    SHORT sStyle = BKA_AUTOPAGESIZE ;

    sStyle |= (pszStatusLine) ? BKA_STATUSTEXTON : 0L ;
    sStyle |= (pszTabText) ? BKA_MAJOR : 0L ;

    // insert a page
    ulPageID =(ULONG) WinSendMsg(   hwndNotebook, BKM_INSERTPAGE,
                                    0L, MPFROM2SHORT( sStyle, BKA_LAST)) ;

    if( pszTabText)
    {
        // Set the size of the tab text
        WinSendMsg( hwndNotebook,           // handle of notebook window
                    BKM_SETDIMENSIONS,
                    MPFROM2SHORT( CX_MAJOR_TAB, CY_MAJOR_TAB),
                    MPFROMSHORT( BKA_MAJORTAB)) ; // Set the size of the major tab

        // Set the tab text
        WinSendMsg( hwndNotebook,
                    BKM_SETTABTEXT,
                    MPFROMP( ulPageID ),
                    MPFROMP( pszTabText)) ;
    }

    if( pszStatusLine)
        // Set the status line text
        WinSendMsg( hwndNotebook,
                    BKM_SETSTATUSLINETEXT,
                    MPFROMP( ulPageID ),
                    MPFROMP( pszStatusLine));

    if( hwndPage)
        // Associate the app page window with the notebook page
        WinSendMsg(     hwndNotebook,
                        BKM_SETPAGEWINDOWHWND,
                        MPFROMP( ulPageID),
                        MPFROMLONG( hwndPage)) ;

    return ulPageID ;
}

HWND TrapObjectWindow( PCHAR pszText)
{
    HENUM henum ;
    CHAR szClassName[ 30] ;
    CHAR szWindowTitle[ 80] ;
    HWND hwnd ;

    henum = WinBeginEnumWindows( HWND_DESKTOP) ;

    while( ( hwnd = WinGetNextWindow( henum)))
    {
        WinQueryClassName( hwnd, sizeof( szClassName), szClassName) ;
        if( strcmp( szClassName, "#1") == 0)
        {
            WinQueryWindowText( hwnd, sizeof( szWindowTitle), szWindowTitle) ;
            if( strcmp( szWindowTitle, pszText) == 0)
            {
                WinEndEnumWindows( henum) ;
                // returns frame window handle
                return hwnd ;
            }
        }
    }
    WinEndEnumWindows( henum) ;
    return NULLHANDLE ;
}

BOOL SetConditionalMenu( HWND hmenu, USHORT msg)
{
    MENUITEM mi ;
    ULONG ulStyle ;

    // query the menuitem to get the attributes
    WinSendMsg( hmenu, MM_QUERYITEM, MPFROM2SHORT( msg, TRUE), MPFROMP( &mi)) ;

    // check if it is a submenu
    if( !mi.hwndSubMenu)
        return FALSE ;

    // modify to become a conditional cascade menu
    ulStyle = WinQueryWindowULong( mi.hwndSubMenu, QWL_STYLE) ;
    if( WinSetWindowULong( mi.hwndSubMenu, QWL_STYLE, ulStyle | MS_CONDITIONALCASCADE))
        return TRUE ;
    // error
    return FALSE ;
}

ULONG GetFullPathName( PSZ pszFullPathName, ULONG ulBuffSize)
{
    APIRET apiret ;
    ULONG ulDisk, ulDriveMap ;

    // get the current disk
    apiret = DosQueryCurrentDisk( &ulDisk, &ulDriveMap) ;
    if( apiret)
        return FALSE ;
    // store the disk name
    sprintf( pszFullPathName, "%c:\\", (CHAR)( ulDisk + 64)) ;

    // store the path name
    apiret = DosQueryCurrentDir( 0L, pszFullPathName + 3, &ulBuffSize) ;
    if( apiret)
        return FALSE ;

    if( *( pszFullPathName + 3))
        strcat( pszFullPathName, "\\") ;

    return strlen( pszFullPathName) ;
}

BOOL SaveText( HWND hwndMLE, PSZ pszFullFile)
{
    ULONG ulBufferSize, ulSize, ulAction, ulBytesWritten, ulLines ;
    PCHAR pBuffer ;
    LONG lOffset = 0L ;
    HFILE hfile ;
    APIRET apiret ;
 
    // get document size
    ulLines = (ULONG)WinSendMsg(  hwndMLE, MLM_QUERYLINECOUNT,
                                  MPFROMLONG( 0L), MPFROMLONG( 0L)) ;
    ulBufferSize = (ULONG)WinSendMsg(   hwndMLE, MLM_QUERYTEXTLENGTH,
                                        MPFROMLONG( 0L), MPFROMLONG( 0L)) ;
    ulBufferSize += ulLines - 1 ;
    ulSize = ulBufferSize ;

    if( !ulBufferSize)
        return FALSE ;

    // allocating memory
    if( DosAllocMem( (PPVOID)&pBuffer, ulBufferSize, PAG_WRITE | PAG_COMMIT))
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        return FALSE ;
    }

    // define export buffer
    WinSendMsg( hwndMLE, MLM_SETIMPORTEXPORT,
                MPFROMP( pBuffer), MPFROMLONG( ulSize)) ;
    WinSendMsg( hwndMLE, MLM_EXPORT, MPFROMP( (PIPT)&lOffset), MPFROMP( &ulSize)) ;

    // save it on a file
    if( apiret = DosOpen(   pszFullFile, &hfile,
                            &ulAction,
                            ulBufferSize,
                            FILE_NORMAL,
                            OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                            OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE | OPEN_FLAGS_SEQUENTIAL,
                            NULL))
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        DosFreeMem( pBuffer) ;
        return FALSE ;
    }

    // writing the file
    if( ( apiret = DosWrite( hfile, (PVOID)pBuffer, ulBufferSize, &ulBytesWritten)))
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        DosClose( hfile) ;
        DosFreeMem( pBuffer) ;
        return FALSE ;
    }

    // close file
    DosClose( hfile) ;
    DosFreeMem( pBuffer) ;

    return TRUE ;
}

BOOL OpenFile( HWND hwndMLE, PSZ pszFileName)
{
    HFILE hfile ;
    ULONG ulAction, ulBytesRead ;
    LONG lOffset = 0L ;
    APIRET apiret ;
    FILESTATUS3 fs3 ;
    PCHAR pBuffer ;

    // open the source file
    if( apiret = DosOpen(   pszFileName, &hfile,
                            &ulAction,
                            0L,
                            FILE_NORMAL,
                            OPEN_ACTION_OPEN_IF_EXISTS,
                            OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE | OPEN_FLAGS_SEQUENTIAL,
                            NULL))
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        return FALSE ;
    }

    // query file info
    apiret = DosQueryFileInfo(  hfile, FIL_STANDARD,
                                (PVOID)&fs3, sizeof( fs3)) ;
    if( apiret || !fs3.cbFileAlloc)
        return FALSE ;

    apiret = DosAllocMem( (PPVOID)&pBuffer, fs3.cbFileAlloc, PAG_WRITE | PAG_COMMIT) ;
    if( apiret)
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        DosClose( hfile) ;
        return FALSE ;
    }

    // read the file
    if( apiret = DosRead(   hfile, (PVOID)pBuffer,
                            fs3.cbFile, &ulBytesRead))
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        DosFreeMem( pBuffer) ;
        DosClose( hfile) ;
        return FALSE ;
    }

    // set the import buffer
    WinSendMsg( hwndMLE, MLM_SETIMPORTEXPORT,
                MPFROMP( pBuffer),
                MPFROMLONG( fs3.cbFile)) ;

    WinSendMsg( hwndMLE, MLM_IMPORT,
                MPFROMP( (PIPT)&lOffset),
                MPFROMP( (PULONG)&fs3.cbFile)) ;

    // free memory and close the file
    DosFreeMem( pBuffer) ;
    DosClose( hfile) ;

    // set the new window title
    WinSetWindowText( PAPA( hwndMLE), pszFileName) ;

    return TRUE ;
}


