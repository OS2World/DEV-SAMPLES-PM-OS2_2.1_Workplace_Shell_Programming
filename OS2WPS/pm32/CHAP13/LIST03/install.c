// INSTALL.C - A WPS install program
// Listing 13-03

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_DOS
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "install.h"
#include "mie02.h"
#include "where.h"

// definitions
#define ID_LISTBOX 1

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// macros
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define MENU( x)    WinWindowFromID( x, FID_MENU)
#define TITLE( x)   WinWindowFromID( x, FID_TITLEBAR)
#define HAB( x)     WinQueryAnchorBlock( x)
#define CTRL( x, y) WinWindowFromID( x, y)

#define WM_PASSPROC     WM_USER


// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY TitleMenuWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
int main( void) ;


int main( void)
{
    CHAR szWindowTitle[ 25] ;
    HWND hwndFrame, hwndClient, hwndTitleMenu ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg;
    ULONG flFrameFlags = FCF_WPS & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinLoadString(  hab, NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof szWindowTitle, szWindowTitle) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    WC_LISTBOX, szWindowTitle,
                                    LS_NOADJUSTPOS | WS_VISIBLE,
                                    NULLHANDLE, RS_ALL,
                                    &hwndClient) ;

    // subclass the listbox
    WinSendMsg( hwndClient, WM_PASSPROC, MPFROMP( WinSubclassWindow( hwndClient, ClientWndProc)), 0L) ;

    // titlebar menu handle
    hwndTitleMenu = CTRL( hwndFrame, FID_SYSMENU) ;
    // subclass the titlebar menu
    WinSendMsg( hwndTitleMenu, WM_PASSPROC,
                MPFROMP( WinSubclassWindow( hwndTitleMenu, TitleMenuWndProc)), 0L) ;

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
            WHERE where ;

            // get the WC_LISTBOX procedure address
            pfnwp = (PFNWP)mp1 ;

            // client window (listbox)
            where.hwnd = hwnd ;
            // files to search
            strcpy( where.szString, "*.*") ;

            search( &where) ;
        }
            return 0L ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> source)
            {
                case CMDSRC_MENU:
                    switch( COMMANDMSG( &msg) -> cmd)
                    {
                        case MN_INSTALL:
                        {
                            HOBJECT hobj ;
                            LONG i = 0L, lItems ;
                            CHAR szFileName[ 255], szDrive[ 60] ;
                            ULONG ulDrive = 0L ;
 
                            // how many files in the listbox?
                            lItems = (LONG)WinSendMsg( hwnd, LM_QUERYITEMCOUNT, 0L, 0L) ;

                            // create the install folder
                            hobj = WinCreateObject(    "WPFolder",
                                                "Install Folder",
                                                "ICONRESOURCE=301,MIE02",
                                                "<WP_DESKTOP>",
                                                CO_REPLACEIFEXISTS) ;

                            // query the boot disk
                            DosQuerySysInfo(    QSV_BOOT_DRIVE, QSV_BOOT_DRIVE,
                                                (PVOID)&ulDrive, sizeof( ulDrive)) ;
                            // sprepare the string
                            sprintf( szDrive, "%c:\\DESKTOP\\INSTALL FOLDER", ulDrive + 'A' - 1) ;
 
                            while( i <= lItems)
                            {
                                WinSendMsg( hwnd, LM_QUERYITEMTEXT,
                                            MPFROM2SHORT( i++, sizeof( szFileName)),
                                            MPFROMP( szFileName)) ;

                                DosCopy(    szFileName,
                                            szDrive,
                                            DCPY_EXISTING) ;
                            }
                            // show the new folder
                            WinSetObjectData( hobj, "OPEN=DEFAULT") ;
                         }
                            break ;

                        case MN_PRODINFO:
                        {
                            DLGDATA dlgData ;

                            dlgData.hwndOwner = hwnd ;
                            strcpy( dlgData.szWindowTitle, "A WPS install program") ;
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

MRESULT EXPENTRY TitleMenuWndProc(  HWND hwnd,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
    static PFNWP pfnwp ;
    static CHAR *pFiles ;

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
            HWND hwndClient = CTRL( PAPA( hwnd), FID_CLIENT) ;
            SHORT sItems, i = 0 ;
            UCHAR **pFileNames ;
            UCHAR **pTypes = { NULL}, **pTargets = { NULL} ;
            LONG l = 0L, lMove = 0L ;

            // how many files in the listbox
            sItems = (SHORT)WinSendMsg( hwndClient, LM_QUERYITEMCOUNT, 0L, 0L) ;

            // allocate one page of memory
            DosAllocMem( (PPVOID)&pFiles, 4096, PAG_READ | PAG_WRITE | PAG_COMMIT) ;

            // save the pointer
            pFileNames = (UCHAR **)pFiles ;

            // leave room for sItems pointers
            lMove += (LONG)sItems * sizeof( PCHAR) ;

            // read all file names
            while( i < sItems)
            {
                // get the ith file name
                l = (LONG)WinSendMsg(   hwndClient, LM_QUERYITEMTEXT,
                                        MPFROM2SHORT( i, 255),
                                        MPFROMP( pFiles + lMove)) ;

                // store the pointer in the initial position
                *( pFileNames + i) = ( pFiles + lMove) ;
                i++ ;

                // update the offset lMove
                lMove += l + 1 ;
            }

            DrgDragFiles(   hwnd,
                            pFileNames,
                            pTypes,
                            pTargets,
                            sItems,
                            WinLoadPointer( HWND_DESKTOP, NULLHANDLE, RS_ICON),
                            VK_BUTTON2,
                            FALSE,
                            0L) ;
        }
            break ;

        case WM_BUTTON2MOTIONEND:
            break ;

        case WM_ENDDRAG:
            // free the block of memory
            DosFreeMem( pFiles) ;
            break ;

        default:
            break ;
    }
    return (* pfnwp)( hwnd, msg, mp1, mp2) ;
}

