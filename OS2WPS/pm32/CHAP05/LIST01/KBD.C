// KBD.C - Keyboard input
// Listing 05-01

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL
#include <os2.h>
#include <stdio.h>
#include <string.h>

// definitions
#define KC_CODES    13

#define FCF_WPS         FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd , ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS & ~FCF_MENU & ~FCF_ICON & ~FCF_ACCELTABLE ;
    CHAR szClassName[] = "KBD" ;
    CHAR szWindowTitle[] = "Keyboard input" ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

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
                                    0L,
                                    &hwndClient) ;

    WinSendMsg( hwndFrame, WM_SETICON,
                (MPARAM)WinQuerySysPointer( HWND_DESKTOP, SPTR_APPICON, FALSE),
                NULL) ;

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
    static CHAR szString[ 60] ;
    static USHORT usKC ;
    static CHAR szCode[][ 50] = { "KC_CHAR",
                                "KC_VIRTUALKEY",
                                "KC_SCANCODE",
                                "KC_SHIFT",
                                "KC_CTRL",
                                "KC_ALT",
                                "KC_KEYUP",
                                "KC_PREVDOWN",
                                "KC_LONEKEY",
                                "KC_DEADKEY",
                                "KC_COMPOSITE",
                                "KC_INVALIDCOMP" 
                                "KC_TOGGLE",
                                "KC_INVALIDCHAR" } ;

    switch( msg)
    {
        case WM_CHAR:
        {
            if( CHARMSG(&msg)->fs & KC_KEYUP)
            {
                sprintf(    szString, "Codice ASCII %c = %d\nCodice di scansione %d",
                                CHARMSG( &msg) -> chr,CHARMSG( &msg) -> chr,
                                CHARMSG( &msg) -> scancode) ;


                usKC = CHARMSG( &msg) -> fs ;
                WinInvalidateRect( hwnd, NULL, FALSE) ;
            }
        }
            break ;

        case WM_PAINT:
        {
            HPS hps ;
            POINTL ptl ;
            ULONG j = KC_CODES ;
            USHORT i = 0x2000 ;

            hps = WinBeginPaint( hwnd, NULLHANDLE, NULL) ;
            GpiErase( hps) ;

            if( usKC)
            {
                do
                {
                    if( usKC >= i)
                    {
                        switch( i)
                        {
                            case KC_CHAR:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_VIRTUALKEY:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_SCANCODE:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_SHIFT:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_ALT:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_CTRL:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_KEYUP:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_PREVDOWN:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_LONEKEY:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_DEADKEY:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_COMPOSITE:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_INVALIDCOMP:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_TOGGLE:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;

                            case KC_INVALIDCHAR:
                                ptl.x = 50L ;
                                ptl.y = 300L - 30L * j ;
                                GpiCharStringAt( hps, &ptl, strlen( szCode[ j]), szCode[ j]) ;
                                break ;
                        }
                        usKC -= i ;
                    }
                    i >>= 1 ;
                    j-- ;
                } while( i <= 0x2000 && usKC) ;
            }
            WinEndPaint( hps) ;
        }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
