// MENU.C - Menu maker
// Listing 09-01

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL
#define INCL_DOSFILEMGR

#include <os2.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "menu.h"
#include "dialog.h"

// definitions
#define MAXROW          2
#define MAXCOL          2
#define ID_VALUESET     100

#define MENUID          90

#define STARTX          120
#define STARTY          50

#define FILENAME        20

#define OFFSET          10

#define ID_EDIT         100

#define CX              100
#define CY              20

#define BT_EDIT         0x00010001
#define BT_CLOSE        0x00020001
#define BT_LOAD         0x00010002
#define BT_SAVE         0x00020002

// macros
#define CTRL( x, y)     WinWindowFromID( x, y)
#define HAB( x)         WinQueryAnchorBlock( x)
#define SYS( x)         WinQuerySysValue( HWND_DESKTOP, x)
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define MENU( x)        WinWindowFromID( PAPA(x), FID_MENU)
#define MAX( a, b)      ( (a) < (b)) ? (b) : (a)

#define FCF_WPS         FCF_STANDARD


// function prototypes
int main(void) ;
void Sub1with2( char *s) ;
char *strrep(  SHORT sRep) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY MenuDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY Project( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
BOOL StringParser( char *szString) ;
char *stristr( register char *s1, register char *s2) ;
BOOL MaskHandler( HWND, BOOL) ;
BOOL Save( HWND hwnd, char * szProject) ;
void RemoveBlanks( register char *s) ;
BOOL CheckSubMenu( HWND hwndSubMenu,
                SHORT sItemNumber,
                HFILE hFile1,
                HFILE hFile2,
                HFILE hFile3,
                BOOL fNewTop,
                SHORT sTopLevel) ;
SHORT CalcMenuItem( SHORT sMenuCount, HWND hwndSubMenu, HWND hmenu, BOOL fReset) ;
char *Accel( register char *t, register char *s) ;

int main( void)
{
    HAB hab ;
    HWND hwndFrame ;
    CHAR szClassName[] = "menu" ;
    CHAR szWindowTitle[] = "Menu Maker" ;
    HMQ hmq ;
    HWND hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    WS_CLIPCHILDREN,
                                    NULLHANDLE, RS_ICON,
                                    &hwndClient) ;

    // show maximized window
    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        0, 0, 0, 0,
                        SWP_MAXIMIZE | SWP_ACTIVATE | SWP_SHOW) ;

    while( WinGetMsg( hab, &qmsg, 0, 0, 0))
        WinDispatchMsg( hab, &qmsg) ;

    WinDestroyWindow( hwndFrame) ;
    WinDestroyMsgQueue( hmq) ;
    WinTerminate( hab) ;
    return 0 ;
}

