# IBM Developer's Workframe/2 Make File Creation run at 21:48:06 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST02;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: BUTTONS.EXE \
     BUTTONS.RES

BUTTONS.EXE:  \
  BUTTONS.OBJ \
  BUTTONS.RES \
  BUTTONS.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"BUTTONS.EXE" BUTTONS.DEF 
BUTTONS.OBJ
<<
   RC BUTTONS.RES BUTTONS.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include BUTTONS.DEP
