# IBM Developer's Workframe/2 Make File Creation run at 08:26:07 on 07/27/93

# Make File Creation run in directory:
#   F:\PM32\CHAP13\LIST04;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: PMEDIT.EXE \
     PMEDIT.RES

PMEDIT.EXE:  \
  GOTO.OBJ \
  MIE02.LIB \
  PMEDIT.OBJ \
  PMEDIT.RES \
  SEARCH.OBJ \
  PMEDIT.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"PMEDIT.EXE" MIE02.LIB PMEDIT.DEF 
GOTO.OBJ 
MIE02.LIB 
PMEDIT.OBJ 
SEARCH.OBJ
<<
   RC PMEDIT.RES PMEDIT.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include PMEDIT.DEP
