// DRAG.C - Drag
// Listing 12-01

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include "drag.h"

// definitions & macros
#define CTRL( x, y) WinWindowFromID( x, y)
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)

#define CT_MLE      1000
#define TMP_FILE    "TMP.TXT"

#define WM_PASSPROC WM_USER + 1000
#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX


// function prototypes
int main( void) ;
MRESULT EXPENTRY FrameWndProc( HWND hwnd , ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY TitlebarWndProc( HWND hwnd , ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd , ULONG msg, MPARAM mp1, MPARAM mp2) ;
BOOL SaveText( HWND hwndMLE, PSZ pszFullFile) ;
ULONG GetFullPathName( PSZ pszFullPathName, ULONG ulBuffSize) ;


int main( void)
{
    HAB hab ;
    CHAR szClassName[] = "DRAG" ;
    CHAR szWindowTitle[] = "Drag" ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient, hwndTitlebar, hwndMLE ;
    QMSG qmsg ;
    PFNWP pfnwp ;
    ULONG ulFCFrame = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_SHELLPOSITION ;
    CHAR szFontName[ 40], szKey[] = "DATA" ;
    COLOR clr ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // DRAG class
    if( !WinRegisterClass(  hab, szClassName,
                            ClientWndProc,
                            CS_SIZEREDRAW, 0L))
        WinAlarm( HWND_DESKTOP, WA_NOTE) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP, 0L,
                                    &ulFCFrame,
                                    szClassName, szWindowTitle,
                                    0L,
                                    NULLHANDLE, RS_ICON,
                                    &hwndClient) ;

    // subclassing the FRAME window
    pfnwp = (PFNWP)WinSubclassWindow( hwndFrame, (PFNWP)FrameWndProc) ;
    WinSendMsg( hwndFrame, WM_PASSPROC, (MPARAM)pfnwp, 0L) ;

    // subclassing the titlebar window
    hwndTitlebar = WinWindowFromID( hwndFrame, FID_SYSMENU) ;
    pfnwp = (PFNWP)WinSubclassWindow( hwndTitlebar, (PFNWP)TitlebarWndProc) ;
    WinSendMsg( hwndTitlebar, WM_PASSPROC, (MPARAM)pfnwp, 0L) ;

    if( !WinRestoreWindowPos( szWindowTitle, szKey, hwndFrame))
    {
        SWP swp ;

        WinQueryTaskSizePos( hab, 0, &swp) ;
        WinSetWindowPos( hwndFrame, HWND_TOP,
                         swp.x, swp.y, swp.cx, swp.cy,
                         SWP_SIZE | SWP_MOVE) ;
    }

    // hwndMLE
    hwndMLE = CTRL( hwndClient, CT_MLE) ;

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

    WinQueryPresParam(  hwndMLE, PP_FOREGROUNDCOLOR,
                        0L, NULL,
                        sizeof( clr),
                        (PULONG)&clr,
                        QPF_NOINHERIT) ;

    // select the previous font
    WinSetPresParam(    hwndMLE, PP_FOREGROUNDCOLOR,
                        sizeof( clr), &clr) ;

    WinShowWindow( hwndFrame, TRUE) ;
    WinSetActiveWindow( HWND_DESKTOP, hwndFrame) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg) ;

    WinShowWindow( hwndFrame, FALSE) ;
    WinStoreWindowPos( szWindowTitle, szKey, hwndFrame) ;

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
            return (MPARAM)0L ;

        case WM_SYSCOMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case SC_CLOSE:
                    WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
                    break ;
            }
            break ;

        default:
            break ;
    }
    return (* pfnwp)( hwnd, msg, mp1, mp2) ;
}


