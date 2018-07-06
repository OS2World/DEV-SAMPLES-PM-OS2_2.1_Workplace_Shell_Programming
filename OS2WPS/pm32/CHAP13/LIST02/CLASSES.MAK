# IBM Developer's Workframe/2 Make File Creation run at 15:37:30 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP13\LIST02;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: CLASSES.EXE \
     CLASSES.RES

CLASSES.EXE:  \
  CLASSES.OBJ \
  CLASSES.RES \
  MIE02.LIB \
  CLASSES.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"CLASSES.EXE" MIE02.LIB CLASSES.DEF 
CLASSES.OBJ 
MIE02.LIB
<<
   RC CLASSES.RES CLASSES.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include CLASSES.DEP
