// SYSDLLS.C
// Listing 10-08

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_GPICONTROL
#define INCL_DOS

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "sysdlls.h"

// definitions and macros
#define FCF_WPS     FCF_HIDEMAX | FCF_STANDARD & ~FCF_MINMAX


#define KEY_NAME    "LoadPerProcess"

#define CTRL( x, y)     WinWindowFromID( x, y)
#define SYS( x)         WinQuerySysValue( HWND_DESKTOP, x)

// function prototypes
int main( void) ;
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


int main( void)
{
    HAB hab ;
    HMQ hmq ;
    HWND hwndFrame, hwndClient ;
    QMSG qmsg ;
    ULONG flFrameFlags = FCF_BORDER | FCF_WPS & ~FCF_MENU & ~FCF_SIZEBORDER ;
    CHAR szClassName[ 10] ;
    CHAR szWindowTitle[ 30]  ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    // load the class name
    WinLoadString( hab, NULLHANDLE, ST_CLASSNAME, sizeof( szClassName), szClassName) ;
    // load the class name
    WinLoadString( hab, NULLHANDLE, ST_WINDOWTITLE, sizeof( szWindowTitle), szWindowTitle) ;

    if( !WinRegisterClass(  hab, szClassName,
                            ClientWndProc,
                            CS_SIZEREDRAW, 0L))
        WinAlarm( HWND_DESKTOP, WA_ERROR) ;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &flFrameFlags,
                                    szClassName,
                                    szWindowTitle,
                                    0L,
                                    NULLHANDLE,
                                    RS_ALL,
                                    &hwndClient) ;

    WinSetWindowPos(    hwndFrame, HWND_TOP,
                        SYS( SV_CXSCREEN) / 2 - WIDTH / 2,
                        SYS( SV_CYSCREEN) / 2 - HEIGHT / 2,
                        WIDTH, HEIGHT,
                        SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER) ;

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
            HWND hwndCtl ;
            PCHAR pBuffer ;
            ULONG length ;
            LONG i = 0L, j = 0L ;
            CHAR szText[ 50] ;

            // output
            hwndCtl = WinCreateWindow(  hwnd, WC_STATIC,
                                        NULL,
                                        WS_VISIBLE | SS_TEXT,
                                        10, 10, WIDTH * .8, 20,
                                        hwnd, HWND_TOP,
                                        CT_ACTION,
                                        NULL, NULL) ;

            // key name
            hwndCtl = WinCreateWindow(  hwnd, WC_ENTRYFIELD,
                                        NULL,
                                        WS_VISIBLE | ES_MARGIN,
                                        12, 120, 200, 22,
                                        hwnd, HWND_TOP,
                                        CT_KEY,
                                        NULL, NULL) ;
            // set the focus
            WinSetFocus( HWND_DESKTOP, hwndCtl) ;

            // listbox
            hwndCtl = WinCreateWindow(  hwnd, WC_LISTBOX,
                                        NULL,
                                        WS_VISIBLE,
                                        250, 40, 200, 170,
                                        hwnd, HWND_TOP,
                                        CT_LIST,
                                        NULL, NULL) ;

            // read strings in OS2.INI
            PrfQueryProfileSize( HINI_PROFILE, "SYS_DLLS", KEY_NAME, &length) ;
            DosAllocMem( (PPVOID)&pBuffer, length, PAG_READ | PAG_WRITE | PAG_COMMIT) ;
            PrfQueryProfileString( HINI_PROFILE, "SYS_DLLS", KEY_NAME, "", pBuffer, length) ;

            // fill the listbox
            while( i < length)
            {
                while( *( pBuffer + i) != ' ' && i <= length)
                    *( szText + j++) = *( pBuffer + i++) ;
                *( szText + j) = '\0' ;

                // insert the string
                WinSendMsg( hwndCtl, LM_INSERTITEM, MPFROMSHORT( LIT_END), MPFROMP( szText)) ;

                // reset j
                j = 0L ;
                // increment i
                i++ ;

            }
            // free memory
            DosFreeMem( pBuffer) ;

            // add button
            hwndCtl = WinCreateWindow(  hwnd, WC_BUTTON,
                                        "~Add",
                                        WS_VISIBLE | BS_PUSHBUTTON,
                                        10, 40, 90, 65,
                                        hwnd, HWND_TOP,
                                        CT_ADD,
                                        NULL, NULL) ;
            // remove button
            hwndCtl = WinCreateWindow(  hwnd, WC_BUTTON,
                                        "~Remove",
                                        WS_VISIBLE | BS_PUSHBUTTON,
                                        110, 40, 90, 65,
                                        hwnd, HWND_TOP,
                                        CT_REMOVE,
                                        NULL, NULL) ;
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
                case CT_ADD:
                {
                    ULONG length ;
                    PSZ buffer ;
                    CHAR szDLLName[ 200] ;
                    CHAR szText[ 200] ;

                    // get the DLL name
                    WinQueryWindowText( CTRL( hwnd, CT_KEY), sizeof( szDLLName), szDLLName) ;
                    // skip if empty
                    if( !*szDLLName)
                        break ;

                    // convert to uppercase
                    strupr( szDLLName) ;

                    // get the buffer dimension
                    PrfQueryProfileSize( HINI_PROFILE, "SYS_DLLS", KEY_NAME, &length) ;
                    // allocate a chunck of memory
                    DosAllocMem(    (PPVOID)&buffer, length + strlen( szDLLName) + 2,
                                    PAG_READ | PAG_WRITE | PAG_COMMIT);
                    // read
                    PrfQueryProfileString( HINI_PROFILE, "SYS_DLLS", KEY_NAME, "", buffer, length);

                    if (!strstr( buffer, szDLLName))
                    {
                        strcat( buffer, " ");
                        strcat( buffer, szDLLName);

                        PrfWriteProfileString(HINI_PROFILE,"SYS_DLLS", KEY_NAME, buffer);

                        // update the listbox
                        WinSendMsg( CTRL( hwnd, CT_LIST), LM_INSERTITEM,
                                    MPFROMSHORT( LIT_END), MPFROMP( szDLLName)) ;

                        sprintf( szText, "%s added to %s", szDLLName, KEY_NAME) ;
                        WinSetWindowText( CTRL( hwnd, CT_ACTION), szText) ;
                    }
                    else
                    {
                        sprintf( szText, "%s already in %s", szDLLName, KEY_NAME) ;
                        WinSetWindowText( CTRL( hwnd, CT_ACTION), szText) ;
                    }
                    DosFreeMem(buffer);
               }
                    break ;

                case CT_REMOVE:
                {
                    PSZ pszarg ;
                    PSZ pszmove;
                    CHAR szDLLName[ 200] ;
                    ULONG length ;
                    PSZ buffer ;
                    CHAR szText[ 200] ;

                    // get the DLL name
                    WinQueryWindowText( CTRL( hwnd, CT_KEY), sizeof( szDLLName), szDLLName) ;
                    // skip if empty
                    if( !*szDLLName)
                        break ;

                    // get the buffer dimension
                    PrfQueryProfileSize( HINI_PROFILE, "SYS_DLLS", KEY_NAME, &length) ;
                    // allocate a chunck of memory
                    DosAllocMem(    (PPVOID)&buffer, length + strlen( szDLLName) + 2,
                                    PAG_READ | PAG_WRITE | PAG_COMMIT);
                    // read
                    PrfQueryProfileString( HINI_PROFILE, "SYS_DLLS", KEY_NAME, "", buffer, length);

                    // convert to uppercase
                    strupr( szDLLName) ;

                    if( pszarg = strstr( buffer, szDLLName))
                    {
                        SHORT sPos ;

                        pszmove = pszarg - 1;
                        if (pszmove < buffer)
                            pszmove = buffer;

                        while (*pszarg && *pszarg != ' ')
                            pszarg++;

                        if (pszmove == buffer && *pszarg)
                            while (*pszarg && *pszarg == ' ')
                                pszarg++;

                        strcpy(pszmove, pszarg);

                        PrfWriteProfileString(HINI_PROFILE,"SYS_DLLS", KEY_NAME, buffer);

                        // get the DLL position in the listbox
                        sPos = (SHORT)WinSendMsg(   CTRL( hwnd, CT_LIST), LM_SEARCHSTRING,
                                                    MPFROM2SHORT( LSS_CASESENSITIVE, LIT_FIRST),
                                                    MPFROMP( szDLLName)) ;

                        // update the listbox
                        WinSendMsg( CTRL( hwnd, CT_LIST), LM_DELETEITEM,
                                    MPFROMSHORT( sPos), 0L) ;

                        sprintf( szText, "%s removed from %s", szDLLName, KEY_NAME) ;
                        WinSetWindowText( CTRL( hwnd, CT_ACTION), szText) ;

                    }
                    else
                    {
                        sprintf( szText, "%s not in %s", szDLLName, KEY_NAME) ;
                        WinSetWindowText( CTRL( hwnd, CT_ACTION), szText) ;
                    }
                }
                    break ;

            }
            break ;

        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1))
            {
                case CT_KEY:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case EN_SETFOCUS:
                            // erase the content of the CT_ACTION window
                            WinSetWindowText( CTRL( hwnd, CT_ACTION), NULL) ;
                            // erase the content of the CT_KEY window
                            WinSetWindowText( CTRL( hwnd, CT_KEY), NULL) ;
                            break ;
                    }
                    break ;

                case CT_LIST:
                    switch( SHORT2FROMMP( mp1))
                    {
                        case LN_SETFOCUS:
                            WinSetWindowText( CTRL( hwnd, CT_ACTION), NULL) ;
                            break ;

                        case LN_SELECT:
                        {
                            SHORT sPos ;
                            CHAR szWindowText[ 50] ;

                            sPos = (SHORT)WinSendMsg(   (HWND)mp2, LM_QUERYSELECTION,
                                                        MPFROMSHORT( LIT_FIRST), 0L) ;
                            WinSendMsg( (HWND)mp2, LM_QUERYITEMTEXT,
                                        MPFROM2SHORT( sPos, sizeof( szWindowText)),
                                        MPFROMP( szWindowText)) ;

                            // set the text in the entryfield
                            WinSetWindowText( CTRL( hwnd, CT_KEY), szWindowText) ;
                        }
                            break ;
                    }
                    break ;
            }
            break ;

        default:
            break ;
    }
    return WinDefWindowProc( hwnd, msg, mp1, mp2) ;
}
