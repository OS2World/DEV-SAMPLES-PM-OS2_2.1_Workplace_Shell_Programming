// PRESPAR.C - A dialog window with presentation params
// Listing 08-01

// Stefano maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include "prespar.h"

// definitions & macros
#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

#define SYS( x)     WinQuerySysValue( HWND_DESKTOP, x)
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define MENU( x)    WinWindowFromID( x, FID_MENU)

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ProdInfoDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ComboDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    CHAR szClientClass[ 20] ;
    CHAR szWindowTitle[ 30] ;
    ULONG flFrameFlags = FCF_WPS ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg ;
    HWND hwndFrame, hwndClient ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    WinLoadString(  hab, NULLHANDLE, ST_CLASSNAME,
                    sizeof( szClientClass), szClientClass) ;
    WinLoadString(  hab, NULLHANDLE, ST_WINDOWTITLE,
                    sizeof( szWindowTitle), szWindowTitle) ;

    WinRegisterClass(   hab, szClientClass,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
									&flFrameFlags,
									szClientClass,
									szWindowTitle,
									NULLHANDLE,
									0L,
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
    switch( msg)
    {
        case WM_CREATE:
        {
            HWND hmenu ;
            CHAR szFont[] = "10.Helvetica Bold Italic" ;

            hmenu = MENU( PAPA( hwnd)) ;

            WinSetPresParam( hmenu, PP_FONTNAMESIZE, sizeof( szFont), szFont) ;

        }
            break ;

        case WM_COMMAND:
            switch (COMMANDMSG(&msg) -> cmd)
            {
                case MN_DIALOG:
                    WinDlgBox(  HWND_DESKTOP, hwnd,
                                ComboDlgProc,
                                NULLHANDLE, DL_PRES,
                                NULL) ;
                    break ;

                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;
                    break ;

				case MN_PRODINFO:
                    WinDlgBox( HWND_DESKTOP, hwnd, ProdInfoDlgProc, NULLHANDLE, DL_PRODINFO, &hwnd);
					break ;
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

		case WM_DESTROY:
			break ;
	}
	return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY ProdInfoDlgProc(   HWND hwnd,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
    static HWND hwndOwner ;

    switch( msg)
    {
        case WM_INITDLG:
        {
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

MRESULT EXPENTRY ComboDlgProc(  HWND hwnd,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
	switch( msg)
	{
		case WM_INITDLG:
		{
			HWND hwndListbox, hwndEnable, hwndEntry ;
			ULONG color ;
			ULONG  i ;
			CHAR FontName[] = "8.Helv" ;
			CHAR szString[ 80] ;

			hwndEnable = WinWindowFromID( hwnd, DL_ENABLE) ;
			hwndListbox = WinWindowFromID( hwnd, DL_LIST) ;

			for( i = 0; i < 5; i++)
			{
				sprintf( szString, "String #%d", i) ;
				WinSendMsg( hwndListbox, LM_INSERTITEM,
							MPFROMSHORT( LIT_END), MPFROMP( szString)) ;
			}

			WinSendMsg( hwndListbox, LM_SELECTITEM,
						MPFROMSHORT( 0),
						MPFROMSHORT( TRUE)) ;

			WinSetPresParam(	hwndListbox, PP_FONTNAMESIZE,
								sizeof( FontName), FontName) ;

			color = (LONG) 0x00ff0000 ;
			WinSetPresParam(	hwndListbox, PP_FOREGROUNDCOLOR,
								sizeof( color), &color) ;

			color = (LONG) CLR_BLACK;
			WinSetPresParam(	hwndListbox, PP_HILITEBACKGROUNDCOLORINDEX,
								sizeof( color), &color) ;

			color = (LONG) CLR_DARKBLUE ;
			WinSetPresParam(	hwndListbox, PP_BACKGROUNDCOLORINDEX,
								sizeof( color), &color) ;

			color = (LONG) CLR_YELLOW;
			WinSetPresParam(	hwndListbox, PP_HILITEFOREGROUNDCOLORINDEX,
								sizeof( color), &color) ;

			hwndEntry = WinWindowFromID( hwnd, DL_ENTRY) ;
			color = (LONG) CLR_RED ;
			WinSetPresParam(	hwndEntry, PP_FOREGROUNDCOLORINDEX,
								sizeof( color), &color) ;

			color = (LONG) CLR_BLUE ;
			WinSetPresParam(	hwndEntry, PP_HILITEFOREGROUNDCOLORINDEX,
								sizeof( color), &color) ;

			WinSetPresParam(	hwndEntry, PP_DISABLEDFOREGROUNDCOLORINDEX,
								sizeof( color), &color) ;
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
				case DL_ENABLE:
				{
					HWND hwndEntry, hwndDisable, hwndEnable ;

					hwndEntry = WinWindowFromID( hwnd, DL_ENTRY) ;
					WinEnableWindow( hwndEntry, TRUE) ;

					hwndEnable = WinWindowFromID( hwnd, DL_ENABLE);
					hwndDisable = WinWindowFromID( hwnd, DL_DISABLE);

					WinEnableWindow( hwndEnable, FALSE) ;
					WinEnableWindow( hwndDisable, TRUE) ;
				}
					return 0L ;

				case DL_DISABLE:
				{
					HWND hwndEntry, hwndDisable, hwndEnable ;

					hwndEntry = WinWindowFromID( hwnd, DL_ENTRY) ;
					WinEnableWindow( hwndEntry, FALSE) ;
					hwndEnable = WinWindowFromID( hwnd, DL_ENABLE) ;
					hwndDisable = WinWindowFromID( hwnd, DL_DISABLE) ;
					WinEnableWindow( hwndDisable, FALSE) ;
					WinEnableWindow( hwndEnable, TRUE) ;
				}
					return 0L ;

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