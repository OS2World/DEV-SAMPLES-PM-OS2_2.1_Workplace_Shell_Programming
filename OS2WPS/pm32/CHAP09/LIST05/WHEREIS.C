// Listing 09-04

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL
#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_DOSMISC

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include "whereis.h"

// definitions
#define THREAD_STACK        8192L * 4
#define WM_PASSPROC         WM_USER + 0


#define HAB( x)         WinQueryAnchorBlock( x)

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY SPWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ListWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY TMWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
BOOL SaveText( HWND hwndList, PSZ pszFullFile) ;
ULONG GetFullPathName( PSZ pszFullPathName, ULONG ulBuffSize) ;
BOOL SetConditionalMenu( HWND hmenu, USHORT msg) ;


void ShowCurDir( ULONG ulDrive, HWND hwndCurDir) ;

#ifdef API
void _System search( PDATA pData) ;
#else
void search( PDATA pData) ;
#endif


MRESULT EXPENTRY YesNoProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "whereis" ;
    HMQ hmq ;
    HWND hwndFrame ;
    QMSG qmsg ;
    HACCEL haccel ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // main window class
    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    // load the window template
    hwndFrame = WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP,
                            NULL, NULLHANDLE, 256, NULL) ;

    // initialize all the controls
    WinSendMsg( CLIENT( hwndFrame), WM_COMMAND,
                MPFROMSHORT( ID_STARTUP), 0L) ;
    // position the window
    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        60, 170, 400, 320,
                        SWP_ACTIVATE | SWP_MOVE | SWP_SIZE) ;

    // load and set accelerators
    haccel = WinLoadAccelTable( hab, NULLHANDLE, RS_ACCELTABLE) ;
    WinSetAccelTable( hab, haccel, hwndFrame) ;

    // disable hard errors
    DosError( FERR_DISABLEHARDERR) ;

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
    static HWND hwndDrive,
                hwndCurDir,
                hwndEdit,
                hwndSearch,
                hwndFound ;

    switch( msg)
    {
        case WM_CREATE:
        {
            HPOINTER hptr ;

            hptr = WinLoadPointer( HWND_DESKTOP, 0, RS_ICON) ;
            WinSendMsg( PAPA( hwnd), WM_SETICON, MPFROMLONG( (LONG) hptr), 0L) ;
        }
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case ID_DRIVE:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case LN_SELECT:
                        {
                            ULONG ulDrivesNum, ulLogDrives ;
                            USHORT usPos ;
                            char szText[ 40] ;

                            usPos = (USHORT)WinSendMsg( hwndDrive, LM_QUERYSELECTION,
                                                        MPFROMSHORT( LIT_FIRST), 0L) ;

                            WinSendMsg( hwndDrive, LM_QUERYITEMTEXT,
                                        MPFROM2SHORT( usPos, sizeof( szText)),
                                        MPFROMP( szText)) ;

                            // are we selecting the current disk?
                            DosQueryCurrentDisk( &ulDrivesNum, &ulLogDrives) ;
                            if( ulDrivesNum == (ULONG)*szText - 64)
                                break ;

                            DosSetDefaultDisk( (ULONG)*szText - 64) ;
                            ShowCurDir( (ULONG)( *szText - 65), hwndCurDir) ;

                            // erase the string if we chamged drive
                            //*Data.szString = '\0' ;
                        }
                            break ;

                        default:
                            break ;
                    }
                    break ;
