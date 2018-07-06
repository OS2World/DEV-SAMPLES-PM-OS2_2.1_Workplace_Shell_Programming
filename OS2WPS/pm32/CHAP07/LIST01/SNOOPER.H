// SNOOPER.H
// Listing 07-01

#define RS_ALL      300
#define RS_ICON     RS_ALL
#define RS_MENU     RS_ALL


#define ST_CLASSNAME    200
#define ST_WINDOWTITLE  201


typedef struct _SNOOP
{
    CHAR szClass[ 34] ;
    CHAR szText[ 34] ;
    HAB hab ;
    CLASSINFO clsi ;
    PFNWP pfnWndProc ;
    HWND hwndOwner ;
    CHAR szOwnerClass[ 34] ;
    HWND hwndParent ;
    CHAR szParentClass[ 34] ;
    ULONG ulStyles ;
    USHORT usID ;
    HWND hwnd ;
    PID pid ;
    TID tid ;
} SNOOP ;

typedef SNOOP * PSNOOP ;
