# IBM Developer's Workframe/2 Make File Creation run at 00:16:44 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP09\LIST01;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: MENU.EXE \
     MENU.RES

MENU.EXE:  \
  MENU.OBJ \
  MENU.RES \
  MENU.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"MENU.EXE" MENU.DEF 
MENU.OBJ
<<
   RC MENU.RES MENU.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Gh /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Gh /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Gh /Ti /G4 /Gs /C   .\$*.cxx

!include MENU.DEP
