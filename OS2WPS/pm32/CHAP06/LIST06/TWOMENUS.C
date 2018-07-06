// TWOMENUS.C - Loading a second menu
// Listing 06-06

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "twomenus.h"

// definitions & macros
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define MENU( x)        WinWindowFromID( PAPA( x), FID_MENU)

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
BOOL StringParser( char *) ;

int main( void)
{
    CHAR szClassName[ 15] ;
    CHAR szWindowTitle[ 25] ;
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_STANDARD & ~FCF_ACCELTABLE ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load class name & window title from resource file
    WinLoadString(  hab, NULLHANDLE,
                    ST_CLASSNAME,
                    sizeof( szClassName),
                    szClassName) ;
    WinLoadString(  hab, NULLHANDLE,
                    ST_WINDOWTITLE,
                    sizeof( szWindowTitle),
                    szWindowTitle) ;

    // register class
    WinRegisterClass(   hab, szClassName,
                        ClientWndProc,
                        CS_SIZEREDRAW, 0L) ;

    // create main window
    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
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

    // destroy resources
    WinDestroyWindow (hwndFrame) ;
    WinDestroyMsgQueue (hmq) ;
    WinTerminate (hab) ;

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

        case WM_COMMAND:
            switch( COMMANDMSG( &msg) -> cmd)
            {
                case MN_NEW:
                {
                    // destroy the current menu
                    WinDestroyWindow( MENU( hwnd)) ;
                    WinLoadMenu( PAPA( hwnd), NULLHANDLE, RS_SECOND) ;

                    // update the frame
                    WinSendMsg( PAPA( hwnd), WM_UPDATEFRAME, MPFROMSHORT( FCF_MENU), 0L) ;
                }
                    break ;

                case MN_BACK:
                {
                    WinDestroyWindow( MENU( hwnd)) ;
                    WinLoadMenu( PAPA( hwnd), NULLHANDLE, RS_MENU) ;

                    WinSendMsg( PAPA( hwnd), WM_UPDATEFRAME, MPFROMSHORT( FCF_MENU), 0L) ;
                }
                    break ;

                default:
                {
                    char szBuffer[ 50], szString[ 80], szSource[ 50] ;

                    WinSendMsg( MENU( hwnd), MM_QUERYITEMTEXT,
                                MPFROM2SHORT( COMMANDMSG( &msg) -> cmd, sizeof szBuffer),
                                MPFROMP( szBuffer)) ;
                    StringParser( szBuffer) ;

                    // determine source
                    switch( SHORT1FROMMP( mp2))
                    {
                        case CMDSRC_ACCELERATOR:
                            strcpy( szSource, "accelerator") ;
                            break ;

                        case CMDSRC_MENU:
                            strcpy( szSource, "menu") ;
                            break ;

                            default:
                                strcpy( szSource, "other") ;
                                break ;
                    }

                    sprintf( szString, "Command: %s\nid: %3d\nsource:%s\ntype:%s",
                                    szBuffer, COMMANDMSG( &msg) -> cmd,
                                    szSource, (COMMANDMSG( &msg)-> fMouse) ? "mouse" : "keyboard") ;

                    WinMessageBox(  HWND_DESKTOP, hwnd,
                                    szString, "Message Received",
                                    0, MB_OK) ;
                }
                    break ;

                case MN_EXIT:
                    WinPostMsg( hwnd, WM_QUIT, NULL, NULL) ;
                    break ;
            }
                break ;

        default:
            break;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
BOOL StringParser( register char *szString)
{
    ULONG i = 0L ;
    CHAR szFinal[ 50], *psz = szString ;

    while( *szString)
    {
        if( *szString == '~')
        {
            szString++ ;
                continue ;
        }
        if( *szString == '\t')
            break ;
        *( szFinal + i++) = *szString++ ;
    }
    *( szFinal + i) = '\0' ;

    // copy the string
    if( strcpy( psz, szFinal))
        return TRUE ;

    return FALSE ;
}
