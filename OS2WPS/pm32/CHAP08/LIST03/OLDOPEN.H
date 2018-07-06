// OLDOPEN.H
// Listing 08-03

#define RS_ALL          300
#define RS_ICON         RS_ALL
#define RS_MENU         RS_ALL
#define RS_ACCELTABLE   RS_ALL
#define RS_BITMAP       RS_ALL



#define MN_FILE         99

// Defines for MN_FILE menu
#define MN_NEW          100
#define MN_OPEN         101
#define MN_SAVE         102
#define MN_SAVEAS       103
#define MN_EXIT         104
#define MN_PRODINFO     105
#define MN_HELP         106

#define MN_EDIT         200

// Defines for MN_EDIT menu
#define MN_UNDO         201
#define MN_CUT          202
#define MN_COPY         203
#define MN_PASTE        204
#define MN_CLEAR        205

typedef struct _DATA
{
    CHAR szCurrentPath[ 80] ;
    HWND hwndOwner ;
} DATA ;

typedef DATA *PDATA ;

// STRINGTABLE definitions
#define ST_CLASSNAME        10
#define ST_WINDOWTITLE      11


#define DL_OPEN             400
#define DL_PRODINFO         401
