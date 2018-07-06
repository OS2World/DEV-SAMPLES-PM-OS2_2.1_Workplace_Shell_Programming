// SEARCH.C - SearchBox di PMEDIT
// Listing 13-04

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_DOSFILEMGR

#include <os2.h>
#include "pmedit.h"


MRESULT EXPENTRY Search(	HWND hwnd,
							ULONG msg,
							MPARAM mp1,
							MPARAM mp2)
{
	switch( msg)
	{
		case WM_INITDLG:
		{
			HWND hwndClient ;

			hwndClient = *((PHWND)PVOIDFROMMP( mp2)) ;

			// centrare la dialog
//			WinCenterDlg( hwndClient, hwnd) ;
		}
			break ;

		case WM_COMMAND:
			switch( COMMANDMSG( &msg) ->cmd)
			{
				case DID_OK:
					WinDismissDlg( hwnd, TRUE) ;
					break ;

				case DID_CANCEL:
					WinDismissDlg( hwnd, FALSE) ;
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