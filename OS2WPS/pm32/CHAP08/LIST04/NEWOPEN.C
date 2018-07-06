// NEWOPEN.C - Predefined open dialog
// Listing 08-04

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "newopen.h"

// macro definitions
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define MENU( x)    WinWindowFromID( x, FID_MENU)
#define CTRL( x, y) WinWindowFromID( x, y)
#define SYS( x)     WinQuerySysValue( HWND_DESKTOP, x)

#define FILEOPEN        2000

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX
#define CT_BTN      200

// function prototypes
int main( void) ;
VOID FileOpen( HWND hwnd) ;
VOID FontBox( HWND hwnd) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ProdInfoDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 35] ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS ;

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

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_OPEN:
                {
                    FileOpen( hwnd) ;
 
                    WinSendMsg( MENU( PAPA( hwnd)), MM_SETITEMATTR,
                                MPFROM2SHORT( MN_SAVE, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;

                    WinSendMsg( MENU( PAPA( hwnd)), MM_SETITEMATTR,
                                MPFROM2SHORT( MN_SAVEAS, TRUE),
                                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED)) ;
                }
                    break ;

                case MN_FONT:
                    FontBox( hwnd) ;
                    break ;

                case MN_PRODINFO:
                    WinDlgBox( HWND_DESKTOP, hwnd, ProdInfoDlgProc, NULLHANDLE, DL_PRODINFO, NULL);
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

VOID FileOpen( HWND hwnd)
{
    FILEDLG fdg ;
    CHAR szTitle[ 40] = "Open", szButton[ 40] = "Open" ;

    fdg.cbSize = sizeof( FILEDLG) ;
    fdg.pszTitle = szTitle ;
    fdg.pszOKButton = szButton ;
    fdg.ulUser = 0L ;
    fdg.fl = FDS_HELPBUTTON | FDS_CENTER | FDS_OPEN_DIALOG ;
    fdg.pfnDlgProc = NULL ;
    fdg.lReturn = 0L ;
    fdg.lSRC = 0L ;
    fdg.hMod = 0 ;
    fdg.usDlgId = FILEOPEN ;
    fdg.x = 0 ;
    fdg.y = 0 ;

    strcpy( fdg.szFullFile, "*.*") ;

    fdg.pszIType = NULL ;
    fdg.papszITypeList = NULL ;
    fdg.pszIDrive = NULL ;
    fdg.papszIDriveList= NULL ;
    fdg.sEAType = 0 ;
    fdg.papszFQFilename= NULL ;
    fdg.ulFQFCount = 0L ;

   // get the file
   if(  !WinFileDlg( HWND_DESKTOP, PAPA( hwnd), &fdg))
      return ;
}


VOID FontBox( HWND hwnd)
{
    static FONTDLG fontDlg ;
    CHAR szString[ 100]= "Hello World" ;
    HWND hwndTitlebar = CTRL( PAPA( hwnd), FID_TITLEBAR) ;
    HPS hps ;
    FONTMETRICS fontMetrics ;
    CHAR szTitle[ 100] = "Font" ;
    CHAR szFamily[ 100] ;
    static fxPointSize = 0 ;

    // memset( &fontDlg, 0, sizeof( fontDlg)) ;
    WinSendMsg( hwndTitlebar, MLM_QUERYFONT, MPFROMP( (PFATTRS)&( fontDlg.fAttrs)), NULL) ;

    hps = WinGetPS( hwndTitlebar) ;
    // create system default font
    GpiCreateLogFont(   hps,
                        (PSTR8)fontDlg.fAttrs.szFacename,
                        1, &(fontDlg.fAttrs)) ;

    GpiSetCharSet( hps, 1L) ;
    GpiQueryFontMetrics( hps, sizeof( FONTMETRICS), &fontMetrics) ;
    GpiSetCharSet( hps, LCID_DEFAULT) ;
    GpiDeleteSetId( hps, 1L) ;
    WinReleasePS( hps) ;

    // Initialize the FONTDLG structure with the current font
    fontDlg.cbSize = sizeof( FONTDLG) ;
    fontDlg.hpsScreen = WinGetScreenPS( HWND_DESKTOP) ;
    fontDlg.hpsPrinter = NULLHANDLE ;

    fontDlg.pszTitle = szTitle ;
    fontDlg.pszPreview = "Preview string" ;
    fontDlg.pszPtSizeList = NULL ;
    fontDlg.pfnDlgProc = NULL ;
    strcpy(szFamily, fontMetrics.szFamilyname) ;
    fontDlg.pszFamilyname = szFamily ;
    fontDlg.fxPointSize = fxPointSize ;
    fontDlg.fl = FNTS_CENTER | FNTS_INITFROMFATTRS | FNTS_CENTER ;
    fontDlg.flFlags = 0 ;
    fontDlg.flType = (LONG) fontMetrics.fsType ;
    fontDlg.flTypeMask = 0 ;
    fontDlg.flStyle = 0 ;
    fontDlg.flStyleMask = 0 ;
    fontDlg.clrFore = CLR_BLACK ;
    fontDlg.clrBack = CLR_WHITE ;
    fontDlg.ulUser = 0 ;
    fontDlg.lReturn = 0 ;
    fontDlg.lSRC ;
    fontDlg.lEmHeight = 0 ;
    fontDlg.lXHeight = 0 ;
    fontDlg.lExternalLeading = 0 ;
    fontDlg.hMod ;
    fontDlg.sNominalPointSize = fontMetrics.sNominalPointSize ;
    fontDlg.usWeight = fontMetrics.usWeightClass ;
    fontDlg.usWidth = fontMetrics.usWidthClass ;
    fontDlg.x = 0 ;
    fontDlg.y = 0 ;
    fontDlg.usDlgId = 0 ;
    fontDlg.usFamilyBufLen = sizeof( szFamily) ;
    fontDlg.fAttrs ;
 
    WinFontDlg( HWND_DESKTOP, hwnd, &fontDlg) ;

    // If a font was successfully selected use it
    WinReleasePS( fontDlg.hpsScreen) ;

}


MRESULT EXPENTRY ProdInfoDlgProc(   HWND hwnd,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
    switch( msg)
    {
        case WM_INITDLG:
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

