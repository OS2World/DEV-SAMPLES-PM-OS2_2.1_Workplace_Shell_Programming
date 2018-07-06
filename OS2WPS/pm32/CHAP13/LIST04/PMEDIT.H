// PMEDIT.H
// Listing 13-04

#include "dialogs.h"

#define CXWIN   180
#define CYWIN   100



#define ID_ASSOCTABLE   3

#define CT_MLE          500
#define CT_LINE         501
#define CT_ENTRYFIELD   502
#define CT_LINES        503

// logo window dimension
#define WIDTH 220
#define HEIGHT 140

#define KEY_DOWN  0x8000

// macros
#define HAB( x)         WinQueryAnchorBlock( x)
#define PAPA( x)        WinQueryWindow( x, QW_PARENT)
#define OWNER( x)       WinQueryWindow( x, QW_OWNER)
#define GRANPA( x)      PAPA( PAPA( x))
#define CLIENT( x)      WinWindowFromID( x, FID_CLIENT)
#define MENU( x)        WinWindowFromID( x, FID_MENU)
#define SYS( x)         WinQuerySysValue( HWND_DESKTOP, x)
#define GETID( x)       WinQueryWindowUShort( PAPA( x), QWS_ID)
#define CTRL( x, y)     WinWindowFromID( x, y)

// DIL window height
#define STATICWINDOW 20

// notebook pages IDs
#define NB_SETTINGS_PAGE01      10001
#define NB_SETTINGS_PAGE02      10002
#define NB_SETTINGS_PAGE03      10003
#define NB_SETTINGS_PAGE04      10004

// window context menu definitions
#define PM_OPEN         400
#define PM_HELP         401
#define PM_CREATEAN     402
#define PM_COPY         403
#define PM_MOVE         404
#define PM_SHADOW       405
#define PM_DELETE       406
#define PM_CLOSE        407
#define PM_SENDIT       408


// offset for the documents
#define INFOAREA 30

// resources for MDI & child windows
#define RS_ALL          300
#define RS_ICON         RS_ALL
#define RS_MENU         RS_ALL
#define RS_ACCELTABLE   RS_ALL

#define RS_POPUP        150
#define RS_NEWSYS       151


// top level menus IDs
#define MN_FILE          81
#define MN_EDIT          82
#define MN_OPTIONS       83
#define MN_HELP          85
#define MN_WINDOW        86
#define MN_EDITOR        87

#define MN_CLOSE        105

// File menu
#define MN_NEW          110
#define MN_OPEN         111
#define MN_CLOSEDOC     112
#define MN_SAVE         113
#define MN_SAVEAS       114
#define MN_EXIT         115


#define MN_SETTINGS     160
#define MN_DEFAULT      161
#define MN_ICONVIEW     162


// EDIT menu
#define MN_UNDO        120
#define MN_REDO        121
#define MN_CUT         122
#define MN_COPY        123
#define MN_PASTE       124
#define MN_DELETE      125
#define MN_FIND        126
#define MN_SELECTALL   127
#define MN_DESELECTALL 128

// OPTIONS menu
#define MN_FONTS        141
#define MN_BACKGROUND   149
#define MN_FOREGROUND   156
#define MN_INFOAREA     158
#define MN_GOTO         157

// HELP menu
#define MN_HELP1        170
#define MN_HELP2        171
#define MN_HELP3        172
#define MN_HELP4        173
#define MN_PRODINFO     174



#define MN_EDITTITLE    200



// help IDs
#define HT_MAIN         170
#define XHT_MAIN        171
#define SHTP_WINDOW         172


typedef struct _NBKDATA
{
    HWND hwndClr ;
    HOBJECT hobjClr ;
    HWND hwndFnt ;
    HOBJECT hobjFnt ;
} NBKDATA ;

typedef NBKDATA * PNBKDATA ;


typedef struct _ATTRIBUTES
{
    COLOR clrOldText ;
    COLOR clrOldBack ;
    COLOR clrNewText ;
    COLOR clrNewBack ;
    LONG lAction ;
} ATTRIBUTES ;

typedef ATTRIBUTES * PATTRIBUTES ;
 
typedef struct _VALUES
{
    LONG l ;

} VALUES ;

typedef VALUES * PVALUES ;

typedef struct _MLEDATA
{
    HWND hwndNbk ;
    PATTRIBUTES pattrib ;
    PVALUES pval ;
} MLEDATA ;

typedef MLEDATA * PMLEDATA ;
