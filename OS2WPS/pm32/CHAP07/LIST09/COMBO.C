// COMBO.C - WC_COMBOBOX

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include "combo.h"


#define LIMITX      200
#define LIMITY      180
#define OFFSET      60
#define ID_COMBOBOX 1
#define COMBOWIDTH  160

// macros
#define SYS( x)         WinQuerySysValue( HWND_DESKTOP, x)
#define DISPL           (SYS( SV_CYSIZEBORDER) + SYS( SV_CYTITLEBAR) + SYS( SV_CYMENU))
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define MENU( x)        WinWindowFromID( x, FID_MENU)

// function prototypes
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
int main( void) ;


int main( void)
{
    CHAR szClassName[] = "COMBO" ;
    CHAR szWindowTitle[] = "WC_COMBOBOX" ;
    HWND hwndFrame, hwndClient ;
    HAB hab ;
    HMQ hmq ;
    QMSG qmsg;
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_TASKLIST & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE,
                                    RS_ALL,
                                    &hwndClient) ;

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0, 0))
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
    static HWND hwndComboBox ;
    static SWP swp ;
    static LONG clr ;

    switch( msg)
    {
        case WM_PAINT:
        {
            HPS hps ;
            RECTL rc ;

            hps = WinBeginPaint( hwnd, 0L, &rc) ;
            WinFillRect( hps, &rc, clr + 1) ;
            WinEndPaint( hps) ;
        }
            break ;

        case WM_CREATE:
        {
            ULONG i ;
            CHAR *szClr[] = { "Blue", "Red", "Pink"} ;

            hwndComboBox = WinCreateWindow( hwnd, WC_COMBOBOX,
                                            NULL,
                                            CBS_DROPDOWNLIST,
                                            0L, 0L, 0L, 0L,
                                            hwnd, HWND_TOP,
                                            ID_COMBOBOX,
                                            NULL, NULL) ;
            for( i = 0; i < 3; i++)
            {
                WinSendMsg( hwndComboBox, LM_INSERTITEM,
                            MPFROM2SHORT( LIT_END, 0),
                            MPFROMP( szClr[ i])) ;
            }
        }
            break ;

        case WM_WINDOWPOSCHANGED:
        {
            PSWP pswpnew, pswpold ;

            pswpnew = PVOIDFROMMP( mp1) ;
            pswpold = pswpnew + 1 ;
            if( ( pswpnew -> cx < LIMITX) || ( pswpnew -> cy < LIMITY))
            {
                WinSetWindowPos(    PAPA( hwnd), HWND_TOP,
                                    swp.x, swp.y,
                                    pswpold -> cx + 2 * SYS( SV_CXSIZEBORDER),
                                    pswpold -> cy + DISPL + SYS( SV_CYSIZEBORDER),
                                    SWP_NOADJUST | SWP_SIZE | SWP_MOVE) ;
                return 0L ;
            }
        }
            break ;

        case WM_SIZE:
            WinQueryWindowPos( PAPA( hwnd), &swp) ;

            WinSetWindowPos(    hwndComboBox, HWND_TOP,
                                40L, SHORT2FROMMP( mp2) - OFFSET - DISPL,
                                COMBOWIDTH, 100L,
                                SWP_MOVE | SWP_SIZE) ;
            break ;

        case WM_COMMAND :
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_SHOW:
                    WinShowWindow( hwndComboBox, TRUE) ;
                    break ;

                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                    break ;
            }
                break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case ID_COMBOBOX:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case CBN_LBSELECT:
                        {
                            clr = (LONG)WinSendMsg( hwndComboBox, LM_QUERYSELECTION,
                                                    MPFROMSHORT( LIT_FIRST), 0L) ;
                            WinInvalidateRect( hwnd, NULL, TRUE) ;
                        }
                            break ;

                        default:
                            break ;
                    }
                        break ;

                default:
                    break ;
            }
                break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
