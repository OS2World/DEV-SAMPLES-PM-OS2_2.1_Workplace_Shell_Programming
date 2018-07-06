// OLDOPEN.C - Open box
// Listing 08-03

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_DOS
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "oldopen.h"
#include "dialog.h"

// macros
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define OWNER( x)   WinQueryWindow( x, QW_OWNER)
#define MENU( x)    WinWindowFromID( x, FID_MENU)
#define SYS( x)     WinQuerySysValue( HWND_DESKTOP, x)
 
#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX


// function prototypes
MRESULT EXPENTRY OpenDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ProdInfoDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY Open( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
SHORT EditParser( CHAR *pcOut, CHAR *pcIn) ;
VOID FillDirListBox( HWND hwnd, CHAR *pcCurrentPath) ;
LONG LoseBrackets( char *buffer, char *szBuffer) ;
int main( void) ;


int main( void)
{
    CHAR szClassName[ 10] ;
    CHAR szWindowTitle[ 30] ;
    HWND hwndFrame, hwndClient ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load classname & window title
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
                                    NULLHANDLE, RS_ALL,
                                    &hwndClient) ;

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
    static DATA Data ;

    switch( msg)
    {
        case WM_CREATE:
        {
            Data.hwndOwner = hwnd ;
            // disable HARDERRORS
            DosError( FERR_DISABLEHARDERR) ;
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

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_OPEN:
                {
                    CHAR szString[ 100] ;

                    if( WinDlgBox(  HWND_DESKTOP, hwnd,
                                    OpenDlgProc,
                                    NULLHANDLE, DL_OPEN,
                                    (PVOID)&Data))
                    {
                        WinSendMsg( MENU( PAPA( hwnd)), MM_SETITEMATTR,
                                    MPFROM2SHORT( MN_SAVE, TRUE),
                                    MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED )) ;

                        WinSendMsg( MENU( PAPA( hwnd)), MM_SETITEMATTR,
                                    MPFROM2SHORT( MN_SAVEAS, TRUE ),
                                    MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED )) ;
                        sprintf( szString, "Open Box - %s", Data.szCurrentPath) ;
                        WinSetWindowText( PAPA( hwnd), szString) ;
                    }
                }
                    break ;

                case MN_PRODINFO:
                    WinDlgBox( HWND_DESKTOP, hwnd, ProdInfoDlgProc, NULLHANDLE, DL_PRODINFO, &hwnd);
                    break;

                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                    break;

                default:
                    break ;
            }
            break ;

        case WM_DESTROY:
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY ProdInfoDlgProc(   HWND hwnd,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
    switch( msg)
    {
        case WM_INITDLG:
        {
            HWND hwndOwner ;

            hwndOwner = *((PHWND)PVOIDFROMMP( mp2)) ;
            break ;
        }
            break ;

        case WM_BUTTON2DBLCLK:
        {
            static RECTL rc ;
            static SWP swp ;
            static BOOL fState ;
            LONG cy = SYS( SV_CYTITLEBAR) + SYS( SV_CYDLGFRAME) * 2 + 2 ;

            if( !fState)
            {
                // query window rect
                WinQueryWindowRect( hwnd, &rc) ;

                WinQueryWindowPos( hwnd, &swp) ;

                WinSetWindowPos( hwnd, HWND_TOP,
                                    swp.x, swp.y + swp.cy - cy,
                                    swp.cx, cy,
                                    SWP_MOVE | SWP_SIZE) ;
                fState = TRUE ;
            }
            else
            {
                SWP newswp ;

                fState = FALSE ;
                WinQueryWindowPos( hwnd, &newswp) ;
                WinSetWindowPos(    hwnd, HWND_TOP,
                                    newswp.x, newswp.y - swp.cy + cy,
                                    swp.cx, swp.cy,
                                    SWP_MOVE | SWP_SIZE) ;
            }
         }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case DID_OK:
                case DID_CANCEL:
                    WinDismissDlg( hwnd, TRUE) ;
                    break ;

                default:
                    break ;
            }
            break ;

        default:
            break ;
    }
    return WinDefDlgProc( hwnd, msg, mp1, mp2) ;
}

