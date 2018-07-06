// WHERE.C

// Stefano Maruzzi 1992

#define INCL_WIN

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "whereis.h"

void search( HWND hwnd, char *filename) ;
void schdir( HWND hwnd, char * dirname) ;
void schfile( HWND hwnd) ;


char sname[ 80] ;


void search( HWND hwnd, char *filename)
{
    ULONG ulDrv ;
    ULONG usDrv ;

    DosQueryCurrentDisk( &usDrv, &ulDrv) ;
    strcpy( sname, filename) ;
    schdir( hwnd, "\\") ;

}

void schdir( HWND hwnd, char * dirname)
{
    HDIR hdir = HDIR_CREATE ;
    ULONG scnt = 1 ;
    FILEFINDBUF3 fileFind ;

    DosSetCurrentDir( dirname) ;
    schfile( hwnd) ;

    if( !DosFindFirst(  "*.*",
                        &hdir,
                        FILE_NORMAL | FILE_DIRECTORY,
                        &fileFind,
                        sizeof( FILEFINDBUF3),
                        &scnt, 1L))
    {
        do
        {
            if( ( fileFind.attrFile & FILE_DIRECTORY) && (fileFind.achName[ 0] != '.'))
            {
                schdir( hwnd, fileFind.achName) ;
                DosChDir( "..") ;
            }
        } while( DosFindNext( hdir, &fileFind, sizeof( FILEFINDBUF3), &scnt) == 0) ;
    }
    DosFindClose( hdir) ;
}

void schfile( HWND hwnd)
{
    HDIR hdir = HDIR_CREATE ;
    ULONG scnt = 1 ;
    char dbuf[ 80] ;
    ULONG sLen = sizeof( dbuf) ;
    char szFile[ 100] ;
    FILEFINDBUF3 fileFind ;
    ULONG ulDrv ;
    ULONG usDrv ;

    if( !DosFindFirst(  sname,
                        &hdir,
                        FILE_NORMAL,
                        &fileFind,
                        sizeof( FILEFINDBUF3),
                        &scnt, 1L))
    {
        DosQueryCurrentDir( 0, dbuf, &sLen) ;
        if( strlen( dbuf) != 0)
        strcat( dbuf, "\\") ;

        do
        {
            DosQueryCurrentDisk( &usDrv, &ulDrv) ;
            sprintf( szFile, "%c:\\%s%s", usDrv + 'a' - 1, strlwr( dbuf), strlwr( fileFind.achName)) ;
            WinSendMsg( SON( hwnd, ID_DIR), LM_INSERTITEM,
                        MPFROMSHORT( LIT_END),
                        MPFROMP( (PSZ) szFile)) ;

        } while( DosFindNext( hdir, &fileFind, sizeof( FILEFINDBUF3), &scnt) == 0) ;
    }
    DosFindClose( hdir) ;
}
