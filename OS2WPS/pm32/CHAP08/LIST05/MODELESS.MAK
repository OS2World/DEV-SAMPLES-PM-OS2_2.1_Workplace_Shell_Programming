# IBM Developer's Workframe/2 Make File Creation run at 23:53:29 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP08\LIST05;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: MODELESS.EXE \
     MODELESS.RES

MODELESS.EXE:  \
  MODELESS.OBJ \
  MODELESS.RES \
  MODELESS.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"MODELESS.EXE" MODELESS.DEF 
MODELESS.OBJ
<<
   RC MODELESS.RES MODELESS.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include MODELESS.DEP
