// WHERE.H
// Listing 13-03


typedef struct _WHERE
{
    HWND hwnd ;
    CHAR szString[ 100] ;
} WHERE ;

typedef WHERE * PWHERE ;


// function prototypes
void EXPENTRY search( PWHERE pWhere) ;
void schfile( PWHERE pWhere) ;


