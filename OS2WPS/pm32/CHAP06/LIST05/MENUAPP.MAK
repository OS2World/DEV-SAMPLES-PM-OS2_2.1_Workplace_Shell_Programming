# IBM Developer's Workframe/2 Make File Creation run at 20:56:54 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP06\LIST05;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: MENUAPP.EXE \
     MENUAPP.RES

MENUAPP.EXE:  \
  MENUAPP.OBJ \
  MENUAPP.RES \
  MENUAPP.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"MENUAPP.EXE" MENUAPP.DEF 
MENUAPP.OBJ
<<
   RC MENUAPP.RES MENUAPP.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include MENUAPP.DEP
