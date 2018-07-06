// BUTTONS.C - Show all the buttons
// Listing 07-02

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include "buttons.h"


// definitions
#define CT_PUSHBUTTON 1
#define CT_DEFPUSHBUTTON 2
#define CT_CHECKBOX 3
#define CT_RADIOBUTTON 4

// a more appropriate look
#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX


// macros
#define HAB( x)     WinQueryAnchorBlock( x)
#define CTRL( x, y) WinWindowFromID( x, y)


// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;

int main( void)
{
    HAB hab ;
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 25] ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load strings
    WinLoadString( hab, NULLHANDLE, ST_CLASSNAME, sizeof( szClassName), szClassName) ;
    WinLoadString( hab, NULLHANDLE, ST_WINDOWTITLE, sizeof( szWindowTitle), szWindowTitle) ;

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
                                    RS_ICON,
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
            HWND hwndButton ;
            HPOINTER hptr ;
            BTNCDATA btncdata ;
            COLOR clr = CLR_WHITE ;

            hptr = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, RS_USA_UP) ;

            btncdata.cb = (USHORT)sizeof( BTNCDATA) ;
            btncdata.fsCheckState = TRUE ;
            btncdata.fsHiliteState = TRUE ;
            btncdata.hImage = hptr ;

            hwndButton = WinCreateWindow(   hwnd, WC_BUTTON,
                                            "Steve",
                                            WS_VISIBLE | BS_PUSHBUTTON | BS_AUTOSIZE | BS_ICON,
                                            5, 5, 0, 0,
                                            hwnd, HWND_TOP,
                                            CT_PUSHBUTTON,
                                            &btncdata, NULL) ;
            WinSetPresParam( hwndButton, PP_BACKGROUNDCOLORINDEX, sizeof( clr), &clr) ;


            hwndButton = WinCreateWindow(   hwnd, WC_BUTTON,
                                            "DefPushbutton",
                                            WS_VISIBLE | BS_PUSHBUTTON | BS_DEFAULT | BS_AUTOSIZE,
                                            130, 5, 0, 0,
                                            hwnd, HWND_TOP,
                                            CT_DEFPUSHBUTTON,
                                            NULL, NULL) ;

            hwndButton = WinCreateWindow(   hwnd, WC_BUTTON,
                                            "Checkbox",
                                            WS_VISIBLE | BS_CHECKBOX | BS_AUTOSIZE,
                                            300, 5, 0, 0,
                                            hwnd, HWND_TOP,
                                            CT_CHECKBOX,
                                            NULL, NULL) ;

            hwndButton = WinCreateWindow(   hwnd, WC_BUTTON,
                                            "Radiobutton",
                                            WS_VISIBLE | BS_RADIOBUTTON | BS_AUTOSIZE,
                                            440, 5, 0, 0,
                                            hwnd, HWND_TOP,
                                            CT_RADIOBUTTON,
                                            NULL, NULL) ;
        }
            break ;

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case CT_PUSHBUTTON:
                {
                    static BOOL fState ;
                    WNDPARAMS wndprms ;
                    BTNCDATA btncdata ;
                    HPOINTER hptr ;

                    fState = !fState ;
                    hptr = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, RS_USA_UP + fState) ;

                    btncdata.cb = (USHORT)sizeof( BTNCDATA) ;
                    btncdata.fsCheckState = TRUE ;
                    btncdata.fsHiliteState = TRUE ;
                    btncdata.hImage = hptr ;

                    wndprms.fsStatus = WPM_CTLDATA ;
                    wndprms.cchText = 0L ;
                    wndprms.pszText = NULL ;
                    wndprms.cbPresParams = 0L ;
                    wndprms.pPresParams = NULL ;
                    wndprms.cbCtlData = (USHORT)sizeof( BTNCDATA) ;
                    wndprms.pCtlData = &btncdata ;

                    WinSendMsg( CTRL( hwnd, CT_PUSHBUTTON), WM_SETWINDOWPARAMS, MPFROMP( &wndprms), 0L) ;


                    WinAlarm( HWND_DESKTOP, WA_NOTE) ;
                }
                    break ;

                default:
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

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
