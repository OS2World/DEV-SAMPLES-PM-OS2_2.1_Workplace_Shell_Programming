// MIE02.H - Header file for MIE02.C


// dialog window data type
typedef struct _DLGDATA
{
    HWND hwndOwner ;
    CHAR szWindowTitle[ 30] ;
} DLGDATA ;

typedef DLGDATA * PDLGDATA ;

// function prototypes
COLOR EXPENTRY MieColorClient( HWND hwnd, HPS hps, PRECTL prc) ;
MRESULT EXPENTRY MieCenterDlg( HWND hwndClient, HWND hwndDlg) ;
BOOL EXPENTRY InitTweny( void) ;
MRESULT EXPENTRY TwenyWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
ULONG EXPENTRY MieDlgWnd( HWND hwndOwner, PDLGDATA pDlgData) ;
MRESULT EXPENTRY ProdInfoDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;
void EXPENTRY MieEmphasizeWindow( HWND hwnd, HPS hps, PRECTL prc) ;


// application definitions
#define RS_ALL          300
#define RS_ICON         RS_ALL
#define RS_MENU         RS_ALL
#define RS_ACCELTABLE   RS_ALL


// STRINGTABLE definitions
#define ST_CLASSNAME        1000

// DIALOG definitions
#define DL_PRODINFO         2000



