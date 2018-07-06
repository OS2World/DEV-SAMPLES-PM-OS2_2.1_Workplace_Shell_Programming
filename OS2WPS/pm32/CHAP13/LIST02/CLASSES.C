// CLASSES.C - WPS classes
// Listing 13-02

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_DOS
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "classes.h"
#include "mie02.h"

// definitions
#define ID_LISTBOX 1
#define ID_STATIC  2

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// macros
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define MENU( x)    WinWindowFromID( x, FID_MENU)
#define TITLE( x)   WinWindowFromID( x, FID_TITLEBAR)
#define HAB( x)     WinQueryAnchorBlock( x)
#define CTRL( x, y) WinWindowFromID( x, y)

#define WM_PASSPROC     WM_USER

// function prototypes
MRESULT EXPENTRY TitlebarWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
ULONG GetFullPathName( PSZ pszFullPathName, ULONG ulBuffSize) ;
int main( void) ;


int main( void)
{
    CHAR szWindowTitle[ 25] ;
    HWND hwndFrame, hwndClient, hwndTitlebar ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg;
    ULONG flFrameFlags = FCF_WPS & ~FCF_ACCELTABLE ;
    PFNWP pfnwp ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinLoadString(  hab, NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof szWindowTitle, szWindowTitle) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE,
                                    &flFrameFlags,
                                    WC_LISTBOX, szWindowTitle,
                                    LS_NOADJUSTPOS | WS_VISIBLE,
                                    NULLHANDLE, RS_ALL,
                                    &hwndClient) ;

    // subclass the listbox
    WinSendMsg( hwndClient, WM_PASSPROC, MPFROMP( WinSubclassWindow( hwndClient, ClientWndProc)), 0L) ;

    // subclassing the titlebar window
    hwndTitlebar = WinWindowFromID( hwndFrame, FID_SYSMENU) ;
    pfnwp = (PFNWP)WinSubclassWindow( hwndTitlebar, (PFNWP)TitlebarWndProc) ;
    WinSendMsg( hwndTitlebar, WM_PASSPROC, (MPARAM)pfnwp, 0L) ;

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
    static POBJCLASS pObjClasses ;

    switch( msg)
    {
        case WM_PASSPROC:
        {
            APIRET apiret ;
            ULONG ulSize = 0L ;
            CHAR szText[ 200] ;
            ULONG ulBuffer, ulAction = 0L, ulBytesWritten ;
            HFILE hfile ;

            // get the WC_LISTBOX procedure address
            pfnwp = (PFNWP)mp1 ;

            // get the buffer dimensions
            WinEnumObjectClasses( NULL, &ulSize) ;

            // allocate the pages
            apiret = DosAllocMem(   (PPVOID)&pObjClasses, 4096 * ( ulSize / 4096 + 1),
                                    PAG_READ | PAG_WRITE | PAG_COMMIT) ;
            if( apiret)
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // get the class names
            WinEnumObjectClasses( (POBJCLASS)pObjClasses, &ulSize) ;

            // open a file
            if( DosOpen(    "CLASSES.DOC", &hfile,
                            &ulAction,
                            10L,
                            FILE_NORMAL,
                            OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                            OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE| OPEN_FLAGS_SEQUENTIAL,
                            NULL))
            {
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                DosExit( EXIT_PROCESS, 1L) ;
            }

            // fill the listbox
             do
            {
                ulBuffer = (ULONG)sprintf( szText, "%s - %s", pObjClasses -> pszClassName, pObjClasses -> pszModName) ;
                WinSendMsg( hwnd, LM_INSERTITEM, MPFROMSHORT( LIT_SORTASCENDING), MPFROMP( szText)) ;

                strcat( szText, "\n") ;
                // writing the file
                if( DosWrite( hfile, (PVOID)szText, ulBuffer + 1, &ulBytesWritten))
                {
                    WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                    DosClose( hfile) ;
                }

                // increase the pointer
                pObjClasses = pObjClasses -> pNext ;
             } while( pObjClasses) ;

            // close file
            DosClose( hfile) ;
        }
            return 0L ;

        case WM_CLOSE:
        {
            APIRET apiret ;
            CHAR szFullPathName[ 260] ;
            PSZ pszFullFile = "CLASSES.DOC" ;

            // free associated memory
            DosFreeMem( pObjClasses) ;

            // get the full path name
            if( !GetFullPathName( szFullPathName, sizeof( szFullPathName)))
                return FALSE ;

            strcat( szFullPathName, pszFullFile) ;
            // delete the temporary file
            apiret = DosDelete( szFullPathName) ;
            if( apiret)
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        }
            break ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> source)
            {
                case CMDSRC_MENU:
                    switch( COMMANDMSG( &msg) -> cmd)
                    {
                        default:
                            break;

                        case MN_PRODINFO:
                        {
                            DLGDATA dlgData ;

                            dlgData.hwndOwner = hwnd ;
                            strcpy( dlgData.szWindowTitle, "WPS object classes") ;
                            MieDlgWnd( hwnd, &dlgData) ;
                        }
                            break ;

                        case MN_EXIT:
                            WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                            break ;
            }
                break ;
        }
        default:
            break ;
    }
    return ( * pfnwp)( hwnd, msg, mp1, mp2) ;
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
            PSZ pszFullFile = "CLASSES.DOC" ;
            HWND hwndTarget ;
            CHAR szFullPathName[ 255] ;

            // get the full path name
            if( !GetFullPathName( szFullPathName, sizeof( szFullPathName)))
                return FALSE ;

            // allocate a DRAGINFO structure
            pdrginfo = DrgAllocDraginfo( 1L) ;
            pdrginfo -> hwndSource = hwnd ;

            // initialize the DRAGITEM structure
            drgitem.hwndItem = hwnd ;
            drgitem.ulItemID = 100L ;
            drgitem.hstrType = DrgAddStrHandle( DRT_TEXT) ;
            drgitem.hstrRMF = DrgAddStrHandle( "<DRM_OS2FILE,DRF_TEXT>") ;
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
            break ;

        case WM_ENDDRAG:
            break;

        default:
            break ;
    }
    return (* pfnwp)( hwnd, msg, mp1, mp2) ;
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