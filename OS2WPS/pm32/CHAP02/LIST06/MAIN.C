// MAIN.C - An OS/2 application

// only the main() function

int main( void)
{
        HAB hab ;
        HMQ         hmq ;
        HWND        hwndFrame, hwndClient ;
        QMSG        qmsg ;
        ULONG flFrameFlags =    FCF_STANDARD &
                                                ~FCF_MENU &
                                                ~FCF_ICON &
                                                ~FCF_ACCELTABLE ;

        ULONG flFrameStyle = WS_VISIBLE ;
        CHAR szClassName[] = "Machine" ;


        hab = WinInitialize( 0) ;
        hmq = WinCreateMsgQueue( hab, 0) ;

        WinRegisterClass(       hab,
                                        szClassName,
                                        ClientWndProc,
                                        0L,
                                        0) ;

        hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                                                                        flFrameStyle,
                                                &flFrameFlags,
                                                szClassName,
                                                "MACHINE Program",
                                                0L,
                                                0,
                                                0,
                                                &hwndClient) ;

        while( WinGetMsg( hab, &qmsg, NULL, 0, 0))
                WinDispatchMsg( hab, &qmsg) ;

        WinDestroyMsgQueue( hmq) ;
        WinDestroyWindow( hwndFrame) ;
        WinTerminate( hab) ;

        return 0 ;
}