/*
                case ID_DIR:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case LN_ENTER:
                        {
                            LONG i ;
                            CHAR *szExt[] = { ".c", ".def", ".rc", ".mak", ".h"} ;
                            CHAR szPath[ 200] = "D:\\OS2\\EPM.EXE\0 " ;
                            USHORT usPos, usItem ;
                            CHAR szText[ 40] ;
                            RESULTCODES rc ;
                            CHAR szString[ 100] ;

                            usPos = (USHORT)WinSendMsg( hwndDir, LM_QUERYSELECTION,
                                                        MPFROMSHORT( LIT_FIRST), 0L) ;
                            usItem = (USHORT)WinSendMsg(    hwndDir, LM_QUERYITEMTEXT,
                                                            MPFROM2SHORT( usPos, sizeof( szText)),
                                                            MPFROMP( szText)) ;

                            // let's execute it if it is an EXE
                            if( !stricmp( ( szText + ( usItem - 3)), "EXE"))
                            {
                                DosExecPgm( szString, sizeof( szString),
                                            EXEC_ASYNC, NULL, NULL, &rc, szText) ;
                            }

                            // let's start E.EXE for H, C, MAK, RC, DEf files
                            for( i = 0; i < 5; i++)
                            {
                                if( !strcmpi( ( szText + ( usItem - strlen(szExt[ i]))), szExt[ i]))
                                {
                                    // prepare arg string
                                    strcpy( szPath + strlen( szPath) + 2, szText) ;
                                    DosExecPgm( szString, sizeof( szString),
                                                EXEC_ASYNC,
                                                szPath, NULL,
                                                &rc, "EPM.EXE") ;
                                }
                            }
                        }
                            break ;

                        case LN_SELECT:
                        {
                            USHORT usItem ;

                            if( ( usItem == (USHORT)WinSendMsg( hwndDir, LM_QUERYSELECTION,
                                                MPFROMSHORT( LIT_FIRST),
                                                MPFROMLONG( 0L))) != LIT_NONE)
                            {
                                WinEnableWindow( hwndDel, TRUE) ;
                                WinEnableWindow( hwndDelAll, TRUE) ;
                            }
                            else
                            {
                                WinEnableWindow( hwndDel, FALSE) ;
                                WinEnableWindow( hwndDelAll, FALSE) ;
                            }
                  }
                            break ;

                        default:
                            break ;
                    }
                    break ;
*/
                case ID_EDIT:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case EN_CHANGE:
                            WinEnableWindow( hwndSearch, TRUE) ;
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
                case ID_STARTUP:
                {
                    USHORT i = 0 ;
                    ULONG ulDrivesNum, ulLogDrives ;
                    CHAR szString[ 10] ;

                    // retireve control handles
                    hwndDrive = WinWindowFromID( hwnd, ID_DRIVE) ;
                    hwndEdit = WinWindowFromID( hwnd, ID_EDIT) ;
                    hwndSearch = WinWindowFromID( hwnd, ID_SEARCH) ;
                    hwndCurDir = WinWindowFromID( hwnd, ID_CURDIR) ;
                    hwndFound = WinWindowFromID( hwnd, ID_FOUND) ;

                    DosQueryCurrentDisk( &ulDrivesNum, &ulLogDrives) ;
                    while( i < 26 && ulLogDrives)
                    {
                        if( ulLogDrives & 1L)
                        {
                            sprintf( szString, "%c:", 65 + i ) ;
                            WinSendMsg( hwndDrive,
                                        LM_INSERTITEM,
                                        MPFROMSHORT( LIT_SORTASCENDING),
                                        MPFROMP( (PSZ)szString)) ;
                        }
                        ulLogDrives >>= 1 ;
                        i++ ;
                    }
                    sprintf( szString, "%c:", ulDrivesNum + 64) ;
                    WinSetWindowText( hwndDrive, (PSZ)szString) ;
                    ShowCurDir( ulDrivesNum - 1, hwndCurDir) ;
                    WinSetFocus( HWND_DESKTOP, hwndEdit) ;
                }
                    break ;

                case ID_SEARCH:
                {
                    CHAR szText[ 50] ;
                    APIRET rc ;
                    TID tid ;
                    HWND hwndSP, hwndSPClient, hwndTM ;
                    ULONG ulFCFrame = FCF_STANDARD & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_SYSMENU ;
                    static LONG i = 0 ;
                    static DATA Data[ 10] ;

                    WinQueryWindowText( hwndEdit, sizeof( szText), szText) ;

                    // skip if there is no file name
                    if( !*szText || !stricmp( szText, Data[ i].szString))
                        break ;

                    // string copied in DATA
                    strcpy( Data[ i].szString, szText) ;

                    // counter zeroed
                    Data[ i].usTot = 0 ;

                    // create new search window panel
                    hwndSP = WinCreateStdWindow(    HWND_DESKTOP,
                                                    0L,
                                                    &ulFCFrame,
                                                    WC_LISTBOX,
                                                    szText,
                                                    LS_MULTIPLESEL | LS_NOADJUSTPOS,
                                                    NULLHANDLE, RS_ICON,
                                                    &hwndSPClient) ;
 
                    // set window pos
                    WinSetWindowPos(    hwndSP, HWND_TOP,
                                        500 + i * 20, 400 + i * 20, 300, 150,
                                        SWP_SHOW | SWP_MOVE | SWP_SIZE | SWP_ZORDER) ;

                    // subclass frame window
                    WinSendMsg( hwndSP, WM_PASSPROC,
                                MPFROMP( WinSubclassWindow( hwndSP, SPWndProc)), 0L) ;

                    // subclass client window (listbox)
                    WinSendMsg( hwndSPClient, WM_PASSPROC,
                                MPFROMP( WinSubclassWindow( hwndSPClient, ListWndProc)), 0L) ;

                    // subclass title menu window
                    hwndTM = CTRL( hwndSP, FID_SYSMENU) ;
                    WinSendMsg( hwndTM, WM_PASSPROC,
                                MPFROMP( WinSubclassWindow( hwndTM, TMWndProc)), 0L) ;

                    // new panel window handle
                    Data[ i].hwnd = hwndSPClient ;

#ifdef API
                    // activate the search thread
                    rc = DosCreateThread(   &tid,
                                            (PFNTHREAD)search,
                                            (ULONG)Data + i,
                                            0L,
                                            THREAD_STACK) ;

                    if( rc)
                        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
#else
                    if( ( tid = _beginthread( search, 0, THREAD_STACK, (PVOID)&Data[ i])) == -1)
                        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
#endif
                    i++ ;
                }
                    break ;
