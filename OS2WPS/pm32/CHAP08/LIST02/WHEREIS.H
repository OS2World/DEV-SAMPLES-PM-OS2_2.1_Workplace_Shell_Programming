// WHEREIS.H
// Listing 08-02

#define RS_ALL          300
#define RS_ICON         RS_ALL
#define RS_ACCELTABLE   RS_ALL
#define RS_MENU         RS_ALL


// macros
#define PAPA( x)    WinQueryWindow( x, QW_PARENT)
#define CLIENT( x)  WinWindowFromID( x, FID_CLIENT)
#define SON( x, y)  WinWindowFromID( x, y)
#define CTRL( x, y) WinWindowFromID( x, y)


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

// dialog templates
#define DL_WINDOW   400
#define DL_CONFIRM  401
#define DL_PRODINFO 402