VOID FillDirListBox( HWND hwnd, CHAR *pcCurrentPath)
{
    CHAR szDrive[] = "[ :]", buffer[ 30] ;
    FILEFINDBUF3 findbuf ;
    HDIR hDir = HDIR_CREATE ;
    ULONG ulDrive ;
    ULONG ulDriveNum, usCurPathLen, usSearchCount = 1 ;
    ULONG ulDriveMap ;

    // current drive number
    DosQueryCurrentDisk( &ulDriveNum, &ulDriveMap) ;
    pcCurrentPath[ 0] = (CHAR) ulDriveNum + (CHAR)64 ;
    pcCurrentPath[ 1] = ':' ;
    pcCurrentPath[ 2] = '\\' ;
    usCurPathLen = 64 ;

    // path and current directory
    DosQueryCurrentDir( 0, pcCurrentPath + 3, &usCurPathLen) ;

    // show the pathname
    WinSetDlgItemText( hwnd, DL_PATH, pcCurrentPath) ;

    // empty DL_DIRLIST
    WinSendDlgItemMsg( hwnd, DL_DIRLIST, LM_DELETEALL, NULL, NULL) ;

    // search a file
    DosFindFirst(   "*.*",
                    &hDir,
                    FILE_DIRECTORY,
                    &findbuf,
                    sizeof findbuf,
                    &usSearchCount, 1L) ;

    do
    {
        if( findbuf.attrFile & FILE_DIRECTORY && (findbuf.achName[ 0] != '.' || findbuf.achName[ 1]))
        {
            sprintf( buffer, "[%s]", findbuf.achName) ;
            WinSendDlgItemMsg(  hwnd, DL_DIRLIST,
                                LM_INSERTITEM,
                                MPFROM2SHORT( LIT_SORTASCENDING, 0),
                                MPFROMP( buffer)) ;
        }
    } while( !DosFindNext( hDir, &findbuf, sizeof findbuf, &usSearchCount)) ;

    DosQueryCurrentDisk( &ulDrive, &ulDriveMap) ;
    ulDrive = 0 ;

    while( ulDriveMap)
    {
        if( ulDriveMap & 1L)
        {
            szDrive[ 1] = (CHAR) (65 + ulDrive) ;
            WinSendDlgItemMsg(  hwnd, DL_DIRLIST,
                                LM_INSERTITEM,
                                MPFROM2SHORT( LIT_END | LIT_SORTASCENDING, 0),
                                MPFROMP( szDrive)) ;
        }
        ulDrive++ ;
        ulDriveMap >>= 1 ;
    }
}

VOID FillFileListBox( HWND hwnd)
{
    FILEFINDBUF3 findbuf ;
    HDIR hDir = HDIR_CREATE ;
    ULONG usSearchCount = 1 ;

    WinSendDlgItemMsg( hwnd, DL_FILELIST, LM_DELETEALL,    NULL, NULL) ;

        DosFindFirst(   "*.*",
                        &hDir,
                        FILE_SYSTEM | FILE_HIDDEN | FILE_READONLY,
                        &findbuf, sizeof findbuf, &usSearchCount, 1L) ;

    do
    {
        WinSendDlgItemMsg(  hwnd, DL_FILELIST,
                            LM_INSERTITEM,
                            MPFROM2SHORT( LIT_SORTASCENDING, 0),
                            MPFROMP( findbuf.achName)) ;
    } while( !DosFindNext( hDir, &findbuf, sizeof findbuf, &usSearchCount)) ;

    WinSetFocus( HWND_DESKTOP, WinWindowFromID( hwnd, DL_FILEEDIT)) ;
}

