// MIECC.H - Include file for MIECC.C
// Listing 10-09

// Function prototypes

void EXPENTRY InitTweny( void) ;
MRESULT EXPENTRY TwenyWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) ;


// notification codes
#define TN_CHANGE	1

// class specific messagges
#define TM_QUERYCOLOR WM_USER
