// client.h
// Listing 11-03

#include "client.hhh"

#define ID_STOCKPROC	256
#define ID_LIST 		300
#define ID_TEXT			301
#define ID_STATS		302
#define ID_FIX			303

#define MN_LINK         700

#define LISTWIDTH       90
#define GRAPHSTART		100
#define STEP			10
#define WINDOWHEIGHT	18
#define STATSWIDTH		300


// resource definitions
#define RS_ALL      300
#define RS_ICON     RS_ALL
#define RS_MENU     RS_ALL


// STRINGTABLE definitions
#define ST_CLASSNAME    1000
#define ST_WINDOWTITLE  1001
#define ST_STOCK        1003

typedef struct _TOPICS
{
	HWND hwnd ;
	PCHAR ptr ;
	CHAR szStock[ 10] ;
	SHORT sTitleID ;
} TOPICS ;

typedef TOPICS * PTOPICS ;
