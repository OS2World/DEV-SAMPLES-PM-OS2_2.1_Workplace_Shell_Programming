// RTMENU.H
// Listing 06-10


#define RS_ALL          300
#define RS_ICON         RS_ALL
#define RS_MENU         RS_ALL

#define ST_CLASSNAME    100
#define ST_WINDOWTITLE  101

#define NUMBER 3
#define MN_FIRST 1001

#define MENUSTRINGSIZE 7

typedef struct _MITEM
{
  USHORT afStyle ;
  USHORT afAttribute ;
  USHORT id ;
  char	 text[ MENUSTRINGSIZE] ; // should only be the size of each text string
} MITEM ;

typedef MITEM *PMITEM ;

typedef struct _MENUCREATETEMPLATE
{
  USHORT size ;
  USHORT version ;
  USHORT codepage ;
  USHORT mnemonic ;
  USHORT itemcount ;
} MENUCREATETEMPLATE ;

typedef MENUCREATETEMPLATE *PMENUCREATETEMPLATE ;