/*
                case ID_CANCEL:
                    WinSendMsg( hwndDir, LM_DELETEALL, 0L, 0L) ;
                    WinSetWindowText( hwndEdit, NULL) ;
                    WinSetWindowText( hwndFound, "") ;
                    WinEnableWindow( hwndSearch, FALSE) ;
                    WinEnableWindow( hwndDel, FALSE) ;
                    WinEnableWindow( hwndDelAll, FALSE) ;
                    WinSetFocus( HWND_DESKTOP, hwndEdit) ;
                    break ;
*/
                case ID_QUIT:
                    WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
                    break ;
/*
                case ID_DELALL:
                {
                    USHORT usItem, usPos ;

                    usItem = (USHORT)WinSendMsg( hwndDir, LM_QUERYITEMCOUNT, 0L, 0L) ;
                    for( usPos = 0; usPos < usItem; usPos++)
                        WinSendMsg( hwndDir, LM_SELECTITEM,
                                    MPFROMSHORT( usPos), MPFROMSHORT( TRUE)) ;
                }

                case ID_DEL:
                {
                    SHORT sPos, sItem ;
                    FILESTATUS3 PathInfoBuf ;
                    char szItem[ 10], szFile[ 80] ;

                    WinSendMsg( hwndDir,LM_SETTOPINDEX, MPFROMSHORT( 0), 0L) ;

                    if( !WinDlgBox( HWND_DESKTOP, hwnd, YesNoProc,
                                    NULLHANDLE, 500, NULL))
                        break ;

                    // count item number
                    sItem = (SHORT)WinSendMsg( hwndDir, LM_QUERYITEMCOUNT, 0L, 0L) ;

                    // continue to loop until
                    while( ( sPos = (SHORT)WinSendMsg(  hwndDir, LM_QUERYSELECTION,
                                                        MPFROMSHORT( LIT_FIRST), 0L)) != LIT_NONE)
                    {
                        WinSendMsg( hwndDir, LM_QUERYITEMTEXT,
                                    MPFROM2SHORT( sPos, sizeof( szFile)),
                                    MPFROMP( szFile)) ;

                        DosQueryPathInfo(   szFile, FIL_STANDARD,
                                            &PathInfoBuf, sizeof PathInfoBuf) ;

                        if( PathInfoBuf.attrFile & ( FILE_NORMAL | FILE_ARCHIVED))
                        {
                            DosDelete( szFile) ;
                            WinSendMsg( hwndDir, LM_DELETEITEM,
                                        MPFROMSHORT( sPos), 0L) ;
                            sprintf( szItem, "%hd", --sItem) ;
                            WinSetWindowText( hwndFound, szItem) ;
                        }
                    }

                    // check if empty
                    if( !sItem)
                    {
                        WinEnableWindow( hwndDel, FALSE) ;
                        WinEnableWindow( hwndDelAll, FALSE) ;
                        WinSetWindowText( hwndEdit, "") ;
                        WinSetFocus( HWND_DESKTOP, hwndEdit) ;
                    }
                }
                    break ;
*/
                default:
                    break ;
            }
            break ;

        case WM_ERASEBACKGROUND:
            return (MRESULT)1L ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