MRESULT EXPENTRY OpenDlgProc(   HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    CHAR szBuffer[ 80], szString[ 80], buffer[ 40], szTmp[ 40] ;
    SHORT sSelect ;
    SHORT sPos, sLen, sItem ;
    static SHORT sIndex = -1 ;
    static BOOL fSelection = FALSE ;
    static PDATA pData ;

    switch( msg)
    {
        case WM_INITDLG:
        {
            RECTL rc1, rc2 ;
            POINTL ptl ;
            HWND hwndOwner ;

            pData = (PDATA)mp2 ;
            hwndOwner = pData -> hwndOwner ;

            WinQueryWindowRect( hwndOwner, &rc1) ;
            WinQueryWindowRect( hwnd, &rc2) ;

            ptl.x = (( rc1.xRight - rc1.xLeft) - ( rc2.xRight - rc2.xLeft)) / 2 ;
            ptl.y = (( rc1.yTop - rc1.yBottom) - ( rc2.yTop - rc2.yBottom)) / 2 ;

            // place the dialog in the center
            if( ptl.x > 0 && ptl.y > 0)
            {
                WinMapWindowPoints( hwndOwner, HWND_DESKTOP,
                                    &ptl, 1) ;

                WinSetWindowPos(    hwnd, HWND_TOP,
                                    ptl.x, ptl.y,
                                    0, 0, SWP_MOVE) ;
            }
        }
            // fill the listbox drive and directory
            FillDirListBox( hwnd, pData -> szCurrentPath) ;

            // fill in the listbox files
            FillFileListBox( hwnd) ;

            WinSendDlgItemMsg(  hwnd, DL_FILEEDIT,
                                EM_SETTEXTLIMIT,
                                MPFROM2SHORT ( 80, 0), NULL) ;
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))       // Control ID
            {
                case DL_DIRLIST:
                    // notification code
                    switch( SHORT2FROMMP( mp1))
                    {
                        case LN_ENTER:
                            sSelect = (USHORT)WinSendDlgItemMsg(    hwnd, DL_DIRLIST,
                                                                    LM_QUERYSELECTION, 0L, 0L) ;

                            WinSendDlgItemMsg(  hwnd, DL_DIRLIST,
                                                LM_QUERYITEMTEXT,
                                                MPFROM2SHORT( sSelect, sizeof szBuffer),
                                                MPFROMP( szBuffer)) ;

                            LoseBrackets( buffer, szBuffer) ;
                            if( strchr( buffer, ':'))
                                DosSelectDisk( buffer[ 0] - '@') ;
                            else
                                DosSetCurrentDir( buffer) ;
                            FillDirListBox( hwnd, pData -> szCurrentPath) ;
                            FillFileListBox( hwnd) ;

                            // erase content in entryfield
                            WinSetDlgItemText( hwnd, DL_FILEEDIT, "") ;
                            sIndex = LIT_FIRST ;
                            return 0 ;

                        case LN_SELECT:
                            fSelection = TRUE ;
                            break ;
                    }
                    break ;

                case DL_FILELIST:
                    sSelect = (USHORT)WinSendDlgItemMsg(    hwnd, DL_FILELIST,
                                                            LM_QUERYSELECTION, 0L, 0L) ;

                    WinSendDlgItemMsg(  hwnd, DL_FILELIST,
                                        LM_QUERYITEMTEXT,
                                        MPFROM2SHORT( sSelect, sizeof szBuffer),
                                        MPFROMP( szBuffer)) ;

                    // notification code
                    switch (SHORT2FROMMP( mp1))
                    {
                        case LN_SELECT:
                            fSelection = TRUE ;
                            WinSetDlgItemText( hwnd, DL_FILEEDIT, szBuffer) ;
                            sIndex = LIT_FIRST ;
                            return 0 ;

                        case LN_ENTER:
                            EditParser( pData -> szCurrentPath, szBuffer) ;
                            WinDismissDlg( hwnd, TRUE) ;
                            return 0 ;
                    }
                    break ;

                case DL_FILEEDIT:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case EN_CHANGE:
                            // skip if an item has been selected
                            if( fSelection)
                            {
                                fSelection = !fSelection ;
                                break ;
                            }

                            WinQueryWindowText( HWNDFROMMP( mp2), sizeof buffer, buffer) ;
                            if( buffer[ 0])
                            {
                                strupr( buffer) ;
                                sItem = (SHORT)WinSendDlgItemMsg(   hwnd,
                                                                    DL_FILELIST,
                                                                    LM_QUERYITEMCOUNT, 0L, 0L) ;

                                while( TRUE)
                                {
                                    sPos = (SHORT)WinSendDlgItemMsg(    hwnd, DL_FILELIST,
                                                                        LM_SEARCHSTRING,
                                                                        MPFROM2SHORT( LSS_SUBSTRING, sIndex),
                                                                        MPFROMP( (PSZ) buffer)) ;

                                    // skip over
                                    if( sPos < 0 || (sPos >= 0 && sPos < sIndex))
                                        break ;

                                    // get the text
                                    sLen = (SHORT)WinSendDlgItemMsg(    hwnd,
                                                                        DL_FILELIST,
                                                                        LM_QUERYITEMTEXTLENGTH,
                                                                        MPFROMSHORT( sPos), 0L) ;
                                    WinSendDlgItemMsg(  hwnd, DL_FILELIST,
                                                        LM_QUERYITEMTEXT,
                                                        MPFROM2SHORT( sPos, sLen + 1),
                                                        MPFROMP( szTmp)) ;

                                    if( strstr( szTmp, buffer) == szTmp)
                                    {
                                        WinSendDlgItemMsg(  hwnd, DL_FILELIST,
                                                            LM_SETTOPINDEX,
                                                            MPFROMSHORT( sPos), 0L) ;
                                        return 0 ;
                                    }
                                    // is the first char in the entryfield bigger
                                    // than text in the string?
                                    if( szTmp[ 0] > buffer[ 0])
                                        break ;

                                    // set the new starting position
                                    sIndex = sPos ;
                                }
                            }
                            else
                            {
                                sIndex = LIT_FIRST ;
                            }
                            break ;

                        default:
                            break ;
                    }
                    break ;
                }
                break ;

            case WM_COMMAND:
                switch( COMMANDMSG( &msg)->cmd)
                {
                    case DID_OK:
                        WinQueryDlgItemText(    hwnd, DL_FILEEDIT,
                                                sizeof szBuffer, szBuffer) ;
                        sSelect = EditParser( szString, szBuffer) ;
                        switch( sSelect)
                        {
                            case 0:
                                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                                FillDirListBox( hwnd, szString) ;
                                FillFileListBox( hwnd) ;
                                return 0 ;

                            case 1:
                                FillDirListBox( hwnd, szString) ;
                                FillFileListBox( hwnd) ;
                                WinSetDlgItemText( hwnd, DL_FILEEDIT, "") ;
                                return 0 ;

                            case 2:
                                strcpy( pData -> szCurrentPath, szString) ;
                                // dismiss the dialog window
                                WinDismissDlg (hwnd, TRUE) ;
                                return 0 ;
                        }
                        break ;

                case DID_CANCEL:
                    WinDismissDlg (hwnd, FALSE) ;
                    return 0 ;
            }
            break ;
    }
    return WinDefDlgProc( hwnd, msg, mp1, mp2) ;
}

