// WHERE.C
// Listing 09-04

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_DOSPROCESS

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "whereis.h"

#ifdef API
void _System search( PDATA pData)
#else
void search( PDATA pData)
#endif
{
    HAB hab ;
    HMQ hmq ;
    DATA Data ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    schdir( pData, "\\") ;
    DosBeep( 175, 300) ;

    WinTerminate( hab) ;
    WinDestroyMsgQueue( hmq) ;
}

void schdir( PDATA pData, char * dirname)
{
    HDIR hdir = (HDIR)HDIR_CREATE ;
    ULONG ulcnt = 1 ;
    FILEFINDBUF3 fileFind ;

    DosSetCurrentDir( dirname) ;
    schfile( pData) ;

    if( !DosFindFirst(  "*.*",
                        &hdir,
                        FILE_NORMAL | FILE_DIRECTORY,
                        &fileFind,
                        sizeof( FILEFINDBUF3),
                        &ulcnt,
                        1L))
    {
        do
        {
            if( ( fileFind.attrFile & FILE_DIRECTORY) && ( fileFind.achName[ 0] != '.'))
            {
                schdir( pData, fileFind.achName) ;
                DosSetCurrentDir( "..") ;
            }
        } while( DosFindNext( hdir, &fileFind, sizeof( FILEFINDBUF3), &ulcnt) == 0) ;
    }
    DosFindClose( hdir) ;
}

void schfile( PDATA pData)
{
    HDIR hdir = (HDIR)HDIR_CREATE ;
    ULONG ulcnt = 1 ;
    FILEFINDBUF3 fileFind ;

    if( !DosFindFirst(  pData -> szString,
                        &hdir,
                        FILE_NORMAL,
                        &fileFind,
                        sizeof( FILEFINDBUF3),
                        &ulcnt,
                        1L))
    {
        ULONG ulDrv ;
        ULONG ulDrvNum ;
        CHAR buffer[ 100], dbuf[ 80] ;
        ULONG ulLen = sizeof( dbuf) ;
        int l ;

        DosQueryCurrentDisk( &ulDrvNum, &ulDrv) ;
        DosQueryCurrentDir( 0, dbuf, &ulLen) ;

        if( strlen( dbuf) != 0)
            strcat( dbuf, "\\") ;

        // Prepare the directory spec and sets l to the end of string
        sprintf( buffer, "%c:\\%s", ulDrvNum + 'a' - 1, strlwr( dbuf)) ;
        l = strlen( buffer) ;

        do
        {
            // Append the found filename
            strcpy( buffer + l, strlwr( fileFind.achName)) ;
            WinSendMsg( pData -> hwnd, LM_INSERTITEM,
                        MPFROMSHORT( LIT_END), MPFROMP( buffer)) ;

            ++pData -> usTot ;

        } while( DosFindNext( hdir, &fileFind, sizeof( FILEFINDBUF3), &ulcnt) == 0) ;
    }
    DosFindClose( hdir) ;
}
