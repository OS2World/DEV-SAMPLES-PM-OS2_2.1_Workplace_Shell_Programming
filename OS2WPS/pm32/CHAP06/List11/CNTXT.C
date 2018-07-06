// CNTXT.C - Window context menu
// Listing 06-11

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdio.h>
#include "cntxt.h"

// macros
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define MENU( x)    WinWindowFromID( x, FID_MENU)
#define HAB( x)     WinQueryAnchorBlock( x)

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
BOOL SetConditionalMenu( HWND hmenu, USHORT msg) ;
int main( void) ;

int main( void)
{
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 25] ;
    HWND hwndFrame, hwndClient ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg;
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_SYSMENU ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;


    // load class name from resource file
    WinLoadString(  hab,
                    NULLHANDLE,
                    ST_CLASSNAME,
                    sizeof( szClassName),
                    szClassName) ;

    WinLoadString(  hab,
                    NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof( szWindowTitle),
                    szWindowTitle) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

     hwndFrame = WinCreateStdWindow(    HWND_DESKTOP,
                                        WS_VISIBLE,
                                        &flFrameFlags,
                                        szClassName,
                                        szWindowTitle,
                                        0L,
                                        NULLHANDLE,
                                        RS_ALL,
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
    static HWND hpopup ;
    static BOOL fPopup ;

    switch( msg)
    {
        case WM_CREATE:
        {
            ULONG ulStyle ;
            MENUITEM mi ;
            HWND hmenu ;

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

            // loading the popup menu from its template
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
            break ;

        case WM_PAINT:
        {
            HPS hps ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CONTEXTMENU:
        {
            POINTL ptl ;
            RECTL rc ;
            HPS hps ;

            // check if it is SHIFT+F10
            if( !mp1)
            {
                WinQueryPointerPos( HWND_DESKTOP, &ptl) ;
            }
            else
            {
                ptl.x = SHORT1FROMMP( mp1) ;
                ptl.y = SHORT2FROMMP( mp1) ;

                // converting the mouse position in HWND_DESKTOP coordinates
                WinMapWindowPoints( hwnd, HWND_DESKTOP, &ptl, 1) ;
            }

            // query the window rect
            WinQueryWindowRect( hwnd, &rc) ;
            WinInflateRect( HAB( hwnd), &rc, -5L, -5L) ;

            hps = WinGetPS( hwnd) ;
            // set upper right corner
            GpiSetCurrentPosition( hps, (PPOINTL)&rc + 1) ;

            GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rc, 20L, 20L) ;
            WinReleasePS( hps) ;

            // the menu is on
            fPopup = TRUE ;

            WinPopupMenu(   HWND_DESKTOP,
                            PAPA( hwnd),
                            hpopup,
                            ptl.x,
                            ptl.y,
                            0,
                            PU_NONE | PU_HCONSTRAIN | PU_VCONSTRAIN |
                            PU_MOUSEBUTTON2 | PU_MOUSEBUTTON1 | PU_KEYBOARD) ;
        }
            return (MPARAM)TRUE ;

        case WM_SETFOCUS:
            // check if we are getting the focus
            if( SHORT1FROMMP( mp2) && fPopup)
            {
                HPS hps ;
                RECTL rc ;

                WinQueryWindowRect( hwnd, &rc) ;
                // remove the window context menu emphazis
                WinInvalidateRect( hwnd, NULL, FALSE) ;
                break ;


                hps = WinGetPS( hwnd) ;
                WinDrawBorder( hps, &rc, 1L, 1L, CLR_WHITE, CLR_WHITE, DB_STANDARD) ;
                WinReleasePS( hps) ;
                fPopup = FALSE ;
            }
            break ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_COPY:
                case MN_UNDO:
                case MN_CUT:
                case MN_PASTE:
                case MN_CLEAR:
                case MN_PRODINFO:
                    WinAlarm( HWND_DESKTOP, WA_ERROR) ;
                    break ;

                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                    break ;
            }
                break ;

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

