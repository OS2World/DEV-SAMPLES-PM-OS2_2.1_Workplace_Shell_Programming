// WHERE.C
// Listing 09-04

// Stefano Maruzzi 1993

#define INCL_WIN
#define INCL_DOSPROCESS

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "whereis.h"

void APIENTRY search( PDATA pData) ;
void schdir( PDATA pData, char * dirname) ;
void schfile( PDATA pData) ;


void APIENTRY search( PDATA pData)
{
    ULONG ulDrv ;
    ULONG ulDrvNum ;
    HAB hab ;
    HMQ hmq ;

    hab = WinInitialize( 0) ;
    hmq = WinCreateMsgQueue( hab, 0L) ;

    DosQueryCurrentDisk( &ulDrvNum, &ulDrv) ;
    schdir( pData, "\\") ;
    DosBeep( 175, 300) ;

    // enable the search button and the drive listbox
    WinEnableWindow( CTRL( pData -> hwnd, ID_SEARCH), TRUE) ;
    WinEnableWindow( CTRL( pData -> hwnd, ID_DRIVE), TRUE) ;

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
            if( ( fileFind.attrFile & FILE_DIRECTORY) && (fileFind.achName[ 0] != '.'))
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
    char dbuf[ 80] ;
    ULONG ulLen = sizeof( dbuf) ;
    FILEFINDBUF3 fileFind ;
    ULONG ulDrv ;
    ULONG ulDrvNum ;
    CHAR buffer[ 100] ;

    if( !DosFindFirst(  pData -> szString,
                        &hdir,
                        FILE_NORMAL,
                        &fileFind,
                        sizeof( FILEFINDBUF3),
                        &ulcnt,
                        1L))
    {
        do
        {
            DosQueryCurrentDir( 0, dbuf, &ulLen) ;
            if( strlen( dbuf) != 0)
                strcat( dbuf, "\\") ;

            DosQueryCurrentDisk( &ulDrvNum, &ulDrv) ;
            sprintf( buffer, "%c:\\%s%s", ulDrvNum + 'a' - 1, strlwr( dbuf), strlwr( fileFind.achName)) ;
            WinSendMsg( CTRL( pData -> hwnd, ID_DIR), LM_INSERTITEM,
                        MPFROMSHORT( LIT_END), MPFROMP( buffer)) ;
            sprintf( buffer, "%u", ++pData -> usTot) ;
            WinSetWindowText( CTRL( pData -> hwnd, ID_FOUND), buffer) ;

        } while( DosFindNext( hdir, &fileFind, sizeof( FILEFINDBUF3), &ulcnt) == 0) ;
    }
    DosFindClose( hdir) ;
}
