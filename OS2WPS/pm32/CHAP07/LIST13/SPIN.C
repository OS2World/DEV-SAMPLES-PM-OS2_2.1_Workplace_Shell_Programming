// SPIN.C - Spin control
// Listing 07-13

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_DOS
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "spin.h"

// macro definitions
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define HAB( x)         WinQueryAnchorBlock( x)
#define MENU( x)        WinWindowFromID( x, FID_MENU)
#define CTRL( x, y)     WinWindowFromID( x, y)
#define CT_SPIN         1000
#define CT_BTN          1001
#define CT_TXT          1002
#define CT_CHARS        1003
#define CT_SERVANT      1004

#define UPPER       100L
#define LOWER       0L
#define STATES      13

#define FCF_WPS FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX


// function prototypesint main( void) ;
VOID FileOpen(HWND hwnd, MPARAM mp2) ;
MRESULT EXPENTRY TemplateOpenFilterProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
BOOL StringParser( char *) ;

int main( void)
{
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 20] ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_WPS ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load class name from resource file
    WinLoadString(  hab, NULLHANDLE, ST_CLASSNAME,
                    sizeof( szClassName), szClassName) ;
    WinLoadString(  hab, NULLHANDLE, ST_WINDOWTITLE,
                    sizeof( szWindowTitle), szWindowTitle) ;

    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
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
    static PCH *pchStart ;

    switch( msg)
    {
        case WM_CREATE:
        {
            HWND hwndSpin, hwndBtn, hwndTxt, hwndMstr, hwndEntry, hwndFirst ;
            LONG i = FIRSTSTATE, l, j = 0L ;
            PCH pch ;
            APIRET rc ;
            ULONG ulID ;

            // numeric spinbutton
            hwndSpin = WinCreateWindow( hwnd, WC_SPINBUTTON,
                                        NULL,
                                        SPBS_MASTER | SPBS_NUMERICONLY | SPBS_JUSTRIGHT | WS_VISIBLE,
                                        10, 10, 100, 30,
                                        hwnd,
                                        HWND_TOP, CT_SPIN,
                                        NULL, NULL) ;

            hwndFirst = hwndSpin ;

            // set limits
            WinSendMsg( hwndSpin, SPBM_SETLIMITS, (MPARAM)UPPER, (MPARAM)LOWER) ;
            // set initial value
            WinSendMsg( hwndSpin, SPBM_SETCURRENTVALUE, MPFROMLONG( LOWER), NULL) ;
            // get the spin arrow handle
            hwndMstr = WinWindowFromID( hwndSpin, 0L) ;

            // alphanumeric spinbutton
            hwndSpin = WinCreateWindow( hwnd, WC_SPINBUTTON,
                                        NULL,
                                        SPBS_MASTER | SPBS_READONLY | SPBS_JUSTLEFT | WS_VISIBLE,
                                        10, 80, 200, 30,
                                        hwnd,
                                        HWND_TOP, CT_CHARS,
                                        NULL, NULL) ;
                       
            // allocate a page
            rc = DosAllocMem( (PPVOID)&pch, 4096L, PAG_READ | PAG_WRITE | PAG_COMMIT) ;
            if( rc)
                WinPostMsg( hwnd, WM_QUIT, 0L, 0L) ;

            // save starting position
            pchStart = (PCH *)pch ;
            // save room for STATES pointers
            pch += STATES * sizeof( PCH) ;
                                                              
            while( ( l = WinLoadString( HAB( hwnd), NULLHANDLE, i++, 20L, pch)))
            {
                // save the address
                *( pchStart + j) = pch ;
                // move the pointer
                pch += l + 1 ;
                j++ ;
            }

            // set limits
            WinSendMsg( hwndSpin, SPBM_SETARRAY, MPFROMP( pchStart), MPFROMSHORT( STATES)) ;
            // set initial value
            WinSendMsg( hwndSpin, SPBM_SETCURRENTVALUE, MPFROMLONG( 0L), NULL) ;

            // servant spinbutton
            hwndSpin = WinCreateWindow( hwnd, WC_SPINBUTTON,
                                        NULL,
                                        SPBS_SERVANT | SPBS_READONLY | SPBS_JUSTRIGHT | WS_VISIBLE,
                                        10, 140, 80, 30,
                                        hwnd,
                                        HWND_TOP, CT_SERVANT,
                                        NULL, NULL) ;
            // set limits
            WinSendMsg( hwndSpin, SPBM_SETLIMITS, (MPARAM)UPPER, (MPARAM)LOWER) ;
            // set initial value
            WinSendMsg( hwndSpin, SPBM_SETCURRENTVALUE, MPFROMLONG( LOWER), NULL) ;

            // get the spinbutton ID
            ulID = (ULONG)WinQueryWindowUShort( hwndSpin, QWS_ID) ;
            // get the entryfield hwnd
            hwndEntry = WinWindowFromID( hwndSpin, ulID) ;

            if( hwndEntry == hwndSpin && !hwndEntry)
                WinAlarm( HWND_DESKTOP, WA_NOTE) ;

            // set the master
            if( !WinSendMsg( hwndSpin, SPBM_SETMASTER, MPFROMHWND( hwndFirst), 0L))
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;

            // button
            hwndBtn = WinCreateWindow(  hwnd, WC_BUTTON,
                                        "Query value ->",
                                        BS_PUSHBUTTON | BS_DEFAULT | WS_VISIBLE,
                                        140, 10, 170, 30,
                                        hwnd,
                                        HWND_TOP, CT_BTN,
                                        NULL, NULL) ;
            // static
            hwndTxt = WinCreateWindow(  hwnd, WC_STATIC,
                                        NULL,
                                        SS_TEXT | WS_VISIBLE,
                                        312, 8, 170, 30,
                                        hwnd,
                                        HWND_TOP, CT_TXT,
                                        NULL, NULL) ;
        }
            break ;

        case WM_CLOSE:
        {
            APIRET rc ;

            rc = DosFreeMem( pchStart) ;
            if( rc)
                WinAlarm( HWND_DESKTOP, WA_ERROR) ;
        }
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case CT_SPIN:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case SPBN_UPARROW:
                        {
                            ULONG ulData ;

                            WinSendMsg( (HWND)mp2, SPBM_QUERYVALUE,
                                        (MPARAM)&ulData,
                                        MPFROM2SHORT( 0, SPBQ_UPDATEIFVALID)) ;
                        }
                            break ;
                    }
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

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                    break ;

                case CT_BTN:
                {
                    char szString[ 30] ;
                    HWND hwndSpin, hwndTxt ;

                    hwndSpin = CTRL( hwnd, CT_SPIN) ;

                    WinSendMsg( hwndSpin, SPBM_QUERYVALUE,
                                MPFROMP( szString),
                                MPFROM2SHORT( sizeof( szString), SPBQ_DONOTUPDATE)) ;
                    hwndTxt = CTRL( hwnd, CT_TXT) ;
                    WinSetWindowText( hwndTxt, szString) ;
                 }
                    break ;
            }

        default:
             break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}