MRESULT EXPENTRY ClientWndProc(    HWND hwnd,
                                   ULONG msg,
                                   MPARAM mp1,
                                   MPARAM mp2)
{
    static HWND hwndMLE ;

    switch( msg)
    {
        case WM_CREATE:
        {
            hwndMLE = WinCreateWindow(  hwnd, WC_MLE,
                                        NULL,
                                        WS_VISIBLE | MLS_VSCROLL | MLS_HSCROLL,
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
            return (MPARAM)0L ;

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
            HWND hwndTarget, hwndClient = CTRL( PAPA( hwnd), FID_CLIENT) ;
            HWND hwndMLE ;
            CHAR szFullPathName[ 255] ;

            // get the MLE window handle
            hwndMLE = CTRL( hwndClient, CT_MLE) ;

            // get the full path name
            if( !GetFullPathName( szFullPathName, sizeof( szFullPathName)))
                return FALSE ;

            if( !SaveText( hwndMLE, TMP_FILE))
                break ;

            // allocate a DRAGINFO structure
            pdrginfo = DrgAllocDraginfo( 1L) ;
            pdrginfo -> hwndSource = hwnd ;

            // initialize the DRAGITEM structure
            drgitem.hwndItem = hwnd ;
            drgitem.ulItemID = 100L ;
            drgitem.hstrType = DrgAddStrHandle( DRT_TEXT) ;
            drgitem.hstrRMF = DrgAddStrHandle( "(DRM_OS2FILE)x(DRF_TEXT,DRF_UNKNOWN)") ;
            drgitem.hstrContainerName = DrgAddStrHandle( szFullPathName) ;
            drgitem.hstrSourceName = DrgAddStrHandle( TMP_FILE) ;
            drgitem.hstrTargetName = NULLHANDLE ;
            drgitem.cxOffset = 0 ;
            drgitem.cyOffset = 0 ;
            drgitem.fsControl = DC_REMOVEABLEMEDIA ;
            drgitem.fsSupportedOps = DO_COPYABLE | DO_MOVEABLE ;

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

{
    static RECORDCORE record ;

            hwndTarget = DrgDrag(   hwnd,
                                    pdrginfo,
                                    &drgimage,
                                    1L,
                                    VK_ENDDRAG,
                                    NULL) ;

            WinSendMsg( hwndTarget, CM_QUERYDRAGIMAGE, MPFROMP( &record), 0L) ;
            WinSetWindowText( PAPA( hwnd), record.pszIcon) ;
            WinAlarm( HWND_DESKTOP, WA_NOTE) ;
}
        }
            break ;

        case WM_BUTTON2MOTIONEND:
            break ;

        case WM_ENDDRAG:
        {
            APIRET rc ;
            CHAR szFullPathName[ 260] ;

            // get the full path name
            if( !GetFullPathName( szFullPathName, sizeof( szFullPathName)))
                return FALSE ;

            strcat( szFullPathName, TMP_FILE) ;
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


BOOL SaveText( HWND hwndMLE, PSZ pszFullFile)
{
    ULONG ulBufferSize, ulSize, ulAction, ulBytesWritten, ulLines ;
    PCHAR pBuffer ;
    CHAR szDesktop[ 255] ;
    LONG lOffset = 0L ;
    HFILE hfile ;

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

/*
    // get the desktop location
    ulSize = sizeof( szDesktop) ;
    PrfQueryProfileData( HINI_PROFILE, "FolderWorkareaRunningObjects", NULL, szDesktop, &ulSize) ;
    // add the file name
    strcat( szDesktop, "\\") ;
    strcat( szDesktop, TMP_FILE) ;
*/
    // save it on a file
    if( DosOpen(    pszFullFile, &hfile,
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

    // writing the file
    if( DosWrite( hfile, (PVOID)pBuffer, ulBufferSize, &ulBytesWritten))
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
    apiret = DosQueryCurrentDir( 0l, pszFullPathName + 3, &ulBuffSize) ;
    if( apiret)
        return FALSE ;

    strcat( pszFullPathName, "\\") ;

    return strlen( pszFullPathName) ;
}

