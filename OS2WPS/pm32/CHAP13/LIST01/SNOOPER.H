// SNOOPER.H
// Listing 13-01

#include <os2.h>

#define RS_ALL      300
#define RS_ICON     RS_ALL

#define RS_NEWSYS   350
#define RS_SNOOP    400
#define RS_WINTREE  401

// button's context menus
#define MN_CLOSEALL     500
#define MN_MINIMIZEALL  501
#define MN_DEFTREE      502
#define MN_DESKTOP      503
#define MN_OBJECT       504

// window context menu definitions
#define MN_HELP             401
#define MN_CREATEANOTHER    402
#define MN_COPY             403
#define MN_MOVE             404
#define MN_SHADOW           405
#define MN_DELETE           406

// top level menus IDs
#define MN_FILE         81
#define MN_EDIT         82
#define MN_WINDOW       83

// File menu
#define MN_NEW          110
#define MN_OPEN         111
#define MN_SETTINGS     160
#define MN_DEFAULT      161
#define MN_ICONVIEW     162

// EDIT menu
#define MN_UNDO         120
#define MN_REDO         121
#define MN_CUT          122
#define MN_PASTE        124
#define MN_FIND         126
#define MN_SELECTALL    127
#define MN_DESELECTALL  128

// HELP menu
#define MN_HELP1        170
#define MN_HELP2        171
#define MN_HELP3        172
#define MN_HELP4        173
#define MN_COPYRIGHT    174

// STRINGATBLE
#define ST_CLASSNAME        1000
#define ST_SNOOPCLASS       1001
#define ST_WINDOWTITLE      1002
#define ST_DROPINFO         1003


typedef struct _SNOOP
{
    CHAR szClass[ 34] ;
    CHAR szText[ 34] ;
    HAB hab ;
    HMQ hmq ;
    CLASSINFO clsi ;
    HWND hwndOwner ;
    CHAR szOwnerClass[ 34] ;
    HWND hwndParent ;
    CHAR szParentClass[ 34] ;
    ULONG ulStyles ;
    USHORT usID ;
    HWND hwnd ;
    PFNWP pfnWndProc ;
    PID pid ;
    TID tid ;
    SWP swp ;
    POINTL ptl ;
    CHAR szModel[ 10] ;
    CHAR szhobj[ 25] ;
} SNOOP ;

typedef SNOOP * PSNOOP ;

typedef struct APPREC
{
    RECORDCORE rec ;
    PRECORDCORE precParent ;
    LONG lType ;
    HWND hpopup ;
    CHAR szString[ 30] ;
} APPREC ;

typedef APPREC * PAPPREC ;