MRESULT EXPENTRY ClientWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{

    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CREATE:
        {
            HWND hwndValueset ;
            VSCDATA vscd ;
            long i, j, k = 0 ;
            CHAR *szButton[] = { "Edit...", "Close", "Load", "Save..."} ;

            vscd.cbSize = sizeof vscd ;
            vscd.usRowCount = MAXROW ;
            vscd.usColumnCount = MAXCOL ;

            hwndValueset = WinCreateWindow( hwnd, WC_VALUESET,
                                            NULL,
                                            VS_TEXT | VS_BORDER | VS_ITEMBORDER | WS_VISIBLE,
                                            100, 100, 240, 120,
                                            hwnd, HWND_TOP,
                                            ID_VALUESET,
                                            (PVOID)&vscd, NULL) ;

            for( i = 1; i <= MAXCOL; i++)
            for( j = 1; j <= MAXROW; j++)
            {
                // set attributes
                WinSendMsg( hwndValueset, VM_SETITEMATTR, MPFROM2SHORT( i, j), MPFROM2SHORT( VIA_TEXT, TRUE)) ;

                // insert text
                WinSendMsg( hwndValueset, VM_SETITEM,
                            MPFROM2SHORT( i, j),
                            MPFROMP( szButton[ k++])) ;
            }
        }
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case ID_VALUESET:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case VN_ENTER:  // double click or Enter
                        {
                            long lVal ;
                            HWND hwndVal = CTRL( hwnd, ID_VALUESET) ;
 
                            lVal = (long)WinSendMsg( hwndVal, VM_QUERYSELECTEDITEM, 0L, 0L) ;
                            switch( lVal)
                            {
                                case BT_EDIT:
                                {
                                    // show dialog window
                                    WinLoadDlg( HWND_DESKTOP, hwnd, MenuDlgProc,
                                                NULLHANDLE, 256, &hwnd) ;
                                }
                                    break ;

                                case BT_CLOSE:
                                    WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
                                    break ;

                                case BT_LOAD:
                                {
                                    // load
                                    WinDestroyWindow( MENU( hwnd)) ;
                                    WinLoadMenu( PAPA( hwnd), NULLHANDLE, MN_MENU) ;
                                    WinSendMsg( PAPA( hwnd), WM_UPDATEFRAME, MPFROMSHORT( FCF_MENU), 0L) ;
                                }
                                    break ;

                                case BT_SAVE:
                                {
                                    char szProject[ FILENAME] ;

                                    // save
                                    WinDlgBox(  HWND_DESKTOP, hwnd,
                                                Project,
                                                NULLHANDLE, ID_PROJECT,
                                                (PVOID)szProject) ;

                                    Save( hwnd, szProject) ;
                                }
                                    break ;
                            }
                        }
                            break ;
                    }
                        break ;
                }
                    break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY MenuDlgProc(   HWND hdlg,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    SHORT sID ;
    SHORT sPos ;
    CHAR buffer[ 80], buff[ 40] ;
    MENUITEM mi ;

    static SHORT sState ;
    static HWND hwndSubMenu, hwndSubSub ;
    static SHORT sMenuCount ;
    static BOOL fSubMenu = FALSE ;
    static BOOL fExtCmd = FALSE ;
    static SHORT sCount[ 12] ;
    static USHORT usAttribute ;
    static HWND hmenu, hwndp ;

    switch( msg)
    {
        case WM_INITDLG:
            // get the client window handle
            hwndp = *((PHWND)PVOIDFROMMP( mp2)) ;

            // determine the menu handle
            hmenu = MENU( hwndp) ;

            // set item limit to 12
            WinSendMsg( CTRL( hdlg, DL_ITEM), EM_SETTEXTLIMIT,
                        MPFROMSHORT( 12), 0L) ;

            // activate the top level option
            WinSendMsg( CTRL( hdlg, DL_TOP), BM_SETCHECK,
                        MPFROMSHORT( TRUE), 0L) ;

            // setting sState
            sState = DL_TOP ;

            // disable DL_EXTCMD & DL_ACC
            WinEnableWindow( CTRL( hdlg, DL_ACC), FALSE) ;
            WinEnableWindow( CTRL( hdlg, DL_ACCTEXT), FALSE) ;

            // analyze previous menus
            while( WinSendMsg(hmenu, MM_QUERYITEM,
                                MPFROM2SHORT( MN_START + sMenuCount * TOTITEM, FALSE),
                                (MPARAM)&mi))
            {
                // get the text in the top level
                WinSendMsg( hmenu, MM_QUERYITEMTEXT,
                            MPFROM2SHORT( MN_START + sMenuCount * TOTITEM, 15),
                            MPFROMP( buffer)) ;

                StringParser( buffer) ;

                // fill in the listbox
                WinSendMsg( CTRL( hdlg, DL_LIST), LM_INSERTITEM,
                            MPFROMSHORT( LIT_END), MPFROMP( buffer)) ;

                // count the menuitems
                if( mi.hwndSubMenu)
                {
                    sCount[ sMenuCount] = CalcMenuItem( sMenuCount,
                                                        mi.hwndSubMenu,
                                                        hmenu, TRUE) ;
                }

                // increase counter
                sMenuCount++ ;
            }

            if( sMenuCount)
                // select the first top level in the listbox
                WinSendMsg( CTRL( hdlg, DL_LIST), LM_SELECTITEM,
                            MPFROMSHORT( 0), MPFROMSHORT( TRUE)) ;

            WinSetFocus( HWND_DESKTOP, CTRL( hdlg, DL_TOP)) ;

            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case DL_TOP:
                    sState = DL_TOP ;

                    MaskHandler( hdlg, FALSE) ;
                    break ;

                case DL_DROP:
                    sState = DL_DROP ;

                    MaskHandler( hdlg, TRUE) ;
                    break ;

                case DL_SUBMENU:
                    sState = DL_SUBMENU ;

                    // disable DL_ITEM
                    if( fSubMenu)
                    {
                        WinEnableWindow( CTRL( hdlg, DL_ITEM), FALSE) ;
                        WinEnableWindow( CTRL( hdlg, DL_ITEMTEXT), FALSE) ;
                    }

                    MaskHandler( hdlg, FALSE) ;
                    break ;

                case DL_SEPARATOR:
                    sState = DL_SEPARATOR ;

                    MaskHandler( hdlg, FALSE) ;
                    WinSetWindowText( CTRL( hdlg, DL_ITEM), (PSZ)"SEPARATOR") ;
                    break ;

                case DL_CHECKED:
                    if(!usAttribute)
                        usAttribute = MIA_CHECKED ;
                    break ;

                case DL_DISABLED:
                    if( !usAttribute)
                        usAttribute = MIA_DISABLED ;
                    break ;

                case DL_EXTCMD:
                    fExtCmd = TRUE ;
                    break ;
            }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case DL_ENTER:
                    // check if there is a menuitem
                    if(WinQueryWindowText( CTRL( hdlg, DL_ITEM),
                                            sizeof buffer,
                                            buffer) || fSubMenu)
                    {
                        // there is some text in the entryfield
                        switch( sState)
                        {
                            case DL_DROP:
                                // determine the selected top level
                                sPos = (SHORT)WinSendMsg( CTRL( hdlg, DL_LIST),
                                            LM_QUERYSELECTION,
                                            MPFROMSHORT( LIT_FIRST), 0L) ;

                                if( !fSubMenu)
                                {
                                    // query the menuitem
                                    WinSendMsg( hmenu, MM_QUERYITEM,
                                                MPFROM2SHORT( MN_START + sPos * TOTITEM, FALSE),
                                                (MPARAM)&mi) ;

                                    // is there a drop down?
                                    if( !mi.hwndSubMenu)
                                    {
                                        CHAR szString[ 40] ;

                                        // create a new window of class WC_MENU
                                        mi.hwndSubMenu = WinCreateWindow( HWND_OBJECT,
                                                                        WC_MENU,
                                                                        NULL,
                                                                        0L,
                                                                        0, 0, 0, 0,
                                                                        hmenu,
                                                                        HWND_TOP,
                                                                        0, NULL, NULL) ;

                                        mi.afStyle = MIS_TEXT | MIS_SUBMENU ;

                                        WinSendMsg( hmenu, MM_QUERYITEMTEXT,
                                                    MPFROM2SHORT( MN_START + sPos * TOTITEM, sizeof szString),
                                                    MPFROMP( szString)) ;

                                        // set handle of drop down menu
                                        // in the top level structure
                                        WinSendMsg( hmenu, MM_SETITEM,
                                                    MPFROM2SHORT( 0, FALSE),
                                                    (MPARAM)&mi) ;

                                        WinSendMsg( hmenu, MM_SETITEMTEXT,
                                                    MPFROMSHORT( MN_START + sPos * TOTITEM),
                                                    MPFROMP( szString)) ;
                                    }
                                    // store the drop down handle
                                    hwndSubMenu = mi.hwndSubMenu ;
                                }

                                // set the new ID
                                sID = MN_START + sPos * TOTITEM + 1 + sCount[ sPos] ;

                                // increase the counter of total menuitem
                                sCount[ sPos]++ ;

                                // set the structure for the updated drop down
                                mi.iPosition = MIT_END ;
                                mi.afStyle = MIS_TEXT ;
                                mi.afAttribute = usAttribute ;
                                mi.id = sID ;
                                mi.hItem = (ULONG)NULL ;
                                mi.hwndSubMenu = (HWND)NULL ;

                                // add the ellipsis
                                if( fExtCmd)
                                    strcat( buffer, "...") ;

                                // check the accelerators
                                if( WinQueryWindowText( CTRL( hdlg, DL_ACC),
                                                        sizeof buff, buff))
                                {
                                    strcat( buffer, "\t") ;
                                    strcat( buffer, buff) ;
                                }

                                // add the drop down to the top level
                                WinSendMsg( (fSubMenu) ? hwndSubSub : hwndSubMenu,
                                            MM_INSERTITEM,
                                            (MPARAM)&mi, MPFROMP( buffer)) ;

                                WinSendMsg( PAPA( hwndp), WM_UPDATEFRAME,
                                            MPFROMSHORT( FCF_MENU), 0L) ;

                                if( fSubMenu)
                                {
                                    // enable DL_SEPARATOR DL_TERMINATE DL_SUBMENU & ESC
                                    WinEnableWindow( CTRL( hdlg, DL_TERMINATE), TRUE) ;
                                    WinEnableWindow( CTRL( hdlg, DID_CANCEL), TRUE) ;
                                    WinEnableWindow( CTRL( hdlg, DL_SUBMENU), TRUE) ;
                                    WinEnableWindow( CTRL( hdlg, DL_SEPARATOR), TRUE) ;

                                    // change text in DL_SUBMENU
                                    WinSetWindowText( CTRL( hdlg, DL_SUBMENU), (PSZ)"End submenu") ;
                                }
                                break ;

                        case DL_TOP:
                            if( fSubMenu)
                            {
                                fSubMenu = FALSE ;
                            }
                            strcpy( buff, buffer) ;

                            StringParser( buff) ;

                            // insert a string in the listbox
                            sPos = (SHORT)WinSendMsg(    CTRL( hdlg, DL_LIST), LM_INSERTITEM,
                                                        MPFROMSHORT( LIT_END), MPFROMP( buff)) ;

                            // select the top level menu
                            WinSendMsg( CTRL( hdlg, DL_LIST), LM_SELECTITEM,
                                        MPFROMSHORT( sPos), MPFROMSHORT( TRUE)) ;

                            sID = MN_START + sPos * TOTITEM ;

                            mi.iPosition = sPos ;
                            mi.afStyle = MIS_TEXT ;
                            mi.afAttribute = 0 ;
                            mi.id = sID ;
                            mi.hwndSubMenu = (HWND)NULL ;
                            mi.hItem = (ULONG)NULL ;

                            // add the top level to the action bar
                            WinSendMsg( hmenu, MM_INSERTITEM,
                                        MPFROMP(&mi), MPFROMP( buffer)) ;
                            StringParser( buffer) ;
                            break ;

                        case DL_SUBMENU:
                            if( fSubMenu)
                            {
                                // terminate the submenu creation
                                fSubMenu = 0 ;

                                // enable DL_TOP
                                WinEnableWindow( CTRL( hdlg, DL_TOP), TRUE) ;

                                // set DL_DROP as the next choice
                                WinSendMsg( hdlg, WM_CONTROL, MPFROM2SHORT( DL_DROP, 0), 0L) ;

                                // change text in DL_SUBMENU
                                WinSetWindowText( CTRL( hdlg, DL_SUBMENU), (PSZ)"Submenu") ;
                                // no more subsub menu
                                hwndSubSub = (HWND)NULL ;
                                break ;
                            }
                            // determine the selected top level
                            sPos = (SHORT)WinSendMsg( CTRL( hdlg, DL_LIST), LM_QUERYSELECTION,
                                            MPFROMSHORT( LIT_FIRST), 0L) ;

                            // no top level available
                            if( sPos < 0)
                                break ;

                            // query the item
                            WinSendMsg( hmenu, MM_QUERYITEM,
                                        MPFROM2SHORT( MN_START + sPos * TOTITEM, FALSE),
                                        (MPARAM)&mi) ;

                            // is there a drop down?
                            if( !mi.hwndSubMenu)
                            {
                                // create a new window of class WC_MENU
                                mi.hwndSubMenu = WinCreateWindow( HWND_OBJECT,
                                                                  WC_MENU,
                                                                  NULL,
                                                                  0L,
                                                                  0, 0, 0, 0,
                                                                  hmenu, HWND_TOP,
                                                                  0, NULL, NULL) ;

                                mi.afStyle = MIS_TEXT | MIS_SUBMENU ;

                                // set the drop down handle
                                // in the top level data structure
                                WinSendMsg( hmenu, MM_SETITEM,
                                            MPFROM2SHORT( 0, FALSE),
                                            (MPARAM) &mi) ;
                                hwndSubMenu = mi.hwndSubMenu ;

                                // change the top level menu
                                WinSendMsg( hmenu, MM_SETITEM,
                                            MPFROM2SHORT( 0, FALSE),
                                            (MPARAM)&mi) ;
                            }

                            // store the drop down handle
                            hwndSubMenu = mi.hwndSubMenu ;

                            // set the new ID
                            sID = MN_START + sPos * TOTITEM + 1 + sCount[ sPos] ;

                            sCount[ sPos]++ ;

                            // set the data structure for the changed item
                            mi.iPosition = MIT_END ;
                            mi.afStyle = MIS_TEXT | MIS_SUBMENU ;
                            mi.afAttribute = usAttribute ;
                            mi.id = sID ;
                            mi.hItem = (ULONG)NULL ;

                            // create a new window of class WC_MENU
                            mi.hwndSubMenu = WinCreateWindow(    HWND_OBJECT, WC_MENU,
                                                                NULL,
                                                                WS_VISIBLE,
                                                                0, 0, 0, 0,
                                                                hmenu, HWND_TOP,
                                                                0, NULL, NULL) ;
                            // add the SUBMENU data to the drop down
                            WinSendMsg( hwndSubMenu, MM_INSERTITEM,
                                        (MPARAM)&mi, MPFROMP( buffer)) ;

                            // creating a submenu
                            fSubMenu = 1 ;

                            // storing the SUBMENU SUBMENU handle
                            hwndSubSub = mi.hwndSubMenu ;

                            // attribute resetting
                            usAttribute = 0 ;

                            // disable TERMINATE & CANCEL
                            WinEnableWindow( CTRL( hdlg, DL_TERMINATE), FALSE) ;
                            WinEnableWindow( CTRL( hdlg, DID_CANCEL), FALSE) ;

                            // disable DL_TOP, DL_SUBMENU e DL_SEPARATOR
                            WinEnableWindow( CTRL( hdlg, DL_TOP), FALSE) ;
                            WinEnableWindow( CTRL( hdlg, DL_SUBMENU), FALSE) ;
                            WinEnableWindow( CTRL( hdlg, DL_SEPARATOR), FALSE) ;
                            WinSendMsg( hdlg, WM_CONTROL,
                                        MPFROM2SHORT( DL_DROP, 0), 0L) ;
                            break ;

                       case DL_SEPARATOR:
                            // determine the selected top level
                            sPos = (SHORT)WinSendMsg(CTRL( hdlg, DL_LIST), LM_QUERYSELECTION,
                                                        MPFROMSHORT( LIT_FIRST), 0L) ;

                            // query the item
                            WinSendMsg( hmenu, MM_QUERYITEM,
                                        MPFROM2SHORT( MN_START + sPos * TOTITEM, FALSE),
                                        (MPARAM)&mi) ;

                            // check if there is a drop down
                            // otherwise we skip over cause we don't want a
                            // SEPARATOR in a top level
                            if( mi.hwndSubMenu == (HWND)NULL)
                                break ;

                            // no SEPARATOR before a generic MENUITEM
                            if( sCount[ sPos] < 1)
                                break ;

                            // set the new ID
                            sID = MN_START + sPos * TOTITEM + 1 +sCount[ sPos] ;

                            // increase the menuitem counter
                            sCount[ sPos]++ ;

                            hwndSubMenu = mi.hwndSubMenu ;

                            // set the data structure
                            mi.iPosition = MIT_END ;
                            mi.afStyle = MIS_SEPARATOR ;
                            mi.afAttribute = 0 ;
                            mi.id = sID ;
                            mi.hItem = (ULONG)NULL ;
                            mi.hwndSubMenu = (HWND)NULL ;

                            // add the drop down to the top level
                            WinSendMsg( (fSubMenu) ? hwndSubSub : hwndSubMenu,
                                        MM_INSERTITEM,
                                        (MPARAM)&mi, 0L) ;
                            break ;
                    }// radio button switch

                    WinSetWindowText( CTRL( hdlg, DL_ITEM), NULL) ;

                    // reset the accelerators if used
                    if( WinQueryWindowText( CTRL( hdlg, DL_ACC), sizeof buffer, buffer))
                        WinSetWindowText( CTRL( hdlg, DL_ACC), NULL) ;

                    // attributes resetting
                    usAttribute = 0 ;
                    WinSendMsg( CTRL( hdlg, DL_DISABLED), BM_SETCHECK,
                                MPFROMSHORT( FALSE), 0L) ;

                    WinSendMsg( CTRL( hdlg, DL_CHECKED), BM_SETCHECK,
                                MPFROMSHORT( FALSE), 0L) ;

                    if( fExtCmd)
                    {
                        WinSendMsg( CTRL( hdlg, DL_EXTCMD), BM_SETCHECK,
                                    MPFROMSHORT( FALSE), 0L) ;
                        fExtCmd = FALSE ;
                    }

                    // disabile DL_ITEM
                    WinEnableWindow( CTRL( hdlg, DL_ITEM), TRUE) ;
                    WinEnableWindow( CTRL( hdlg, DL_ITEMTEXT), TRUE) ;
                    return 0L ;

                case DID_CANCEL:
                    WinDismissDlg( hdlg, FALSE) ;
                    break ;
            }
            break ;
        }
        default:
            break ;
    }
    return WinDefDlgProc( hdlg, msg, mp1, mp2) ;
}

