# IBM Developer's Workframe/2 Make File Creation run at 21:19:57 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP06\LIST10;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: RTMENU.EXE \
     RTMENU.RES

RTMENU.EXE:  \
  RTMENU.OBJ \
  RTMENU.RES \
  RTMENU.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"RTMENU.EXE" RTMENU.DEF 
RTMENU.OBJ
<<
   RC RTMENU.RES RTMENU.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wextparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wextparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wextparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include RTMENU.DEP