void ShowCurDir( ULONG ulDrive, HWND hwndCurDir)
{
    CHAR szPath[ 100] ;
    ULONG cbSize = sizeof( szPath) ;

    *szPath = (CHAR)( ulDrive + 65) ;
    *(szPath + 1) = ':' ;
    *(szPath + 2) = '\\' ;

    if( DosQueryCurrentDir( ulDrive + 1, szPath + 3, &cbSize) == ERROR_INVALID_DRIVE)
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        strcpy( szPath, "NOT VALID DRIVE") ;
        // disable Search
    }
    else
    {
        WinSetFocus( HWND_DESKTOP, WinWindowFromID( PAPA( hwndCurDir), ID_EDIT)) ;
    }
    WinSetWindowText( hwndCurDir, (PSZ)szPath) ;
}

MRESULT EXPENTRY YesNoProc( HWND hwnd,
                            ULONG msg,
                            MPARAM mp1,
                            MPARAM mp2)
{
    switch( msg)
    {
        case WM_INITDLG:
        {
            COLOR clr = CLR_GREEN ;

            WinSetPresParam(CTRL( hwnd, DID_OK), PP_BACKGROUNDCOLORINDEX, sizeof clr, &clr) ;

            clr = CLR_RED ;
            WinSetPresParam( CTRL( hwnd, DID_CANCEL), PP_BACKGROUNDCOLORINDEX, sizeof clr, &clr) ;
        }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case DID_OK:
                case DID_CANCEL:
                    WinDismissDlg( hwnd, 2L - (ULONG)COMMANDMSG( &msg) -> cmd) ;
                    return (MRESULT) (2 - COMMANDMSG( &msg) -> cmd);

            }
            break ;

        default:
            break ;

    }
    return WinDefDlgProc( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY SPWndProc( HWND hwnd,
                            ULONG msg,
                            MPARAM mp1,
                            MPARAM mp2)
{
    static PFNWP pfnwp ;

    switch( msg)
    {
        case WM_PASSPROC:
        {
            pfnwp = (PFNWP)mp1 ;
        }
            return 0L ;

        case WM_SYSCOMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case SC_CLOSE:
                    WinDestroyWindow( hwnd) ;
                    return 0L ;
            }
            break ;

        default:
            break ;
    }
    return (* pfnwp)( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY ListWndProc(   HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static PFNWP pfnwp ;
    static HWND hpopup ;
    static BOOL fPopup ;

    switch( msg)
    {
        case WM_PASSPROC:
        {
            ULONG ulStyle ;
            MENUITEM mi ;
            HWND hmenu ;

            pfnwp = (PFNWP)mp1 ;

            // loading the window context menu
            hmenu = WinLoadMenu( PAPA( hwnd), NULLHANDLE, RS_TBMENU) ;

            // modify to become the frame window sysmenu
            ulStyle = WinQueryWindowULong( hmenu, QWL_STYLE) ;
            WinSetWindowULong( hmenu, QWL_STYLE, ulStyle | MS_TITLEBUTTON) ;

            SetConditionalMenu( hmenu, PM_OPEN) ;
            SetConditionalMenu( hmenu, PM_HELP) ;
            SetConditionalMenu( hmenu, PM_CREATEAN) ;

            // now it is a sysmenu
            WinSetWindowUShort( hmenu, QWS_ID, (USHORT)FID_SYSMENU) ;
            WinSendMsg( PAPA( hwnd), WM_UPDATEFRAME, MPFROMLONG( FCF_SYSMENU), 0L) ;

            // loading the popup menu from its template
            hpopup = WinLoadMenu( HWND_DESKTOP, NULLHANDLE, RS_TBMENU) ;

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
        }
            return 0L ;

        case WM_CONTEXTMENU:
        {
            POINTL ptl ;
            RECTL rc ;
            HPS hps ;

            // check if it is SHIFT+F10
            if( !mp1)
            {
                WinQueryPointerPos( HWND_DESKTOP, &ptl) ;
            }
            else
            {
                ptl.x = SHORT1FROMMP( mp1) ;
                ptl.y = SHORT2FROMMP( mp1) ;

                // converting the mouse position in HWND_DESKTOP coordinates
                WinMapWindowPoints( hwnd, HWND_DESKTOP, &ptl, 1) ;
            }

            // query the window rect
            WinQueryWindowRect( hwnd, &rc) ;
            WinInflateRect( HAB( hwnd), &rc, -5L, -5L) ;

            hps = WinGetPS( hwnd) ;
            // set upper right corner
            GpiSetCurrentPosition( hps, (PPOINTL)&rc + 1) ;

            GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rc, 20L, 20L) ;
            WinReleasePS( hps) ;

            // the menu is on
            fPopup = TRUE ;

            WinPopupMenu(   HWND_DESKTOP,
                            PAPA( hwnd),
                            hpopup,
                            ptl.x,
                            ptl.y,
                            0,
                            PU_NONE | PU_HCONSTRAIN | PU_VCONSTRAIN |
                            PU_MOUSEBUTTON2 | PU_MOUSEBUTTON1 | PU_KEYBOARD) ;
        }
            return (MPARAM)TRUE ;

        case WM_SETFOCUS:
            // check if we are getting the focus
            if( SHORT1FROMMP( mp2) && fPopup)
            {
                HPS hps ;
                RECTL rc ;

                WinQueryWindowRect( hwnd, &rc) ;
                // remove the window context menu emphazis
                WinInvalidateRect( hwnd, NULL, FALSE) ;
                break ;


                hps = WinGetPS( hwnd) ;
                WinDrawBorder( hps, &rc, 1L, 1L, CLR_WHITE, CLR_WHITE, DB_STANDARD) ;
                WinReleasePS( hps) ;
                fPopup = FALSE ;
            }
            break ;


        default:
            break ;
    }
    return (* pfnwp)( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY TMWndProc(   HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static PFNWP pfnwp ;

    switch( msg)
    {
        case WM_PASSPROC:
        {
            pfnwp = (PFNWP)mp1 ;
        }
            return 0L ;

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
            PSZ pszFullFile = "SEARCH.DAT" ;
            HWND hwndTarget, hwndClient = CTRL( PAPA( hwnd), FID_CLIENT) ;
            CHAR szFullPathName[ 255] ;

            // get the full path name
            if( !GetFullPathName( szFullPathName, sizeof( szFullPathName)))
                return FALSE ;

            if( !SaveText( hwndClient, pszFullFile))
                break ;

            // allocate a DRAGINFO structure
            pdrginfo = DrgAllocDraginfo( 1L) ;
            pdrginfo -> hwndSource = hwnd ;

            // initialize the DRAGITEM structure
            drgitem.hwndItem = hwnd ;
            drgitem.ulItemID = 100L ;
            drgitem.hstrType = DrgAddStrHandle( DRT_TEXT) ;
            drgitem.hstrRMF = DrgAddStrHandle( "(DRM_OS2FILE,DRM_PRINT)x(DRF_TEXT)") ;
            drgitem.hstrContainerName = DrgAddStrHandle( szFullPathName) ;
            drgitem.hstrSourceName = DrgAddStrHandle( pszFullFile) ;
            drgitem.hstrTargetName = DrgAddStrHandle( pszFullFile) ;
            drgitem.cxOffset = 10 ;
            drgitem.cyOffset = 10 ;
            drgitem.fsControl = 0 ;
            drgitem.fsSupportedOps = DO_MOVEABLE | DO_COPYABLE | DO_LINKABLE ;

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

            hwndTarget = DrgDrag(   hwnd,
                                    pdrginfo,
                                    &drgimage,
                                    1L,
                                    VK_ENDDRAG,
                                    NULL) ;
        }
            break ;

        case WM_BUTTON2MOTIONEND:
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
            break ;

        case WM_ENDDRAG:
        {
            APIRET rc ;
            CHAR szFullPathName[ 260] ;
            PSZ pszFullFile = "SEARCH.DAT" ;


                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // get the full path name
            if( !GetFullPathName( szFullPathName, sizeof( szFullPathName)))
                return FALSE ;

            strcat( szFullPathName, pszFullFile) ;
            // delete the temporary file
            rc = DosDelete( szFullPathName) ;
            if( rc)
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        }
            break;

        default:
            break ;
    }
    return (* pfnwp)( hwnd, msg, mp1, mp2) ;
}


BOOL SaveText( HWND hwndList, PSZ pszFullFile)
{
    ULONG ulBufferSize = 4096 * 10, ulSize, ulAction, ulBytesWritten ;
    LONG i = 0, lLen, lOffset = 0L, lLines ;
    PCHAR pBuffer, pStart ;
    HFILE hfile ;



    // check the listbox content
    if( !( lLines = WinQueryLboxCount( hwndList)))
        return FALSE ;

    // allocating memory
    if( DosAllocMem( (PPVOID)&pBuffer, ulBufferSize, PAG_WRITE | PAG_COMMIT))
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        return FALSE ;
    }

    // save the buffer starting address
    pStart = pBuffer ;

    // browse the listbox content
    for( i = 0; i < lLines; i++)
    {
        lLen = WinQueryLboxItemText( hwndList, i, pBuffer, 100) ;
        // substitute the NULL char with a NEWLINE
        *( pBuffer + lLen) = '\n' ;
        // move the pointer
        pBuffer += lLen + 1 ;
    }

    // save it on a file
    if( DosOpen(    pszFullFile, &hfile,
                    &ulAction,
                    pBuffer - pStart,
                    FILE_NORMAL,
                    OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                    OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE| OPEN_FLAGS_SEQUENTIAL,
                    NULL))
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        DosFreeMem( pBuffer) ;
        return FALSE ;
    }

    // writing the file
    if( DosWrite( hfile, (PVOID)pStart, pBuffer - pStart, &ulBytesWritten))
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
    if( ( apiret = DosQueryCurrentDir( 0L, pszFullPathName + 3, &ulBuffSize)))
        return FALSE ;

    if( *( pszFullPathName + 3))
        strcat( pszFullPathName, "\\") ;

    return strlen( pszFullPathName) ;
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