BOOL StringParser( register char *szString)
{

    ULONG i = 0 ;
    char szFinal[ 80] ;
    char *s = szString ;

    while( *szString)
    {
        if( *szString == '~' || *szString == '.')
        {
            szString++ ;
            continue ;
        }
        if( *szString == '\t')
            break ;
        *( szFinal + i++) = *szString++ ;
    }
    *( szFinal + i) = '\0' ;

    // copy parsed string on the original
    strcpy( s, szFinal) ;

    // return pointer to parsed string
    return TRUE ;
}

BOOL MaskHandler( HWND hdlg, BOOL fActivate)
{
    // enable/disable accelerators & extended commands
    WinEnableWindow( CTRL( hdlg, DL_ACC), fActivate) ;
    WinEnableWindow( CTRL( hdlg, DL_ACCTEXT), fActivate) ;
    WinEnableWindow( CTRL( hdlg, DL_EXTCMD), fActivate) ;
    WinEnableWindow( CTRL( hdlg, DL_CHECKED), fActivate) ;
    WinEnableWindow( CTRL( hdlg, DL_DISABLED), fActivate) ;

    // erase the entryfield
    WinSetWindowText( CTRL( hdlg, DL_ITEM), NULL) ;

    return fActivate ;
}

BOOL Save( HWND hwnd, char *szProject)
{

    SHORT sMenuCount = 0 ;
    MENUITEM mi ;
    CHAR buffer[ 200] ;
    HFILE hFile1, hFile2, hFile3 ; 
    APIRET apiret ;
    ULONG ulAction, ulWritten ;
    ULONG ulBufferSize = 0L ;
    CHAR szMenuItem[ 200], szIDBuffer[ 200], szDefine[ 200] ;
    SHORT sItem ;
    HWND hwndSubMenu, hmenu = MENU( hwnd) ;

    // check the previous menus
    while( WinSendMsg(  hmenu, MM_QUERYITEM,
                        MPFROM2SHORT( MN_START + sMenuCount * TOTITEM, FALSE),
                        (MPARAM)&mi))
    {
        // get the text in the top level
        if( !WinSendMsg(    hmenu, MM_QUERYITEMTEXT,
                            MPFROM2SHORT( MN_START + sMenuCount * TOTITEM, 15),
                            MPFROMP( buffer)))
            continue ;

        if( !sMenuCount)
        {
            // open the file
            sprintf( szIDBuffer, "%s%s", szProject, ".MNU") ;
            if( apiret = DosOpen(   szIDBuffer,&hFile1,
                                    &ulAction,ulBufferSize,
                                    FILE_NORMAL,
                                    OPEN_ACTION_REPLACE_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW,
                                    OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE| OPEN_FLAGS_SEQUENTIAL,
                                    NULL))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            sprintf( szIDBuffer, "%s%s", szProject, ".HHH") ;
            if( apiret = DosOpen(   szIDBuffer,
                                    &hFile2,
                                    &ulAction,
                                    ulBufferSize,
                                    FILE_NORMAL,
                                    OPEN_ACTION_REPLACE_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW,
                                    OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE| OPEN_FLAGS_SEQUENTIAL,
                                    NULL))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            sprintf( szIDBuffer, "%s%s", szProject, ".ACC") ;
            if( apiret = DosOpen(   szIDBuffer,
                                    &hFile3,
                                    &ulAction,
                                    ulBufferSize,
                                    FILE_NORMAL,
                                    OPEN_ACTION_REPLACE_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW,
                                    OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE| OPEN_FLAGS_SEQUENTIAL,
                                    NULL))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // write the MNU header
            sprintf( szMenuItem, "MENU %s\n{\n", "ID_RES") ;
            if( apiret = DosWrite(  hFile1,
                                    szMenuItem,
                                    strlen( szMenuItem),
                                    &ulWritten))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // write the HHH definitions
            sprintf( szMenuItem, "#define\tMN_MENU\t%d\n", MENUID) ;
            if( apiret = DosWrite(    hFile2,
                                    szMenuItem,
                                    strlen( szMenuItem),
                                    &ulWritten))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // write the ACC header
            sprintf( szMenuItem, "ACCELTABLE %s\n{\n", "ID_RES") ;
            if( apiret = DosWrite(    hFile3,
                                    szMenuItem,
                                    strlen( szMenuItem),
                                    &ulWritten))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        }

        strcpy( szIDBuffer, buffer) ;
        StringParser( szIDBuffer) ;
        strupr( szIDBuffer) ;

        // write the top level
        sprintf(    szMenuItem, "\t%s \"%s\", MN_%s\n\t%s",
                    ( mi.hwndSubMenu) ? "SUBMENU" : "MENUITEM",
                    buffer,
                    szIDBuffer,
                    ( mi.hwndSubMenu) ? "{\n" : "\n") ;

        if( apiret = DosWrite(    hFile1,
                                szMenuItem,
                                strlen( szMenuItem),
                                &ulWritten))
            WinAlarm( HWND_DESKTOP, WA_ERROR) ;

        // write the IDs in .HHH
        sprintf( szDefine, "\n#define MN_%s\t%d\n", szIDBuffer, mi.id) ;
        if( apiret = DosWrite(    hFile2,
                                szDefine,
                                strlen( szDefine),
                                &ulWritten))
            WinAlarm( HWND_DESKTOP, WA_ERROR) ;

        // there is a drop down
        if( mi.hwndSubMenu)
        {
            // drop down handle
            hwndSubMenu = mi.hwndSubMenu ;

            // count the items
            sItem = (SHORT)WinSendMsg( hwndSubMenu, MM_QUERYITEMCOUNT, 0L, 0L) ;

            CheckSubMenu( hwndSubMenu, sItem, hFile1, hFile2, hFile3, TRUE, sMenuCount) ;

            // close the top level
            strcpy( szMenuItem, "\t}\n") ;

            if( apiret = DosWrite(    hFile1,
                                    szMenuItem,
                                    strlen( szMenuItem),
                                    &ulWritten))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        }

        // increase the counter
        sMenuCount++ ;
    }

    if( sMenuCount)
    {
        // close the template in MNU
        sprintf( buffer, "}%s", "\n") ;
        if( apiret = DosWrite(    hFile1,
                            buffer,
                            strlen( buffer),
                            &ulWritten))
            WinAlarm( HWND_DESKTOP, WA_ERROR) ;

        // close the template in ACC
        sprintf( buffer, "}%s", "\n") ;
        if( apiret = DosWrite(    hFile3,
                                buffer,
                                strlen( buffer),
                                &ulWritten))
            WinAlarm( HWND_DESKTOP, WA_ERROR) ;

        DosClose( hFile1) ;
        DosClose( hFile2) ;
        DosClose( hFile2) ;
    }
    return TRUE ;
}

