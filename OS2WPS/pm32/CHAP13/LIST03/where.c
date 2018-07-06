// WHERE.C
// Listing 13-03

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_DOSPROCESS

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "where.h"


void EXPENTRY search( PWHERE pWhere)
{
    HAB hab ;
    HMQ hmq ;
    PTIB ptib ;
    PPIB ppib ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    DosGetInfoBlocks( &ptib, &ppib) ;

    // start searching
    schfile( pWhere) ;
    DosBeep( 175, 300) ;

    WinTerminate( hab) ;
    WinDestroyMsgQueue( hmq) ;
}


void schfile( PWHERE pWhere)
{
    HDIR hdir = HDIR_SYSTEM ;
    ULONG ulcnt = 1L ;
    CHAR szDir[ 80] ;
    ULONG ulLen = sizeof( szDir) ;
    FILEFINDBUF3 fileFind ;
    ULONG ulDrv ;
    ULONG ulDrvNum ;
    CHAR szString[ 100] ;

    if( !DosFindFirst(  pWhere -> szString,
                        &hdir,
                        FILE_NORMAL,
                        &fileFind,
                        sizeof( FILEFINDBUF3),
                        &ulcnt,
                        FIL_STANDARD))
    {
        do
        {
            DosQueryCurrentDir( 0, szDir, &ulLen) ;
            if( strlen( szDir))
                strcat( szDir, "\\") ;

            DosQueryCurrentDisk( &ulDrvNum, &ulDrv) ;
            sprintf( szString, "%c:\\%s%s", ulDrvNum + 64, szDir, strlwr( fileFind.achName)) ;
            WinSendMsg( pWhere -> hwnd, LM_INSERTITEM,
                        MPFROMSHORT( LIT_END), MPFROMP( szString)) ;

        } while( DosFindNext( hdir, &fileFind, sizeof( FILEFINDBUF3), &ulcnt) == 0) ;
    }
    DosFindClose( hdir) ;
}
