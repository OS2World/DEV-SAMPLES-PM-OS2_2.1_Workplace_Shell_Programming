// WPSFLDR.H
// Listing 12-04

// container objs
#define OBJECTID    1000

#define RS_RGB      OBJECTID
#define RS_RED      (OBJECTID + 1)
#define RS_GREEN    (OBJECTID + 2)
#define RS_BLUE     (OBJECTID + 3)

// new data type

typedef struct APPREC
{
    RECORDCORE rec ;
    PRECORDCORE precParent ;
    LONG lType ;
    HWND hpopup ;
} APPREC ;

typedef APPREC * PAPPREC ;

#define RS_ALL          300
#define RS_ICON         RS_ALL
#define RS_MENU         RS_ALL
#define RS_ACCELTABLE   RS_ALL
#define RS_TBMENU       350
#define RS_RGBMENU      351
#define RS_COLORS       352

#define MN_FILE 99
// Defines for MN_FILE menu
#define MN_NEW  100
#define MN_OPEN 101
#define MN_SAVE 102
#define MN_SAVEAS 103
#define MN_EXIT 104

#define MN_EDIT 200
// Defines for MN_EDIT menu
#define MN_UNDO 201
#define MN_CUT  202
#define MN_COPY 203
#define MN_PASTE 204
#define MN_CLEAR 205

#define MN_HELP 300
#define MN_PRODINFO 301


// window context menu
#define PM_OPEN         500
#define PM_HELP         501
#define PM_CREATEAN     502
#define PM_COPY         503
#define PM_MOVE         504
#define PM_SHADOW       505
#define PM_DELETE       506


#define MN_SETTINGS     600
#define MN_ICONVIEW     601
#define MN_TREEVIEW     602
#define MN_DETAILVIEW   603

#define MN_HELP1        610
#define MN_HELP2        611
#define MN_HELP3        612
#define MN_HELP4        613

#define MN_DEFAULT      620
#define MN_EDITOR       621
#define MN_REDO         622
#define MN_DELETE       623
#define MN_FIND         624
#define MN_SELECTALL    625
#define MN_DESELECTALL  626
#define MN_WINDOW       627
#define MN_CREATEANOBJ  628
#define MN_RGBOBJ       629



