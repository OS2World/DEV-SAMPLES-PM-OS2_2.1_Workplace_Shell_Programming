// ODLIST.H
// Listing 07-07

#define IDLB 258


#define RS_ALL          300
#define RS_ICON         RS_ALL
#define RS_MENU         RS_ALL

#define MN_TOP              100
#define MN_DIALOG           101
#define MN_EXIT             102

// STRINGTABLE definitions
#define ST_CLASSNAME    500
#define ST_WINDOWTITLE  501
#define ST_CHILDCLASS   502


// define new data types
typedef struct _DATA
{
    BOOL fStatus ;
    COLOR clr ;
} DATA ;

typedef DATA * PDATA ;