SHORT EditParser( CHAR *pcOut, CHAR *pcIn)
{
    CHAR   *pcLastSlash, *pcFileOnly, *s1 ;
    ULONG  ulDriveMap ;
    ULONG ulDriveNum, ulDirLen = 64 ;

    strupr( pcIn) ;

    // return 1 if the string is empty
    if( *pcIn == '\0')
        return 1 ;

    // which is the current drive?
    if( pcIn + 1 == ( s1 = strchr( pcIn, ':')))
    {
        if( DosSelectDisk( pcIn[ 0] - '@'))
            return 0 ;

        // move the pointer after the colon
        pcIn += 2 ;
    }
    else
        if( s1)
            return 0 ;

    DosQueryCurrentDisk( &ulDriveNum, &ulDriveMap) ;

    *pcOut++ = (CHAR)( ulDriveNum + 64) ;
    *pcOut++ = ':' ;
    *pcOut++ = '\\' ;

    // if what is left is empty lets return 1
    if( *pcIn == '\0')
        return 1 ;

    // looking for every backslash
    if( !( pcLastSlash = strchr( pcIn, '\\')))
    {
        if( !DosSetCurrentDir( pcIn))
            return 1 ;

            // current dir and pathname
            DosQueryCurrentDir( 0, pcOut, &ulDirLen) ;

            if( strlen( pcIn) > 12)
                return 0 ;

            if( *( pcOut + strlen( pcOut) - 1) != '\\')
                strcat( pcOut++, "\\") ;

            strcat( pcOut, pcIn) ;
                return 2 ;
    }

    // lets move to the root
    if( pcIn == pcLastSlash)
    {
        DosSetCurrentDir( pcIn) ;

        if ( *( pcIn + 1) == '\0' || !strchr( pcIn, '.'))
            return 1 ;

        strcpy( pcOut, pcIn + 1) ;
            return 2 ;
    }

    // select a new directory
    *pcLastSlash = '\0' ;

    if( DosSetCurrentDir( pcIn))
        return 0 ;

    DosQueryCurrentDir( 0, pcOut, &ulDirLen) ;

    // adding the file name
    pcFileOnly = pcLastSlash + 1 ;

    if( *pcFileOnly == '\0')
        return 1 ;

    if( strlen (pcFileOnly) > 12)
        return 0 ;

    if( *(pcOut + strlen( pcOut) - 1) != '\\')
        strcat( pcOut++, "\\") ;

    strcat( pcOut, pcFileOnly) ;
    return 2 ;
}

LONG LoseBrackets( register char *s1, register char *s2)
{
    LONG i = 0 ;
    while( *s2)
    {
        if( *s2 != '[' && *s2 != ']')
        {
            *s1++ = *s2 ;
            i++ ;
        }
        s2++ ;
    }
    *s1 = '\0' ;
    return i ;
}
