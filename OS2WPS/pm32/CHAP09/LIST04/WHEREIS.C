// WHEREIS.C - Searching files
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
#define THREAD_STACK        8192L



// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
void ShowCurDir( ULONG ulDrive, HWND hwndCurDir) ;
void APIENTRY search( PDATA pData) ;
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

    // load the window template
    hwndFrame = WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP,
                            NULL, NULLHANDLE, 256, NULL) ;

    // initialize all the controls
    WinSendMsg( CLIENT( hwndFrame), WM_COMMAND,
                MPFROMSHORT( ID_STARTUP), 0L) ;
    // position the window
    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        60, 200, 850, 320,
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
    static DATA Data ;
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
            WinSendMsg( PAPA( hwnd), WM_SETICON, MPFROMLONG( (LONG) hptr), 0L) ;

            Data.hwnd = hwnd ;
            *Data.szString = '\0' ;
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
                            *Data.szString = '\0' ;
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
					hwndDir = WinWindowFromID( hwnd, ID_DIR) ;
					hwndEdit = WinWindowFromID( hwnd, ID_EDIT) ;
					hwndSearch = WinWindowFromID( hwnd, ID_SEARCH) ;
					hwndCurDir = WinWindowFromID( hwnd, ID_CURDIR) ;
					hwndDel = WinWindowFromID( hwnd, ID_DEL) ;
					hwndDelAll = WinWindowFromID( hwnd, ID_DELALL) ;
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

                    WinQueryWindowText( hwndEdit, sizeof( szText), szText) ;

                    // skip if there is no file name
                    if( !*szText || !stricmp( szText, Data.szString))
                        break ;

                    // string copied in DATA
                    strcpy( Data.szString, szText) ;

                    // counter zeroed
                    Data.usTot = 0 ;

                    // erase Found
                    WinSetWindowText( hwndFound, "") ;
                    WinEnableWindow( hwndDel, FALSE) ;
                    WinEnableWindow( hwndDelAll, FALSE) ;

                    // delete the listbox content
                    WinSendMsg( hwndDir, LM_DELETEALL, 0L, 0L) ;

                    // activate the search thread
                    rc = DosCreateThread(   &tid,
                                            (PFNTHREAD)search,
                                            (ULONG)&Data,
                                            0L,
                                            THREAD_STACK) ;
                    if( rc)
                        WinAlarm( HWND_DESKTOP, WA_ERROR) ;

                    // disable Search
                    WinEnableWindow( hwndSearch, FALSE) ;

                    // disable drive listbox
                    WinEnableWindow( hwndDrive, FALSE) ;
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
