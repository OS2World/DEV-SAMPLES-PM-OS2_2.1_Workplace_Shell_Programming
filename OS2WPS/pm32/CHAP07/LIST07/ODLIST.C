// ODLIST.C - Owner-draw listbox
// Listing 07-07

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_DOS
#define INCL_GPI

#include <os2.h>
#include <stdio.h>
#include "odlist.h"

// definitions & macros
#define OFFSET 10
#define ID_LISTBOX 1
#define DOCNUMBER   8

#define HAB( x)     WinQueryAnchorBlock( x)
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define SYS( x)     WinQuerySysValue( HWND_DESKTOP, x)
#define MENU( x)    WinWindowFromID( x, FID_MENU)
#define LIST( x)    WinWindowFromID( x, ID_LISTBOX)

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ChildWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY NewList( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
void CreateChildWindows( HWND hwnd, HWND hwndListbox) ;
int main( void) ;


int main( void)
{
    CHAR szClassName[ 15] ;
    CHAR szChildClass[ 15] ;
    CHAR szWindowTitle[ 25] ;
    HMQ hmq ;
    HWND hwndClient, hwndFrame ;
    HAB hab ;
    QMSG qmsg ;
    ULONG ulFCFlags = FCF_STANDARD & ~FCF_ACCELTABLE & ~FCF_SHELLPOSITION ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // loading clas names & window title
    WinLoadString(  hab, NULLHANDLE,
                    ST_CLASSNAME,
                    sizeof( szClassName), szClassName) ;
    WinLoadString(  hab, NULLHANDLE,
                    ST_CHILDCLASS,
                    sizeof( szChildClass), szChildClass) ;
    WinLoadString(  hab, NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof( szWindowTitle), szWindowTitle) ;

    if( !WinRegisterClass(  hab,
                            szClassName,
                            ClientWndProc,
                            CS_SIZEREDRAW, sizeof( PDATA)))
        return FALSE ;

    if( !WinRegisterClass(  hab,
                            szChildClass,
                            ChildWndProc,
                            CS_SIZEREDRAW, sizeof( LONG)))
        return FALSE ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &ulFCFlags,
                                    szClassName,
                                    szWindowTitle,
                                    WS_CLIPCHILDREN,
                                    NULLHANDLE,
                                    RS_ALL,
                                    &hwndClient) ;

    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        40, 40, 550, 440,
                        SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ACTIVATE) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
        WinDispatchMsg( hab, &qmsg);

    // destroy the chunk of memory
    DosFreeMem( (PVOID)WinQueryWindowPtr( hwndClient, QWL_USER)) ;

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
    static HWND hwndListbox ;
    static HBITMAP hbm[ 2] ;
    static LONG cy = 0, cx = 16 ;
    static RECTL rc ;

    switch( msg)
    {
        case WM_CREATE:
        {
            HPS hps ;
            FONTMETRICS fm ;
            PDATA pData ;

            // allocate the chunk of memory
            if( DosAllocMem( (PPVOID)&pData, sizeof( DATA) * DOCNUMBER, PAG_READ | PAG_WRITE | PAG_COMMIT))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // store the pointer
            WinSetWindowPtr( hwnd, QWL_USER, pData) ;

            hps = WinGetPS( hwnd) ;
            GpiQueryFontMetrics( hps, sizeof( FONTMETRICS), &fm) ;
            cy = fm.lMaxBaselineExt ;
            hbm[ 1] = GpiLoadBitmap( hps, 0, 100, 0, 0) ;  // show
            hbm[ 0] = GpiLoadBitmap( hps, 0, 101, 0, 0) ;  // hide
            WinReleasePS( hps) ;

            hwndListbox = WinCreateWindow(  hwnd, WC_LISTBOX,
                                            NULL,
                                            LS_OWNERDRAW | WS_CLIPSIBLINGS,
                                            10, 10, 180, 120,
                                            hwnd, HWND_TOP,
                                            ID_LISTBOX,
                                            NULL, NULL) ;
        }
            break ;

        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1))
            {
                case MN_DIALOG:
                    // creating child documents
                    CreateChildWindows( hwnd, hwndListbox) ;
                    // show the listbox
                    WinShowWindow( hwndListbox, TRUE) ;
                    WinSendMsg( MENU( PAPA( hwnd)), MM_SETITEMATTR,
                                MPFROM2SHORT( MN_DIALOG, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED)) ;
                    break ;

                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
                    break ;
            }
                break ;

        case WM_MEASUREITEM:
            return MRFROMLONG( cy) ;

        case WM_DRAWITEM:
        {
            RECTL rcl ;
            OWNERITEM * pitem ;
            LONG clrForeGround, clrBackGround ;
            POINTL ptl = { 0, 0} ;
            CHAR szbuf[ 80] ;
            BITMAPINFOHEADER bmp ;
            HWND hwndDoc, hwndCheck ;
            PDATA pData ;
            LONG lOffset ;

            // pointer to the OWNERITEM struct
            pitem = (POWNERITEM)PVOIDFROMMP( mp2) ;

            // determine the text
            WinSendMsg( pitem -> hwnd, LM_QUERYITEMTEXT,
                        MPFROM2SHORT( pitem -> idItem, sizeof szbuf),
                        (MPARAM)(PSZ)szbuf) ;

            // icon's rectangle
            rc.xLeft = pitem -> rclItem.xLeft ;
            rc.yBottom = pitem -> rclItem.yBottom ;

            GpiQueryBitmapParameters( hbm[ 0], &bmp) ;
            rc.xRight = cx = rc.xLeft + bmp.cx ;
            rc.yTop = rc.yBottom + cy ;

            // check the pointer pos
            WinQueryPointerPos( HWND_DESKTOP, &ptl) ;
            hwndCheck = WinWindowFromPoint( HWND_DESKTOP, &ptl, TRUE) ;

            // get the doc hwnd
            hwndDoc = (HWND)WinSendMsg( hwndListbox, LM_QUERYITEMHANDLE,
                                        MPFROMSHORT( pitem -> idItem), 0L) ;

            // get the data pointer
            pData = (PDATA)WinQueryWindowPtr( hwnd, QWL_USER) ;
            lOffset = (LONG)WinQueryWindowULong( hwndDoc, QWL_USER) ;
            pData += lOffset ;

            // are you clicking on the listbox?
            if( hwndCheck == hwndListbox)
            {
                WinMapWindowPoints( HWND_DESKTOP, hwndListbox, &ptl, 1L) ;

                // is the mouse on the icon?
                if( ptl.x > 0 && ptl.x <= ( cx + OFFSET) && pitem -> fsState)
                {
                    WinAlarm( HWND_DESKTOP, WA_NOTE) ;

                    pData -> fStatus = ! pData -> fStatus ;

                    // show/hide the doc window
                    WinSetWindowPos(    PAPA( hwndDoc), HWND_TOP,
                                        0L, 0L, 0L, 0L,
                                        ( pData -> fStatus) ? (SWP_SHOW | SWP_ACTIVATE) : SWP_HIDE) ;
                }
            }

            // no highlight on the selected item
            if(  pitem -> fsState == FALSE )
            {
                clrForeGround = CLR_BLACK ;
                clrBackGround = CLR_WHITE ;
            }
            else
            {
                clrForeGround = CLR_WHITE ;
                clrBackGround = CLR_BLACK ;
            }

            // draw the icon
            WinDrawBitmap(  pitem -> hps,
                            ( pData -> fStatus) ? hbm[ 0] : hbm[ 1],
                            NULL,
                            (PPOINTL)&rc,
                            CLR_BLACK, CLR_WHITE,
                            DBM_STRETCH) ;

            // determining the text rectangle
            rcl.xLeft = pitem -> rclItem.xLeft + bmp.cx + OFFSET ;
            rcl.xRight = pitem -> rclItem.xRight ;
            rcl.yTop = pitem -> rclItem.yTop ;
            rcl.yBottom = pitem -> rclItem.yBottom ;

            // writing text
            WinDrawText(    pitem -> hps,
                            -1,
                            szbuf,
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

            // don't change the active doc if we are scrolling
            if( hwndCheck != hwndListbox)
            {
                // reactivate current doc
                WinSendMsg( (HWND)WinQueryWindowULong( hwndListbox, QWL_USER), WM_ACTIVATE,
                            MPFROMSHORT( TRUE),
                            MPFROMHWND( WinQueryWindowULong( hwndListbox, QWL_USER))) ;
                return (MRESULT)TRUE ;
            }

            // changing active doc
            if( PAPA( hwndDoc) != (HWND)WinQueryWindowULong( hwndListbox, QWL_USER))
            {
                // deactivate current doc
                WinSendMsg( (HWND)WinQueryWindowULong( hwndListbox, QWL_USER), WM_ACTIVATE,
                            MPFROMSHORT( FALSE),
                            MPFROMHWND( WinQueryWindowULong( hwndListbox, QWL_USER))) ;

                // activate selected doc
                WinSetWindowPos(    PAPA( hwndDoc), HWND_TOP,
                                    0L, 0L, 0L, 0L,
                                    SWP_ACTIVATE | SWP_ZORDER) ;

                // store the active doc hwnd
                WinSetWindowULong( hwndListbox, QWL_USER, (ULONG)PAPA( hwndDoc)) ;
            }
        }
        return (MRESULT)TRUE ;

        case WM_CLOSE:
            WinPostMsg( hwnd, WM_QUIT, 0L, 0L);
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

MRESULT EXPENTRY ChildWndProc(  HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;
            PDATA pData ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            pData = (PDATA)WinQueryWindowPtr( PAPA( PAPA( hwnd)), 0) ;
            pData += WinQueryWindowULong( hwnd, QWL_USER) ;

            WinFillRect( hps, &rc, pData -> clr) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_ACTIVATE:
        {
            short sPos, sID ;
            HWND hwndListbox = LIST( PAPA( PAPA( hwnd))) ;
            HWND hwndDoc ;

            sPos = (short)WinSendMsg(   hwndListbox, LM_QUERYSELECTION,
                                        MPFROMSHORT( LIT_FIRST), 0L) ;

            sID = (short)WinQueryWindowULong( hwnd, QWL_USER) ;

            if( sID == sPos || sPos == LIT_NONE)
                break ;

            hwndDoc = (HWND)WinSendMsg( hwndListbox, LM_QUERYITEMHANDLE,
                                        MPFROMSHORT( sPos), 0L) ;

            // deselect the current selected item
            WinSendMsg( hwndListbox, LM_SELECTITEM,
                        MPFROMSHORT( sPos), MPFROMSHORT( FALSE)) ;

            // deactivate current doc
            WinSendMsg( hwndDoc, WM_ACTIVATE,
                        MPFROMSHORT( FALSE),
                        MPFROMHWND( hwndDoc)) ;


            // select the new item item
            WinSendMsg( hwndListbox, LM_SELECTITEM,
                        MPFROMSHORT( sID), MPFROMSHORT( TRUE)) ;

        }
            break ;



        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

void CreateChildWindows( HWND hwnd, HWND hwndListbox)
{
    short i ;
    short sPos ;
    HWND hwndFrame, hwndClient ;
    PDATA pData ;
    CHAR szString[ 80] ;
    ULONG ulFCF = FCF_TITLEBAR | FCF_SIZEBORDER ;
    CHAR szChildClass[ 15] ;

    // retrieve the pointer
    pData = (PDATA)WinQueryWindowPtr( hwnd, QWL_USER) ;

    WinLoadString(  HAB( hwnd), NULLHANDLE,
                    ST_CHILDCLASS,
                    sizeof( szChildClass), szChildClass) ;

    for( i = 0; i < DOCNUMBER; i++)
    {
        sprintf( szString, "Child #%2d", i) ;

        hwndFrame = WinCreateStdWindow( hwnd,
                                        WS_VISIBLE | WS_CLIPSIBLINGS,
                                        &ulFCF,
                                        szChildClass,
                                        szString,
                                        0L,
                                        NULLHANDLE,
                                        0L,
                                        &hwndClient) ;

        WinSetWindowPos(    hwndFrame, HWND_TOP,
                            1 + i * 40, 250 - i * 22,
                            110, 100,
                            SWP_MOVE | SWP_SIZE | SWP_ZORDER) ;

        // storing info
        pData -> fStatus = TRUE ;
        pData -> clr = i ;
        // storing the offset in the doc client window word
        WinSetWindowULong( hwndClient, QWL_USER, i) ;

        // increase the pointer
        pData++ ;

        // insert document text in the listbox
        sPos = (short)WinSendDlgItemMsg(    hwnd, ID_LISTBOX, LM_INSERTITEM,
                                            MPFROMSHORT( LIT_END),
                                            MPFROMP((PSZ)szString)) ;

        // storing the doc client window in the listbox data area
        WinSendDlgItemMsg(  hwnd, ID_LISTBOX, LM_SETITEMHANDLE,
                            MPFROMSHORT( sPos),
                            MPFROMHWND( hwndClient)) ;
    }
    // activate the last document
    WinSetActiveWindow( HWND_DESKTOP, hwndFrame) ;

    // storing the active doc frame hwnd in the listbox QWL_USER area
    WinSetWindowULong( hwndListbox, QWL_USER, (ULONG)hwndFrame) ;
}
