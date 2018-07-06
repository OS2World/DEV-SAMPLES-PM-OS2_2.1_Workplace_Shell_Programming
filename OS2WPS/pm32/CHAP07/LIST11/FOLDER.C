// FOLDER.C - WC_CONTAINER
// Listing  07-11

// Stefano Maruzzi 1992

#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <string.h>
#include "folder.h"


// definitions and macros
#define WM_PASSPROC     WM_USER + 0
#define WM_STARTUP      WM_USER + 1
#define WM_PASSOBJ      WM_USER + 2

#define CNR_COLS        2

#define OBJECTS         3UL

#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define MENU( x)        WinWindowFromID( x, FID_MENU)
#define CLIENT( x)      WinWindowFromID( x, FID_CLIENT)
#define HAB( x)         WinQueryAnchorBlock( x)

#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
MRESULT EXPENTRY FrameWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY RedWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY GreenWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY BlueWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
BOOL SetConditionalMenu( HWND hmenu, USHORT msg) ;
PRECORDCORE CreateAnotherObj( HWND hwnd, short sObjNum, PRECORDCORE precParent, LONG lType) ;
int main( void) ;

int main( void)
{
    CHAR szRedClass[] = "RED_CLASS" ;
    CHAR szGreenClass[] = "GREEN_CLASS" ;
    CHAR szBlueClass[] = "BLUE_CLASS" ;
    CHAR szWindowTitle[] = "Folder" ;
    HWND hwndFrame, hwndClient ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg;
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_SYSMENU ;
    PFNWP pfnwp ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // registration of the RED, GREEN and BLUE classes
    WinRegisterClass(   hab, szRedClass,
                        RedWndProc,
                        CS_SIZEREDRAW, 0L) ;
    WinRegisterClass(   hab, szGreenClass,
                        GreenWndProc,
                        CS_SIZEREDRAW, 0L) ;
    WinRegisterClass(   hab, szBlueClass,
                        BlueWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &flFrameFlags,
                                    WC_CONTAINER,
                                    szWindowTitle,
                                    CCS_AUTOPOSITION | CCS_EXTENDSEL,
                                    NULLHANDLE,
                                    RS_ICON,
                                    &hwndClient) ;

    // set the frame window as the owner
    WinSetOwner( hwndClient, hwndFrame) ;
    // set the FID_CLIENT ID
    WinSetWindowUShort( hwndClient, QWS_ID, FID_CLIENT) ;

    // subclass the container and pass the pointer
    pfnwp = WinSubclassWindow( hwndClient, ClientWndProc) ;
    WinSendMsg( hwndClient, WM_PASSPROC, (MPARAM)pfnwp, 0L) ;
    // prepare the container
    WinSendMsg( hwndClient, WM_STARTUP, 0L, 0L) ;

    // subclass the frame window
    pfnwp = WinSubclassWindow( hwndFrame, FrameWndProc) ;
    WinSendMsg( hwndFrame, WM_PASSPROC, (MPARAM)pfnwp, 0L) ;

    WinShowWindow( hwndFrame, TRUE) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0L, 0L))
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
    static HWND hpopup ;

    switch( msg)
    {
        case WM_PASSPROC:
        {
            MENUITEM mi ;

            pfnwp = (PFNWP)mp1 ;

            // loading the window context menu
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

        case WM_CONTROL:
        {
            static RECTL rcEmph ;
            static LONG hRound, vRound ;

            switch( SHORT1FROMMP( mp1))
            {
                case FID_CLIENT:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case CN_DRAGOVER:
                            WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                            break ;

                        case CN_REALLOCPSZ:
                        {
                        }
                            break ;

                        case CN_BEGINEDIT:
                        {
                        }
                            break ;

                        case CN_ENDEDIT:
                        {
                        }
                            break ;

                        case CN_EMPHASIS:
                        {
                        }
                            break ;

                        case CN_ENTER:
                        {
                            PNOTIFYRECORDENTER pntrecenter ;
                            PRECORDCORE prec ;
                            PAPPREC papprec ;
                            CHAR *szClass[] = { "RED_CLASS", "GREEN_CLASS", "BLUE_CLASS"} ;
                            CHAR *szWindowTitle[] = { "Red", "Green", "Blue"} ;
                            ULONG ulFCFrame = FCF_STANDARD & ~FCF_MENU & ~FCF_ACCELTABLE ;
                            HWND hwndFrame, hwndClient ;

                            pntrecenter = (PNOTIFYRECORDENTER)mp2 ;
                            prec = (PRECORDCORE)pntrecenter -> pRecord ;
 
                            papprec = (PAPPREC)prec ;
                            // skip if we are clicking on an empty place
                            if( !prec || !papprec -> precParent)
                                break ;

                            papprec = (PAPPREC)prec ;

                            // create a window of the appropriate class
                            hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                                            0L,
                                                            &ulFCFrame,
                                                            szClass[ papprec -> lType - 1],
                                                            szWindowTitle[ papprec -> lType - 1],
                                                            0L,
                                                            NULLHANDLE,
                                                            OBJECTID + papprec -> lType,
                                                            &hwndClient) ;

                            // frame owner is the container' frame window
                            WinSetOwner( hwndFrame, hwnd) ;

                            // resize it and show it
                            WinSetWindowPos(    hwndFrame, HWND_TOP,
                                                0L, 0L, 200L, 300L,
                                                SWP_SIZE | SWP_SHOW | SWP_ZORDER) ;

                            // set record emphasis
                            WinSendMsg( CLIENT( hwnd), CM_SETRECORDEMPHASIS,
                                        MPFROMP( prec),
                                        MPFROM2SHORT( TRUE, CRA_CURSORED)) ;
                        }
                            break ;

                        case CN_CONTEXTMENU:
                        {
                            PRECORDCORE prec ;
                            PAPPREC papprec ;
                            POINTL ptl ;
                            HPS hps ;

                            // get the address of the RECORDCORE struct of the selected object
                            prec = (PRECORDCORE)mp2 ;

                            // get the mouse position in desktop coordinates
                            WinQueryPointerPos( HWND_DESKTOP, &ptl) ;

                            // if NULL show the window context menu
                            if( !prec)
                            {
                                // query window rect
                                WinQueryWindowRect( CLIENT( hwnd), &rcEmph) ;

                                WinSendMsg( CLIENT( hwnd), CM_QUERYVIEWPORTRECT,
                                            MPFROMP( &rcEmph),
                                            MPFROM2SHORT( CMA_WINDOW, TRUE)) ;

                                WinInflateRect( HAB( hwnd), &rcEmph, -5L, -5L) ;

                                hps = WinGetPS( CLIENT( hwnd)) ;
                                GpiSetMix( hps, FM_INVERT) ;
                                GpiSetColor( hps, CLR_PALEGRAY) ;
                                GpiSetCurrentPosition( hps, (PPOINTL)&rcEmph + 1) ;
                                hRound = vRound = 30L ;
                                GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rcEmph, hRound, vRound) ;

                                //WinDrawBorder( hps, &rc, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_STANDARD) ;
                                WinReleasePS( hps) ;

                                WinPopupMenu(   HWND_DESKTOP, hwnd,
                                                hpopup,
                                                ptl.x, ptl.y,
                                                0,
                                                PU_NONE | PU_HCONSTRAIN | PU_VCONSTRAIN |
                                                PU_MOUSEBUTTON2 | PU_MOUSEBUTTON1 | PU_KEYBOARD) ;
                            }
                            else
                            {
                                QUERYRECORDRECT qrecrc ;

                                // get the pointer to the APPREC struct
                                papprec = (PAPPREC)prec ;

                                // query the obj position
                                qrecrc.cb = sizeof( qrecrc) ;
                                qrecrc.pRecord = prec ;
                                qrecrc.fRightSplitWindow = TRUE ; // window is not split
                                qrecrc.fsExtent = CMA_ICON ;

                                WinSendMsg( CLIENT( hwnd), CM_QUERYRECORDRECT,
                                            MPFROMP( &rcEmph),
                                            MPFROMP( &qrecrc)) ;

                                // enlarge the rectangle
                                WinInflateRect( HAB( hwnd), &rcEmph, 5L, 5L) ;

                                hps = WinGetPS( CLIENT( hwnd)) ;
                                GpiSetCurrentPosition( hps, (PPOINTL)&rcEmph + 1) ;
                                hRound = vRound = 20L ;
                                GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rcEmph, hRound, vRound) ;
                                WinReleasePS( hps) ;

                                // show the object menu
                                WinPopupMenu(   HWND_DESKTOP, hwnd,
                                                papprec -> hpopup,
                                                ptl.x, ptl.y,
                                                0,
                                                PU_NONE | PU_HCONSTRAIN | PU_VCONSTRAIN |
                                                PU_MOUSEBUTTON2 | PU_MOUSEBUTTON1 | PU_KEYBOARD) ;

                                // pass the object ID to the container window
                                WinSendMsg( CLIENT( hwnd), WM_PASSOBJ,
                                            MPFROMP( prec),
                                            0L) ;
                            }
                         }
                            break ;

                        case CN_SETFOCUS:
                        {
                            HPS hps ;

                            if( rcEmph.yTop)
                            {
                                hps = WinGetPS( CLIENT( hwnd)) ;
                                GpiSetMix( hps, FM_INVERT) ;
                                GpiSetColor( hps, CLR_PALEGRAY) ;
                                GpiSetCurrentPosition( hps, (PPOINTL)&rcEmph + 1) ;
                                GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rcEmph, hRound, vRound) ;
                                WinReleasePS( hps) ;
                                rcEmph.yTop = 0L ;
                            }
                        }
                            break ;

                        default:
                            break ;
                    }
                    break ;

                default:
                    break ;
        }
            }
            break ;

        default:
            break ;
    }

    return (*pfnwp)( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY ClientWndProc( HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
    static PFNWP pfnwp ;
    static PRECORDCORE precObj ;

    switch( msg)
    {
        case WM_PASSPROC:
        {
            ULONG ulStyle ;
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
        }
            return 0L ;

        case WM_STARTUP:
        {
            PRECORDCORE prec ;
            CNRINFO cnrinfo ;

            // create a new record and get the pointer
            prec = CreateAnotherObj( hwnd, 1L, NULL, 0L) ;

            // create three objects
            CreateAnotherObj( hwnd, OBJECTS, prec, -1L) ;

            // filling the CNRINFO structure
            cnrinfo.cb = sizeof( cnrinfo) ;
            cnrinfo.pszCnrTitle = "Multiple view container" ;
            cnrinfo.flWindowAttr = CA_TITLESEPARATOR | CA_CONTAINERTITLE ;

            // passing the data to the container window
            if( !WinSendMsg(    hwnd, CM_SETCNRINFO,
                                (MPARAM)&cnrinfo,
                                MPFROMLONG( CMA_CNRTITLE | CMA_FLWINDOWATTR)))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        }
            break ;

        case WM_PASSOBJ:
            precObj = (PRECORDCORE)mp1 ;
            return 0L ;

        case WM_CLOSE:
        {
            PRECORDCORE prec ;
            prec = (PRECORDCORE)WinSendMsg( hwnd, CM_QUERYRECORD,
                                            NULL, MPFROM2SHORT( CMA_FIRST, CMA_ITEMORDER)) ;
            if( WinSendMsg( hwnd, CM_REMOVERECORD,
                            MPFROMP( prec),
                            MPFROM2SHORT( 0, CMA_FREE)) == 0)
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_TREEVIEW:
                {
                    CNRINFO cnrinfo ;

                    // query the container
                    WinSendMsg( hwnd, CM_QUERYCNRINFO,
                                MPFROMP( &cnrinfo), MPFROMSHORT( sizeof( CNRINFO))) ;

                    // set the Tree view
                    cnrinfo.flWindowAttr =  CV_TREE | CV_ICON | CA_TREELINE |
                                            CA_TITLESEPARATOR | CA_CONTAINERTITLE ;

                    WinSendMsg( hwnd, CM_SETCNRINFO,
                                MPFROMP( &cnrinfo),
                                MPFROMLONG( CMA_FLWINDOWATTR | CMA_CNRTITLE)) ;

                    WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                }
                    break ;

                case MN_ICONVIEW:
                {
                    CNRINFO cnrinfo ;

                    // query the container
                    WinSendMsg( hwnd, CM_QUERYCNRINFO,
                                MPFROMP( &cnrinfo), MPFROMSHORT( sizeof( CNRINFO))) ;

                    // set the Tree view
                    cnrinfo.flWindowAttr =  CV_ICON | CA_TITLESEPARATOR |
                                            CA_CONTAINERTITLE ; // | CA_OWNERPAINTBACKGROUND ;

                    WinSendMsg( hwnd, CM_SETCNRINFO,
                                MPFROMP( &cnrinfo),
                                MPFROMLONG( CMA_FLWINDOWATTR | CMA_CNRTITLE)) ;

                    WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                }
                    break ;

                case MN_DETAILVIEW:
                {
                    CNRINFO cnrinfo ;
                    static PFIELDINFO pFieldInfo, pFieldStart ;
                    FIELDINFOINSERT FieldInfoInsert ;
 
                    // query the container
                    WinSendMsg( hwnd, CM_QUERYCNRINFO,
                                MPFROMP( &cnrinfo), MPFROMSHORT( sizeof( CNRINFO))) ;

                    if( !pFieldStart)
                    {
                    pFieldStart = pFieldInfo = WinSendMsg(  hwnd, CM_ALLOCDETAILFIELDINFO,
                                                            MPFROMLONG( CNR_COLS), NULL) ;

                    pFieldInfo -> cb = sizeof( FIELDINFO) ;
                    pFieldInfo -> flData = CFA_BITMAPORICON | CFA_HORZSEPARATOR | CFA_CENTER | CFA_SEPARATOR ;
                    pFieldInfo -> flTitle = CFA_CENTER ;
                    pFieldInfo -> pTitleData = (PVOID)"Object" ;
                    pFieldInfo -> offStruct = FIELDOFFSET( APPREC, rec.hptrIcon) ;
                    pFieldInfo = pFieldInfo -> pNextFieldInfo ;

                    pFieldInfo -> cb = sizeof( FIELDINFO) ;
                    pFieldInfo -> flData = CFA_STRING | CFA_HORZSEPARATOR | CFA_CENTER | CFA_SEPARATOR ;
                    pFieldInfo -> flTitle = CFA_CENTER ;
                    pFieldInfo -> pTitleData =  (PVOID)"Name" ;
                    pFieldInfo -> offStruct = FIELDOFFSET( APPREC, rec.pszText) ;
                    pFieldInfo = NULL ;

                    // empty the object
                    (void) memset( &FieldInfoInsert, 0, sizeof( FIELDINFOINSERT)) ;

                    FieldInfoInsert.cb = (ULONG)(sizeof(FIELDINFOINSERT)) ;
                    FieldInfoInsert.pFieldInfoOrder = (PFIELDINFO)CMA_FIRST ;
                    FieldInfoInsert.cFieldInfoInsert = (SHORT)CNR_COLS ;
                    FieldInfoInsert.fInvalidateFieldInfo = TRUE ;

                    WinSendMsg( hwnd, CM_INSERTDETAILFIELDINFO,
                                MPFROMP( pFieldStart),    
                                MPFROMP( &FieldInfoInsert)) ;
                    }

                    // set the Details view
                    cnrinfo.cb = sizeof( CNRINFO) ;
                    cnrinfo.flWindowAttr =  CV_DETAIL | CA_TITLESEPARATOR | CA_CONTAINERTITLE |
                                            CA_DETAILSVIEWTITLES ;
                    cnrinfo.pFieldInfoLast = pFieldInfo ;
                    cnrinfo.xVertSplitbar  = 150 ;

                    WinSendMsg( hwnd, CM_SETCNRINFO,
                                MPFROMP( &cnrinfo),
                                MPFROMLONG( CMA_FLWINDOWATTR | CMA_CNRTITLE)) ;
                }
                    break ;

                case MN_DEFAULT:
                {
                    PAPPREC papprec ;

                    papprec = (PAPPREC)precObj ;

                    CreateAnotherObj( hwnd, 1,  papprec -> precParent, papprec -> lType) ;

                    // clear the pointer
                    precObj = NULL ;
                }
                    break ;

                case MN_RGBOBJ:
                {
                    PRECORDCORE prec ;

                    prec = CreateAnotherObj( hwnd, 1L, NULL, 0L) ;

                    // create three objects
                    CreateAnotherObj( hwnd, OBJECTS, prec, -1L) ;
                }
                    break ;

                default:
                    break ;
            }
                break ;

            default:
                break ;
    }
    return (* pfnwp)( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY RedWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            WinFillRect( hps, &rc, CLR_RED) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CLOSE:
            WinDestroyWindow( PAPA( hwnd)) ;
            return 0L ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY GreenWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            WinFillRect( hps, &rc, CLR_GREEN) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CLOSE:
            WinDestroyWindow( PAPA( hwnd)) ;
            return 0L ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY BlueWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            WinFillRect( hps, &rc, CLR_BLUE) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CLOSE:
            WinDestroyWindow( PAPA( hwnd)) ;
            return 0L ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
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
PRECORDCORE CreateAnotherObj( HWND hwnd, short sObjNum, PRECORDCORE precParent, LONG lType)
{
    PAPPREC papprec ;
    RECORDINSERT recins ;
    PRECORDCORE prec, precstart ;
    LONG j = 0, inc = 0 ;
    CHAR szString[ 30], *psz ;

    // allocate a RECORDCORE structure
    papprec = (PAPPREC)WinSendMsg(  hwnd, CM_ALLOCRECORD,
                                    MPFROMLONG( sizeof( APPREC) - sizeof( RECORDCORE)),
                                    MPFROMSHORT( sObjNum)) ;

    prec = precstart = (PRECORDCORE)papprec ;
 
    // filling the RECORDINSERT structure
    recins.cb = sizeof( RECORDINSERT) ;
    recins.pRecordParent = (PRECORDCORE)precParent ;

    // root objs don't have a parent
    if( precParent)
    {
        recins.pRecordOrder = (PRECORDCORE)CMA_END ;
        recins.zOrder = CMA_BOTTOM ;
    }
    else
    {
        recins.pRecordOrder = (PRECORDCORE)CMA_FIRST ;
        recins.zOrder = CMA_TOP ;
    }
    recins.fInvalidateRecord = TRUE ;
    recins.cRecordsInsert = sObjNum ;

    inc = ( precParent) ? 1 : 0 ;

    do
    {
        // store the ID
        papprec -> lType = lType ;
        if( lType < 0L)
            papprec -> lType = j + inc ;

        // load string
        WinLoadString( HAB( hwnd), NULLHANDLE, OBJECTID + papprec -> lType, sizeof( szString), szString) ;
        psz = strdup( szString) ;

        // filling the RECORDCORE structure
        prec -> flRecordAttr = CRA_CURSORED | CRA_DROPONABLE ;
        prec -> ptlIcon.x = 0L ;
        prec -> ptlIcon.y = 0L ;
        prec -> pszIcon = psz ;
        prec -> hptrIcon = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, OBJECTID + papprec -> lType) ;
        prec -> hptrMiniIcon = NULLHANDLE ;
        prec -> hbmBitmap = NULLHANDLE ;
        prec -> hbmMiniBitmap = NULLHANDLE ;
        prec -> pTreeItemDesc = NULL ;
        prec -> pszText = psz ;
        prec -> pszName = psz ;
        prec -> pszTree = psz ;

        // store the parent pointer
        papprec -> precParent = precParent ;

        // loading the window context menu
        papprec -> hpopup = WinLoadMenu( HWND_DESKTOP, NULLHANDLE, RS_RGBMENU + inc) ;
        // assign the RS_COLORS ID
        WinSetWindowUShort( papprec -> hpopup, QWS_ID, RS_RGBMENU + inc) ;
        // add the mini-pushbuttons
        SetConditionalMenu( papprec -> hpopup, PM_HELP) ;
        SetConditionalMenu( papprec -> hpopup, PM_CREATEAN) ;

        // move to the following data area of type RECORDCODE
        prec = prec -> preccNextRecord ;

        // update the pointer
        papprec = (PAPPREC)prec ;

        // increase the counter
        j++ ;
    } while( prec) ;

    // pass all the data to the container window
    if( !WinSendMsg(    hwnd, CM_INSERTRECORD,
                        (MPARAM)precstart,
                        (MPARAM)&recins))
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;

    return precstart ;
}
