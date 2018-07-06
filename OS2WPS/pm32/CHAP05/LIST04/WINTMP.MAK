# IBM Developer's Workframe/2 Make File Creation run at 19:56:56 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP05\LIST04;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: WINTMP.EXE \
     WINTMP.RES

WINTMP.EXE:  \
  WINTMP.OBJ \
  WINTMP.RES \
  WINTMP.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"WINTMP.EXE" WINTMP.DEF 
WINTMP.OBJ
<<
   RC WINTMP.RES WINTMP.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include WINTMP.DEP