BOOL CheckSubMenu(  HWND hwndSubMenu,
                    SHORT sItemNumber,
                    HFILE hFile1,
                    HFILE hFile2,
                    HFILE hFile3,
                    BOOL fNewTop,
                    SHORT sTopLevel)
{
    static SHORT sCount, sMenuCount = -1 ;
    SHORT sID, i ;
    ULONG ulWritten ;
    BOOL fSubMenu ;
    CHAR szMenuItem[ 80], buffer[ 80], szIDBuffer[ 80], szDefine[ 80] ;
    MENUITEM mit ;
    CHAR *s ;
    APIRET apiret ;

    // reset the counter for each top level
    if( fNewTop)
    {
        sCount = 0 ;
        sMenuCount++ ;
    }

    for( i = 0; i < sItemNumber; i++)
    {
        sID = MN_START + sTopLevel * TOTITEM + ++sCount;

        fSubMenu = FALSE ;

        if( !WinSendMsg(    hwndSubMenu, MM_QUERYITEM,
                            MPFROM2SHORT( sID, TRUE),
                            (MPARAM)&mit) || mit.afStyle == MIS_SEPARATOR)
        {
            strcpy( szMenuItem, strcat( strrep( 2 + fSubMenu),    "MENUITEM SEPARATOR\n")) ;
        }
        else
        {
            // get the text
            WinSendMsg( hwndSubMenu, MM_QUERYITEMTEXT,
                            MPFROM2SHORT( sID, sizeof buffer),
                            MPFROMP( buffer)) ;

            if( mit.hwndSubMenu != 0L)
                fSubMenu = TRUE ;

            strcpy( szIDBuffer, buffer) ;

            // remove ... and ~
            StringParser( szIDBuffer) ;

            // upper case
            strupr( szIDBuffer) ;

            // remove blanks from the ID
            if( strchr( szIDBuffer, ' '))
                RemoveBlanks( szIDBuffer) ;

            // save ACCELTABLE
            if( strchr( buffer, '\t'))
            {
                s = Accel( szIDBuffer, strchr( buffer, '\t') + 1) ;

                // writing in ACC
                if( ( apiret = DosWrite( hFile3, s, strlen( s), &ulWritten)))
                    WinAlarm( HWND_DESKTOP, WA_ERROR) ;
            }

            // TAB substitution
            Sub1with2( buffer) ;

            // writing a drop down menuitem/submenu
            sprintf( szMenuItem, "%s%s \"%s\", MN_%s%s%s",
                 (fNewTop) ? "\t\t" : "\t\t\t",
                 (fSubMenu) ? "SUBMENU" : "MENUITEM",
                 buffer, szIDBuffer,
                 (mit.afAttribute == 0x2000) ? ",,MIA_CHECKED" :
                 (mit.afAttribute == 0x4000) ? ",,MIA_DISABLED" : "",
                 (fSubMenu) ? "\n\t\t{\n" : "\n") ;

            // write #define in .HHH
            sprintf( szDefine, "#define MN_%s\t%d\n", szIDBuffer, mit.id) ;
            if( ( apiret = DosWrite( hFile2, szDefine, strlen( szDefine), &ulWritten)))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        }
        // writing a menuitem in .MNU
        if( ( apiret = DosWrite( hFile1, szMenuItem, strlen( szMenuItem), &ulWritten)))
            WinAlarm( HWND_DESKTOP, WA_ERROR) ;

        // if it is a SUBMENU lets calculate each MENUITEM
        if( fSubMenu)
            CheckSubMenu(   mit.hwndSubMenu,
                            (SHORT)WinSendMsg( mit.hwndSubMenu, MM_QUERYITEMCOUNT, 0L, 0L),
                            hFile1, hFile2, hFile3, FALSE, sTopLevel) ;
    }
    if( !fNewTop)
    {
        strcpy( szMenuItem, "\t\t}\n") ;
        if( ( apiret = DosWrite( hFile1, szMenuItem, strlen( szMenuItem), &ulWritten)))
            WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        fNewTop = TRUE ;
    }
    return TRUE ;
}

