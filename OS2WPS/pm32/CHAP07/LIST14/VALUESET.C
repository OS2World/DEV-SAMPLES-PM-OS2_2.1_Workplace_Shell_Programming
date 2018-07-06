// VALUESET.C - Valueset
// Listing 07-14

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "valueset.h"

// macro definitions
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define MENU( x)        WinWindowFromID( x, FID_MENU)
#define CTRL( x, y)     WinWindowFromID( x, y)

#define CT_NUM      1000
#define CT_COL      1001
#define CT_VAR      1002

#define MAXCOL  2
#define MAXROW  2
#define ROWS    4
#define COLS    4

#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX


// prototipi di funzioni
int main( void) ;
VOID FileOpen(HWND hwnd, MPARAM mp2) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 15] ;
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
    static COLOR clr = CLR_WHITE ;

    switch( msg)
    {
        case WM_CREATE:
        {
            HWND hwndValueset ;
            VSCDATA vscd ;
            LONG i, j ;
            CHAR szString[ 30] ;
            HPS hps ;

            vscd.cbSize = sizeof vscd ;
            vscd.usRowCount = MAXROW ;
            vscd.usColumnCount = MAXCOL ;

            hwndValueset = WinCreateWindow( hwnd, WC_VALUESET,
                                            NULL,
                                            VS_TEXT | VS_BORDER | WS_VISIBLE,
                                            10, 10, 180, 90,
                                            hwnd, HWND_TOP, CT_NUM,
                                            (PVOID)&vscd, NULL) ;

            for( i = 1; i <= MAXROW; i++)
                for( j = 1; j <= MAXCOL; j++)
                {
                    // set attributes
                    WinSendMsg( hwndValueset, VM_SETITEMATTR,
                                MPFROM2SHORT( i, j), MPFROM2SHORT( VIA_TEXT, TRUE)) ;
                    sprintf( szString, "%ld - %ld", i, j) ;
                    // insert text
                    WinSendMsg( hwndValueset, VM_SETITEM,
                                MPFROM2SHORT( i, j), MPFROMP( szString)) ;
                }

            // Initialize the parameters in value set structure
            vscd.cbSize = sizeof( VSCDATA) ;
            vscd.usRowCount = ROWS ;
            vscd.usColumnCount = COLS ;

            // Create the value set control on the page
            hwndValueset = WinCreateWindow( hwnd, WC_VALUESET,
                                            NULL,
                                            VS_COLORINDEX | VS_ITEMBORDER | VS_BORDER | WS_VISIBLE,
                                            10, 130, 150, 150,
                                            hwnd, HWND_TOP, CT_COL,
                                            &vscd, NULL) ;

            for( i = 1; i <= ROWS; i++)
                for( j = 1; j <= COLS; j++)
                    WinSendMsg( hwndValueset, VM_SETITEM,
                                MPFROM2SHORT( i, j),
                                MPFROMLONG( (i - 1) * COLS + j)) ;

            vscd.cbSize = sizeof vscd ;
            vscd.usRowCount = MAXROW ;
            vscd.usColumnCount = MAXCOL ;

            hwndValueset = WinCreateWindow( hwnd, WC_VALUESET,
                                            NULL,
                                            VS_BORDER | WS_VISIBLE,
                                            200, 10, 180, 90,
                                            hwnd, HWND_TOP, CT_VAR,
                                            (PVOID)&vscd, NULL) ;

            // set attributes
            WinSendMsg( hwndValueset, VM_SETITEMATTR,
                        MPFROM2SHORT( 1, 1), MPFROM2SHORT( VIA_TEXT, TRUE)) ;
            // text item
            WinSendMsg( hwndValueset, VM_SETITEM,
                        MPFROM2SHORT( 1, 1),
                        MPFROMP( "Hello!")) ;

            // set attributes
            WinSendMsg( hwndValueset, VM_SETITEMATTR,
                        MPFROM2SHORT( 1, 2), MPFROM2SHORT( VIA_BITMAP, TRUE)) ;
            // bitmap item
            hps = WinGetPS( hwnd) ;
            WinSendMsg( hwndValueset, VM_SETITEM,
                        MPFROM2SHORT( 1, 2),
                        MPFROMLONG( GpiLoadBitmap( hps, NULLHANDLE, RS_BITMAP, 0L, 0L))) ;
            WinReleasePS( hps) ;

            // set attributes
            WinSendMsg( hwndValueset, VM_SETITEMATTR,
                        MPFROM2SHORT( 2, 1), MPFROM2SHORT( VIA_ICON, TRUE)) ;
            // icon item
            WinSendMsg( hwndValueset, VM_SETITEM,
                        MPFROM2SHORT( 2, 1),
                        MPFROMLONG( WinLoadPointer( HWND_DESKTOP, NULLHANDLE, RS_ICON))) ;

            // set attributes
            WinSendMsg( hwndValueset, VM_SETITEMATTR,
                        MPFROM2SHORT( 2, 2), MPFROM2SHORT( VIA_RGB, TRUE)) ;
            // RGB item
            WinSendMsg( hwndValueset, VM_SETITEM,
                        MPFROM2SHORT( 2, 2),
                        MPFROMLONG( 0x00aabbcc)) ;
        }
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, &rc) ;
            WinFillRect( hps, &rc, clr);
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case CT_COL:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case VN_ENTER:
                        {
                            USHORT usSelRow ;
                            USHORT usSelCol ;
                            MRESULT mr ;
                            HWND hwndCol = CTRL( hwnd, CT_COL) ;

                            mr = WinSendMsg( hwndCol, VM_QUERYSELECTEDITEM, NULL, NULL) ;

                            usSelCol = SHORT2FROMMR( mr) ;
                            usSelRow = SHORT1FROMMR( mr) ;

                            clr = (ULONG) WinSendMsg(   hwndCol, VM_QUERYITEM,
                                                        MPFROM2SHORT( usSelRow, usSelCol),
                                                        NULL) ;

                            WinInvalidateRect( hwnd, NULL, FALSE) ;
                        }
                            break ;

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
            }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

