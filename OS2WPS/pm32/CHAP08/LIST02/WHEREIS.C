// WHEREIS.C - File search
// Listing 08-02

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
#include "whereis.h"

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
void ShowCurDir( SHORT sDrive, HWND hwndCurDir) ;
void search( HWND hwndDir, char *filename) ;
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

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP,
                            NULL,
                            NULLHANDLE, DL_WINDOW,
                            NULL) ;

    WinSendMsg( CLIENT( hwndFrame), WM_COMMAND,
                        MPFROMSHORT( ID_STARTUP), 0L) ;

    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        60, 200, 830, 310,
                        SWP_ACTIVATE | SWP_MOVE | SWP_SIZE) ;

    haccel = WinLoadAccelTable( hab, NULLHANDLE, RS_ACCELTABLE) ;
    WinSetAccelTable( hab, haccel, hwndFrame) ;

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
                hwndDir,
                hwndCurDir,
                hwndEdit,
                hwndSearch,
                hwndDel,
                hwndDelAll,
                hwndFound ;

    switch( msg)
    {
        case WM_CREATE:
        {
            HPOINTER hptr ;

            hptr = WinLoadPointer( HWND_DESKTOP, 0, RS_ICON) ;
            WinSendMsg( PAPA( hwnd), WM_SETICON,
                        MPFROMLONG( (LONG) hptr), 0L) ;
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
                            ULONG ulDriveNum ;
                            ULONG ulDrives ;
                            CHAR szBuffer [ 60] ;
                            SHORT sPos ;

                            sPos = (short)WinSendMsg(   hwndDrive, LM_QUERYSELECTION,
                                                        MPFROMSHORT( LIT_FIRST),
                                                        MPFROMLONG( 0L)) ;
                            WinSendMsg( hwndDrive, LM_QUERYITEMTEXT,
                                        MPFROM2SHORT( sPos, sizeof( szBuffer)),
                                        MPFROMP( szBuffer)) ;

                            // are we acting on the current drive
                            DosQueryCurrentDisk( &ulDriveNum, &ulDrives) ;
                            if( ulDriveNum == (ULONG)*szBuffer - 64)
                                break ;

                            DosSetDefaultDisk( (ULONG)*szBuffer - 64) ;
                            ShowCurDir( *szBuffer - 65, hwndCurDir) ;
                        }
                            break ;

                        default:
                            break ;
                    }
                    break ;

                case ID_DIR:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case LN_ENTER:
                        {
                            CHAR szBuffer[ 60], szString[ 60] ;
                            RESULTCODES rc ;
                            SHORT sPos, sItem ;

                            sPos = (short)WinSendMsg( hwndDir, LM_QUERYSELECTION,
                                        MPFROMSHORT( LIT_FIRST), 0L) ;
                            sItem = (short)WinSendMsg( hwndDir, LM_QUERYITEMTEXT,
                                        MPFROM2SHORT( sPos, sizeof( szBuffer)),
                                        MPFROMP( szBuffer)) ;

                            // skip if not an EXE
                            if( stricmp( ( szBuffer + ( sItem - 3)), "EXE"))
                                break ;

                            DosExecPgm( szString, sizeof( szString),
                                        EXEC_ASYNC,
                                        NULL, NULL,
                                        &rc,
                                        szBuffer) ;
                        }
                            break ;

                        case LN_SELECT:
                        {
                            SHORT sItem ;

                            if( ( sItem = (SHORT)WinSendMsg(    hwndDir, LM_QUERYSELECTION,
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
                    ULONG ulDriveNum, i = 0 ;
                    ULONG ulDrives ;
                    CHAR szBuffer[ 10] ;

                    // retireve control handles
                    hwndDrive = WinWindowFromID( hwnd, ID_DRIVE) ;
                    hwndDir = WinWindowFromID( hwnd, ID_DIR) ;
                    hwndEdit = WinWindowFromID( hwnd, ID_EDIT) ;
                    hwndSearch = WinWindowFromID( hwnd, ID_SEARCH) ;
                    hwndCurDir = WinWindowFromID( hwnd, ID_CURDIR) ;
                    hwndDel = WinWindowFromID( hwnd, ID_DEL) ;
                    hwndDelAll = WinWindowFromID( hwnd, ID_DELALL) ;
                    hwndFound = WinWindowFromID( hwnd, ID_FOUND) ;

                    DosQueryCurrentDisk( &ulDriveNum, &ulDrives) ;
                    while( i < 26 && ulDrives)
                    {
                        if( ulDrives & 1)
                        {
                            sprintf( szBuffer, "%c:", 65 + i ) ;
                            WinSendMsg( hwndDrive, LM_INSERTITEM,
                                MPFROMSHORT( LIT_SORTASCENDING),
                                MPFROMP( (PSZ)szBuffer)) ;
                        }
                        ulDrives >>= 1 ;
                        i++ ;
                    }
                    sprintf( szBuffer, "%c:", ulDriveNum + 64) ;
                    WinSetWindowText( hwndDrive, (PSZ)szBuffer) ;
                    ShowCurDir( ulDriveNum - 1, hwndCurDir) ;
                    WinSetFocus( HWND_DESKTOP, hwndEdit) ;
                }
                    break ;

                case ID_SEARCH:
                {
                    CHAR szBuffer[ 60], szString[ 60] ;
                    HPOINTER hptr ;
                    SHORT sPos ;

                    WinQueryWindowText( hwndEdit, sizeof( szBuffer), szBuffer) ;

                    // let's skip if no file is available
                    if( !*szBuffer)
                        break ;

                    // disable the Search button
                    WinEnableWindow( CTRL( hwnd, ID_SEARCH), FALSE) ;

                    // clear Found
                    WinSetWindowText( hwndFound, "") ;
                    WinEnableWindow( hwndDel, FALSE) ;
                    WinEnableWindow( hwndDelAll, FALSE) ;

                    // empty listbox
                    WinSendMsg( hwndDir, LM_DELETEALL, 0L, 0L) ;

                    // show the clock pointer
                    hptr = WinQuerySysPointer( HWND_DESKTOP, SPTR_WAIT, TRUE) ;
                    WinSetPointer( HWND_DESKTOP, hptr) ;

                    // start searching
                    search( hwnd, szBuffer) ;

                    // arrow pointer
                    hptr = WinQuerySysPointer( HWND_DESKTOP, SPTR_ARROW, TRUE) ;
                    WinSetPointer( HWND_DESKTOP, hptr) ;

                    // show found files
                    sPos = (short)WinSendMsg( hwndDir, LM_QUERYITEMCOUNT, 0L, 0L) ;
                    sprintf( szString, "%d", sPos) ;
                    WinSetWindowText( hwndFound, szString) ;

                    // beep
                    WinAlarm( HWND_DESKTOP, WA_WARNING) ;

                    // enable the Search button
                    WinEnableWindow( CTRL( hwnd, ID_SEARCH), TRUE) ;
                }
                    break ;

                case ID_CANCEL:
                    WinSendMsg( hwndDir, LM_DELETEALL, 0L, 0L) ;
                    WinSetWindowText( hwndEdit, NULL) ;
                    WinSetWindowText( hwndFound, "") ;
                    WinEnableWindow( hwndSearch, FALSE) ;
                    WinEnableWindow( hwndDel, FALSE) ;
                    WinEnableWindow( hwndDelAll, FALSE) ;
                    WinSetFocus( HWND_DESKTOP, hwndEdit) ;
                    break ;

                case ID_QUIT:
                    WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
                    break ;

                case ID_DELALL:
                {
                    SHORT sPos, sItem ;

                    sItem = (short)WinSendMsg( hwndDir, LM_QUERYITEMCOUNT, 0L, 0L) ;
                    for( sPos = 0; sPos < sItem; sPos++)
                        WinSendMsg( hwndDir, LM_SELECTITEM,
                                    MPFROMSHORT( sPos),
                                    MPFROMSHORT( TRUE)) ;
                }

                case ID_DEL:
                {
                    SHORT sPos, sItem ;
                    CHAR szBuffer[ 60] ;
                    APIRET apiret ;
                    FILESTATUS3 fs3 ;

                    WinSendMsg( hwndDir, LM_SETTOPINDEX, MPFROMSHORT( 0), 0L) ;

                    if( !WinDlgBox( HWND_DESKTOP, hwnd,
                                    YesNoProc,
                                    NULLHANDLE, DL_CONFIRM,
                                    NULL))
                        break ;

                    sItem = (SHORT)WinSendMsg( hwndDir, LM_QUERYITEMCOUNT, 0L, 0L) ;

                    while( ( sPos = (SHORT)WinSendMsg( hwndDir, LM_QUERYSELECTION,
                                                        MPFROMSHORT( LIT_FIRST),
                                                        0L)) != LIT_NONE)
                    {
                        WinSendMsg( hwndDir, LM_QUERYITEMTEXT,
                                    MPFROM2SHORT( sPos, sizeof( szBuffer)),
                                    MPFROMP( szBuffer)) ;

                        DosQueryPathInfo( szBuffer, FIL_STANDARD, &fs3, sizeof( fs3)) ;
                        if( fs3.attrFile & ( FILE_NORMAL | FILE_ARCHIVED))
                        {
                            apiret = DosDelete( szBuffer) ;
                            sItem = (short)WinSendMsg(  hwndDir, LM_DELETEITEM,
                                                        MPFROMSHORT( sPos), 0L) ;
                            sprintf( szBuffer, "%d", sItem) ;
                            WinSetWindowText( hwndFound, szBuffer) ;
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

void ShowCurDir( SHORT sDrive, HWND hwndCurDir)
{
    CHAR szPath[ 100] ;
    ULONG cbSize = sizeof( szPath) ;

    *szPath = (CHAR)sDrive + 65 ;
    *(szPath + 1) = ':' ;
    *(szPath + 2) = '\\' ;

    if( DosQueryCurrentDir( sDrive + 1, szPath + 3, &cbSize) == ERROR_NOT_READY)
    {
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        strcpy( szPath, "NOT VALID DRIVE") ;
        // disable Search
        WinEnableWindow( SON( PAPA( hwndCurDir), ID_SEARCH), FALSE) ;
        WinEnableWindow( SON( PAPA( hwndCurDir), ID_EDIT), FALSE) ;
    }
    else
    {
        WinEnableWindow( SON( PAPA( hwndCurDir), ID_SEARCH), TRUE) ;
        WinEnableWindow( SON( PAPA( hwndCurDir), ID_EDIT), TRUE) ;
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

            WinSetPresParam(    SON( hwnd, DID_OK), PP_BACKGROUNDCOLORINDEX,
                                sizeof clr, &clr) ;

            clr = CLR_RED ;
            WinSetPresParam(    SON( hwnd, DID_CANCEL), PP_BACKGROUNDCOLORINDEX,
                                sizeof clr, &clr) ;
        }
        break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case DID_OK:
                case DID_CANCEL:
                    WinDismissDlg( hwnd, 2 - COMMANDMSG( &msg) -> cmd) ;
                    return (MRESULT) (2 - COMMANDMSG( &msg) -> cmd);
            }
            break ;

        default:
            break ;
    }
    return WinDefDlgProc( hwnd, msg, mp1, mp2) ;
}
