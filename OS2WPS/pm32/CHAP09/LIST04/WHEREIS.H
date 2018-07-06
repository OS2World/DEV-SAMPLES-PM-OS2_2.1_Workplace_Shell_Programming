// WHEREIS.H
// Listing 09-04

// macros
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define CLIENT( x)      WinWindowFromID( x, FID_CLIENT)
#define CTRL( x, y)     WinWindowFromID( x, y)

#define ID_DRIVE	1
#define ID_CURDIR	2
#define ID_DIR		3
#define ID_EDIT 	4
#define ID_SEARCH	5
#define ID_QUIT 	6
#define ID_CANCEL	7
#define ID_STARTUP	8
#define ID_DEL		9
#define ID_DELALL	10
#define ID_FOUND	11
#define ID_CONFIRM	12


#define RS_ALL          300
#define RS_ICON         RS_ALL
#define RS_ACCELTABLE   RS_ALL

#define WM_SHOWITEMS WM_USER

typedef struct _DATA
{
    HWND hwnd ;
    CHAR szString[ 100] ;
    USHORT usTot ;
} DATA ;

typedef DATA * PDATA ;

