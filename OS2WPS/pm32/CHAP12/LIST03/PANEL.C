// PANEL.C - Valueset drag & drop example
// Listing 12-03

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "panel.h"
#include "mie02.h"


// macro definitions
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define MENU( x)        WinWindowFromID( x, FID_MENU)
#define CTRL( x, y)     WinWindowFromID( x, y)
#define SYS( x)         WinQuerySysValue( HWND_DESKTOP, x)
#define HAB( x)         WinQueryAnchorBlock( x)

#define CT_PANEL    1000

#define MAXCOL  2
#define MAXROW  4

// listbox frame
#define FRAME           10
#define WM_PASSPROC     WM_USER + 0
#define WM_PASSICON     WM_USER + 1


#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX
#define ID_LISTBOX  100

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ListWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 15] ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load class name from resource file
    WinLoadString(  hab, NULLHANDLE,
                    ST_CLASSNAME,
                    sizeof( szClassName), szClassName) ;
    WinLoadString(  hab, NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof( szWindowTitle), szWindowTitle) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, sizeof( HWND)) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName, szWindowTitle,
                                    WS_CLIPCHILDREN,
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
    //static COLOR clr = CLR_WHITE ;
    static HWND hwndList ;
    static HPOINTER hicon ;

    switch( msg)
    {
        case WM_CREATE:
        {
            HWND hwndValueset ;
            VSCDATA vscd ;
            LONG i, j ;
            CHAR szFont[] = "8.Helv" ;
            COLOR clr ;
            HWND hmenu ;

            // load the application menu
            hmenu = WinLoadMenu( HWND_DESKTOP, NULLHANDLE, RS_MENU) ;
            // store the menu in the applicationQWL_USER area
            WinSetWindowULong( hwnd, QWL_USER, hmenu) ;

            // valueset structure
            vscd.cbSize = sizeof vscd ;
            vscd.usRowCount = MAXROW ;
            vscd.usColumnCount = MAXCOL ;

            hwndValueset = WinCreateWindow( hwnd, WC_VALUESET,
                                            NULL,
                                            VS_ICON | VS_BORDER | WS_VISIBLE,
                                            10, 10,
                                            20 + SYS( SV_CXICON) * MAXCOL,
                                            20 + SYS( SV_CYICON) * MAXROW,
                                            hwnd, HWND_TOP, CT_PANEL,
                                            (PVOID)&vscd, NULL) ;

            for( i = 1; i <= MAXROW; i++)
                for( j = 1; j <= MAXCOL; j++)
                {
                    // set attributes
                    WinSendMsg( hwndValueset, VM_SETITEMATTR,
                                MPFROM2SHORT( i, j),
                                MPFROM2SHORT( VIA_DROPONABLE | VIA_DRAGGABLE, TRUE)) ;

                    // icon item
                    WinSendMsg( hwndValueset, VM_SETITEM,
                                MPFROM2SHORT( i, j),
                                MPFROMLONG( WinLoadPointer( HWND_DESKTOP, NULLHANDLE, RS_ICON))) ;
                }

            // create the listbox
            hwndList = WinCreateWindow( hwnd, WC_LISTBOX,
                                        NULL,
                                        LS_OWNERDRAW | WS_VISIBLE,
                                        200, 10, 0, 0,
                                        hwnd, HWND_TOP,
                                        ID_LISTBOX,
                                        NULL, NULL) ;

            // set a white background
            clr = CLR_WHITE ;
            WinSetPresParam( hwndList, PP_BACKGROUNDCOLORINDEX, sizeof( clr), &clr) ;

            // set font
            WinSetPresParam( hwndList, PP_FONTNAMESIZE, sizeof( szFont), szFont) ;

            // load the listbox menu
            hmenu = WinLoadMenu( HWND_DESKTOP, NULLHANDLE, RS_LISTMENU) ;
            // store the menu in the listbox QWL_USER area
            WinSetWindowULong( hwndList, QWL_USER, hmenu) ;

            // subclass it
            WinSendMsg( hwndList, WM_PASSPROC,
                        MPFROMP( WinSubclassWindow( hwndList, ListWndProc)), 0L) ;
        }
            break ;

        case WM_SIZE:
        {
            HWND hmenu ;
            SHORT sState ;

            // get the menu
            hmenu = WinQueryWindowULong( hwndList, QWL_USER) ;

            sState = (SHORT)WinSendMsg( hmenu, MM_QUERYITEMATTR,
                                        MPFROM2SHORT( MN_TEXT, TRUE),
                                        MPFROMSHORT( MIA_CHECKED)) ;
            WinSetWindowPos(    hwndList, HWND_TOP,
                                0, 0,
                                ( sState) ? 300 : SYS( SV_CXICON) + SYS( SV_CXVSCROLL) + FRAME,
                                SHORT2FROMMP( mp2),
                                SWP_SIZE | SWP_ZORDER) ;
        }
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps);
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CONTEXTMENU:
        {
            HWND hmenu ;
            POINTL pt ;
            HPS hps ;

            // get the mouse position
            pt.x = SHORT1FROMMP( mp1) ;
            pt.y = SHORT2FROMMP( mp1) ;
            // convert it in screen coordinates
            WinMapWindowPoints( hwnd, HWND_DESKTOP, &pt, 1L) ;

            // get the menu handle
            hmenu = (HWND)WinQueryWindowULong( hwnd, QWL_USER) ;

            // emphasize the main window
            hps = WinGetPS( hwnd) ;
            MieEmphasizeWindow( hwnd, hps) ;
            WinReleasePS( hps) ;

            WinPopupMenu(   HWND_DESKTOP, hwnd,
                            hmenu,
                            pt.x, pt.y,
                            0,
                            PU_NONE | PU_HCONSTRAIN | PU_VCONSTRAIN |
                            PU_MOUSEBUTTON2 | PU_MOUSEBUTTON1 | PU_KEYBOARD) ;
        }
            break ;

        case WM_MENUEND:
        {
            HPS hps ;

            // de-emphasize the main window
            hps = WinGetPS( hwnd) ;
            MieEmphasizeWindow( hwnd, hps) ;
            WinReleasePS( hps) ;
        }
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1 ))
            {
                case CT_PANEL:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case VN_SELECT:
                        {
                            USHORT usSelRow ;
                            USHORT usSelCol ;
                            MRESULT mr ;
                            HPOINTER hptr ;
                            HWND hwndPanel = CTRL( hwnd, CT_PANEL) ;

                            mr = WinSendMsg( hwndPanel, VM_QUERYSELECTEDITEM, NULL, NULL) ;

                            usSelCol = SHORT2FROMMR( mr) ;
                            usSelRow = SHORT1FROMMR( mr) ;

                            hptr = (HPOINTER) WinSendMsg(   hwndPanel, VM_QUERYITEM,
                                                            MPFROM2SHORT( usSelRow, usSelCol), 0L) ;

                            // insert your logic
                        }
                            break ;

                        case VN_INITDRAG:
                        {
                            WinAlarm( HWND_DESKTOP, WA_NOTE) ;

                        }
                            break ;

                        case VN_DROP:
                        {
                            PVSDRAGINFO pvsdraginfo ;
                            PDRAGINFO pdraginfo ;
                            PDRAGITEM pdragitem ;
                            ULONG ulItem, ulLen ;
                            CHAR szContainer[ 260] ;
                            HPOINTER hptr ;
                            PDRAGIMAGE pdragimage ;
 
                            // point to the VSDRAGINFO structure
                            pvsdraginfo = (PVSDRAGINFO)mp2 ;
                            pdraginfo = (PDRAGINFO)pvsdraginfo -> pDragInfo ;

                            if( !DrgAccessDraginfo( pdraginfo))
                            {
                                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                                break ;
                            }

                            // #of DRAGITEM structures
                            ulItem = DrgQueryDragitemCount( pdraginfo) ;
                            pdragitem = DrgQueryDragitemPtr( pdraginfo, 0L) ;

                            switch( pdraginfo -> usOperation)
                            {
                                case DO_DEFAULT:
                                    break ;

                                case DO_UNKNOWN:
                                    break ;

                                case DO_COPY:
                                    break ;

                                case DO_MOVE:
                                    break ;

                                case DO_LINK:
                                    break ;

                                case DO_CREATE:
                                    break ;
                            }

                            // retrieve the container name
                            ulLen = DrgQueryStrName(   pdragitem -> hstrContainerName,
                                                       sizeof( szContainer),
                                                       szContainer) ;
                            // retrieve the source name
                            DrgQueryStrName(   pdragitem -> hstrSourceName,
                                               sizeof( szContainer) - ulLen,
                                               szContainer + ulLen) ;

                            hptr = WinLoadFileIcon( szContainer, FALSE) ;
                            // skip if no icon is available
                            if( !hptr)
                                break ;

                            // set the new icon
                            WinSendMsg( CTRL( hwnd, CT_PANEL), VM_SETITEM,
                                        MPFROM2SHORT( pvsdraginfo -> usRow, pvsdraginfo -> usColumn),
                                        MPFROMLONG( hptr)) ;

                            // free the info block
                            DrgFreeDraginfo( pdraginfo) ;

                            WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                        }
                            break ;

                        case VN_DRAGOVER:
                        {
                            PVSDRAGINFO pvsdraginfo ;
                            PDRAGINFO pdraginfo ;

                            // point to the VSDRAGINFO structure
                            pvsdraginfo = (PVSDRAGINFO)mp2 ;
                            pdraginfo = (PDRAGINFO)pvsdraginfo -> pDragInfo ;

                        }
                            return MRFROM2SHORT( DOR_DROP, DO_MOVE) ;

                        default:
                            break ;
                    }
                    break;

                default:
                    break ;
            }
            break;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                    break ;

                case MN_PRODINFO:
                {
                    DLGDATA dlgData ;

                    dlgData.hwndOwner = hwnd ;
                    strcpy( dlgData.szWindowTitle, "Panel window") ;
                    MieDlgWnd( hwnd, &dlgData) ;
                }
                    break ;
            }
            break ;

        case WM_PASSICON:
            hicon = (HPOINTER)HWNDFROMMP( mp1) ;
            return 0L ;

        case WM_MEASUREITEM:
            return MRFROMLONG( SYS( SV_CYICON) + 10) ;

        case WM_DRAWITEM:
        {
            RECTL rcl ;
            OWNERITEM * pitem ;
            LONG clrForeGround, clrBackGround ;
            POINTL ptl = { 0, 0} ;
            CHAR szItemText[ 30] ;

            // pointer to the OWNERITEM struct
            pitem = (POWNERITEM)PVOIDFROMMP( mp2) ;

            // no highlight on the selected item
            if(  pitem -> fsState == FALSE )
            {
                HPOINTER hptr ;

                clrForeGround = CLR_BLACK ;
                clrBackGround = CLR_WHITE ;

                // retrieve the icon handle
                hptr = (HPOINTER)WinSendMsg(    hwndList, LM_QUERYITEMHANDLE,
                                                MPFROMSHORT( pitem -> idItem), 0L) ;
                if( hptr)
                    hicon = hptr ;
                else
                    // store the icon handle
                    WinSendMsg( hwndList, LM_SETITEMHANDLE,
                                MPFROMSHORT( pitem -> idItem), MPFROMLONG( hicon)) ;
            }
            else
            {
                clrForeGround = CLR_WHITE ;
                clrBackGround = CLR_BLACK ;
                // retrieve the icon handle
                hicon = (HPOINTER)WinSendMsg(   hwndList, LM_QUERYITEMHANDLE,
                                                MPFROMSHORT( pitem -> idItem), 0L) ;
            }

            // clear the icon background
            WinFillRect( pitem -> hps, &pitem -> rclItem, clrBackGround) ;

            // draw the icon
            WinDrawPointer( pitem -> hps,
                            pitem -> rclItem.xLeft + FRAME / 2,
                            pitem -> rclItem.yBottom + FRAME / 2,
                            hicon, DP_NORMAL) ;

            // determining the text rectangle
            rcl.xLeft = pitem -> rclItem.xLeft + FRAME + SYS( SV_CXICON) ;
            rcl.xRight = pitem -> rclItem.xRight ;
            rcl.yTop = pitem -> rclItem.yTop ;
            rcl.yBottom = pitem -> rclItem.yBottom ;

            // get the item text
            WinSendMsg( hwndList, LM_QUERYITEMTEXT,
                        MPFROM2SHORT( pitem -> idItem, sizeof( szItemText)), MPFROMP( szItemText)) ;

            // writing text
            WinDrawText(    pitem -> hps,
                            -1,
                            szItemText,
                            &rcl,
                            clrForeGround, clrBackGround,
                            DT_LEFT | DT_VCENTER | DT_ERASERECT) ;

            // item previously was unselected
            if( ( pitem -> fsState == FALSE) && ( pitem -> fsStateOld == TRUE))
            {
                pitem -> fsStateOld = FALSE ;
            }
            else
                // item was previously selected
                if( ( pitem -> fsState == TRUE) && ( pitem -> fsStateOld == FALSE) )
                {
                    pitem -> fsStateOld = TRUE;
                }
                else
                {
                    pitem -> fsStateOld = pitem -> fsState = FALSE ;
                }
        }
        return (MRESULT)TRUE ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}


