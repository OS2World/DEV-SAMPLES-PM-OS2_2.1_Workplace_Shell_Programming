// MENU.H

#define ID_MENU		9
#define ID_CLOSE	10
#define ID_LOAD		11
#define ID_SAVE 	12

#define RS_ALL          300
#define RS_ICON         RS_ALL
#define RS_MENU         RS_ALL

#define ID_PROJECT	1
#define DL_EDIT 2

#define ID_RES	4

// max # of menuitem per drop-down
#define TOTITEM         40


#define MN_FAKE 1000

#define MN_START        500

#define MN_ONE          400
#define MN_UNO          401
#define MN_MENU         399


typedef struct _DATA
{
    char szFileName[ 20] ;
    HWND hwnd ;
} DATA ;

typedef DATA  *PDATA ;