SHORT CalcMenuItem( SHORT sMenuCount,
                    HWND hwndSubMenu,
                    HWND hmenu,
                    BOOL fReset)
{

    MENUITEM mi ;
    SHORT sItems, i, j ;
    static SHORT sCounter ;

    // counter reset for a new top level
    if( fReset)
        sCounter = 0 ;

    sItems = (SHORT) WinSendMsg( hwndSubMenu, MM_QUERYITEMCOUNT, 0L, 0L) ;

    for( i = 0; i < sItems; i++)
    {
        sCounter++ ;
        j = MN_START + sMenuCount * TOTITEM + sCounter ;
        if( !WinSendMsg( hmenu, MM_QUERYITEM,
                MPFROM2SHORT( j, TRUE),
                (MPARAM) &mi))
        {

            // skip over if a SEPARATOR
            continue ;
        }

        // is there a SUBMENU
        if( mi.hwndSubMenu)
            CalcMenuItem( sMenuCount, mi.hwndSubMenu, hmenu, FALSE) ;
    }
    return sCounter    ;
}

char *strrep( SHORT sRep)
{

    CHAR buffer[ 50] ;
    SHORT i ;

    for( i = 0; i < sRep; i++)
    {
        buffer[ i] = '\t' ;
    }
    buffer[ i] = '\0' ;

    return buffer ;
}