MRESULT EXPENTRY ListWndProc(   HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static PFNWP pfnwp ;
    static BOOL fText ;
    static BOOL fEmph ;

    switch( msg)
    {
        case WM_PASSPROC:
            pfnwp = (PFNWP)mp1 ;
            return 0L ;

        case WM_CONTEXTMENU:
        {
            HWND hmenu ;
            POINTL pt ;
            SHORT sItems ;

            // get the mouse position
            pt.x = SHORT1FROMMP( mp1) ;
            pt.y = SHORT2FROMMP( mp1) ;
            // convert it in screen coordinates
            WinMapWindowPoints( hwnd, HWND_DESKTOP, &pt, 1L) ;

            // get the menu handle
            hmenu = (HWND)WinQueryWindowULong( hwnd, QWL_USER) ;

            // query the listbox content
            sItems = (SHORT)WinSendMsg( hwnd, LM_QUERYITEMCOUNT, 0L, 0L) ;
            // enable/disable the Empty menuitem
            if( sItems)
            {
                WinSendMsg( hmenu, MM_SETITEMATTR,
                            MPFROM2SHORT( MN_EMPTY, TRUE),
                            MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;
                WinSendMsg( hmenu, MM_SETITEMATTR,
                            MPFROM2SHORT( MN_TEXT, TRUE),
                            MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;
            }
            else
            {
                WinSendMsg( hmenu, MM_SETITEMATTR,
                            MPFROM2SHORT( MN_EMPTY, TRUE),
                            MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;
                WinSendMsg( hmenu, MM_SETITEMATTR,
                            MPFROM2SHORT( MN_TEXT, TRUE),
                            MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;
            }

            WinPopupMenu(   HWND_DESKTOP, hwnd,
                            hmenu,
                            pt.x, pt.y,
                            0,
                            PU_NONE | PU_HCONSTRAIN | PU_VCONSTRAIN |
                            PU_MOUSEBUTTON2 | PU_MOUSEBUTTON1 | PU_KEYBOARD) ;
        }
            break ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_EMPTY:
                    WinSendMsg( hwnd, LM_DELETEALL, 0L, 0L) ;
                    break ;

                case MN_TEXT:
                {
                    RECTL rc ;
                    HWND hmenu ;

                    // toggle the listbox status
                    fText = !fText ;

                    // get the menu handle
                    hmenu = (HWND)WinQueryWindowULong( hwnd, QWL_USER) ;
 
                    // add/remove the checkmark on the Show Text menuitem
                    if( fText)
                        WinSendMsg( hmenu, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_TEXT, TRUE),
                                MPFROM2SHORT( MIA_CHECKED, MIA_CHECKED)) ;
                    else
                        WinSendMsg( hmenu, MM_SETITEMATTR,
                                MPFROM2SHORT( MN_TEXT, TRUE),
                                MPFROM2SHORT( MIA_CHECKED, ~MIA_CHECKED)) ;

                    // query the window size
                    WinQueryWindowRect( hwnd, &rc) ;

                    // resize the listbox
                    WinSetWindowPos(    hwnd, HWND_TOP,
                                        0, 0,
                                        ( fText) ? 300 : SYS( SV_CXICON) + SYS( SV_CXVSCROLL) + FRAME,
                                        rc.yTop - rc.yBottom,
                                        SWP_SIZE | SWP_ZORDER) ;
                }
                    break ;
            }
            break ;

        case DM_DRAGOVER:
        {
            PDRAGINFO pdrginfo ;
            PDRAGITEM pdrgitem ;
            USHORT usDOR, usDO ;

            // pointer to the DRAGINFO structure
            pdrginfo = (PDRAGINFO)mp1 ;

            if( !DrgAccessDraginfo( pdrginfo))
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                break ;
            }
            // access the first DRAGITEM object
            pdrgitem = DrgQueryDragitemPtr( pdrginfo, 0L) ;

            // check if it is coming from a different window
            if( pdrginfo -> hwndSource == hwnd)
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
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
            HPS hps ;

            // de-enphasize the target
            hps = DrgGetPS( hwnd) ;
            //GpiErase( hps) ;
            fEmph = FALSE ;
            DrgReleasePS( hps) ;
        }
            return (MRESULT)NULL ;

        case DM_DROP:
        {
            PDRAGINFO pdraginfo ;
            PDRAGITEM pdragitem ;
            ULONG ulItem, ulLen ;
            CHAR szContainer[ 260] ;
            HPOINTER hptr ;
            PDRAGIMAGE pdragimage ;
            POINTL pt ;
            SHORT sSel, sItems ;

            // deselect everything
            WinSendMsg( hwnd, LM_SELECTITEM, MPFROMSHORT( LIT_NONE), MPFROMSHORT( FALSE)) ;

            // how many items in the listbox?
            sItems = (SHORT)WinSendMsg( hwnd, LM_QUERYITEMCOUNT, 0L, 0L) ;
            // query the pointer position
            WinQueryPointerPos( HWND_DESKTOP, &pt) ;
            // transform the point in listbox coordinates
            WinMapWindowPoints( HWND_DESKTOP, hwnd, &pt, 1L) ;
            // simulate a left button down
            WinSendMsg( hwnd, WM_BUTTON1DOWN, MPFROM2SHORT( (SHORT)pt.x, (SHORT)pt.y), 0L) ;
            // simulate a left button down
            WinSendMsg( hwnd, WM_BUTTON1UP, MPFROM2SHORT( (SHORT)pt.x, (SHORT)pt.y), 0L) ;
            // query the selected item
            sSel = (SHORT)WinSendMsg( hwnd, LM_QUERYSELECTION, MPFROMSHORT( LIT_FIRST), 0L) ;

            // if no selection the listbox might be either empty or
            // the user is beyond the last item
            if( sSel == -1)
                if( !sItems)
                    // insert as the first one
                    sSel = 0 ;
                else
                    // the last one
                    sSel = sItems ;

            // deselect everything
            WinSendMsg( hwnd, LM_SELECTITEM, MPFROMSHORT( LIT_NONE), MPFROMSHORT( FALSE)) ;

            // point to DRAGINFO structure
            pdraginfo = (PDRAGINFO)mp1 ;

            if( !DrgAccessDraginfo( pdraginfo))
            {
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                break ;
            }

            // #of DRAGITEM structures
            ulItem = DrgQueryDragitemCount( pdraginfo) ;
            pdragitem = DrgQueryDragitemPtr( pdraginfo, 0L) ;

            // retrieve the container name
            ulLen = DrgQueryStrName(    pdragitem -> hstrContainerName,
                                        sizeof( szContainer),
                                        szContainer) ;

            // retrieve the source name
            DrgQueryStrName(    pdragitem -> hstrSourceName,
                                sizeof( szContainer) - ulLen,
                                szContainer + ulLen) ;

            //hptr = WinQuerySysPointer( HWND_DESKTOP, SPTR_FILE, FALSE) ;
            hptr = WinLoadFileIcon( szContainer, FALSE) ;

            // reset emphatization control
            fEmph = FALSE ;

            // skip if no icon is available
            if( !hptr)
                 break ;

            // pass the icon handle
            WinSendMsg( PAPA( hwnd), WM_PASSICON, MPFROMLONG( hptr), 0L) ;

            // insert new item
            WinSendMsg( hwnd, LM_INSERTITEM, MPFROMSHORT( sSel), MPFROMP( szContainer)) ;

            // free the info block
            DrgFreeDraginfo( pdraginfo) ;
        }
            break ;

        default:
            break ;
    }
    return ( *pfnwp)( hwnd, msg, mp1, mp2) ;
}