void Sub1with2( register char *s)
{
    char *s1 ;

    if( !( s1 = strchr( s, '\t')))
        return ;

    // transfer s at the end
    while( *s++)
        ;

    *( s + 1) = '\0' ;

    while( s != s1)
    {
        *s = *(s - 1) ;
        s-- ;
    }

    *s1++ = '\\' ;
    *s1 = 't' ;

}

void RemoveBlanks( register char *s)
{
    SHORT sCnt = 0 ;

    while( *s)
    {

        *( s - sCnt) = *s ;

        if( *s == ' ')
            sCnt++ ;

        s++ ;
    }
    *( s -sCnt) = '\0' ;
}

MRESULT EXPENTRY Project(   HWND hdlg,
                            ULONG msg,
                            MPARAM mp1,
                            MPARAM mp2)
{
    static char *s ;

    switch( msg)
    {
        case WM_INITDLG:
            s = (CHAR *)PVOIDFROMMP( mp2) ;
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case DID_OK:
                    if( WinQueryWindowText( (HWND)CTRL( hdlg, DL_EDIT),
                                            FILENAME, s))
                        WinDismissDlg( hdlg, TRUE) ;
                    return (MRESULT)0L ;

                default:
                    break ;
            }
            break ;

        default:
            break ;
    }
    return WinDefDlgProc( hdlg, msg, mp1, mp2) ;
}

char *Accel( register char *t, register char *s)
{
    char *szParam[] = { "ALT", "CTRL", "SHIFT"} ;
    char *szOption[] = { "ALT", "CTRL", "SHIFT"} ;
    char *szKeys[] = { "INS", "DEL", "BACK"} ;
    char *szDef[] = { "VK_INSERT", "VK_DELETE", "VK_BACKSPACE"} ;
    char szCommand[ 40] = { '\0'} ;
    char szAttr[ 40] = { '\0'};
    char *p, *l = NULL ;
    short i ;
    CHAR szFinal[ 150] ;

    // remove the blanks
    RemoveBlanks( s) ;

    // look for CTRL, SHIFT & ALT
    for ( i = 0; i < 3; i++)
    {
        if( ( p = stristr( s, szParam[ i])))
        {
            strcat( szAttr, ", ") ;
            strcat( szAttr, szOption[ i]) ;
            l = MAX( l, p + strlen( szParam[ i])) ;
        }

    }
    s = l ;
    if( *s == '+')
        ++s ;

    // look for dead keys
    for( i = 0; i < 3; i++)
    {
        if( stristr( s, szKeys[ i]))
        {
            strcat( szCommand, szDef[ i]) ;
            strcat( szCommand, ", ") ;
            s += strlen( szKeys[ i]) ;
            break ;
        }

    }

    // is szCommand empty?
    if( *szCommand == '\0')
    {
        sprintf( szCommand,    "\"%c\", ", *s) ;
    }

    // prepare the final string
    sprintf( szFinal, "\t%sMN_%s%s\n", szCommand, t, szAttr) ;

    // merge the two strings
    return szFinal ;

}

char *stristr( register char *s1, register char *s2)
{
    char *s3, *s4 = s2 ;
    char c1, c2 ;

    while( *s1 || *s2)
    {
        c1 = *s1 ;
        c2 = *s2 ;

        while( *s1 && toupper( c1) != toupper( c2))
        {
            s1++ ;
            c1 = *s1 ;
        }
        if( !*s1)
            return NULL ;

        s3 = s1 ;

        while( *++s2)
        {
            c1 = *++s1 ;
            c2 = *s2 ;
            if( toupper( c1) != toupper( c2))
                break ;
        }
        if( !*s2)
            return s3 ;

        s2 = s4 ;
        s1 = s3 + 1 ;
    }
    return NULL ;
}
