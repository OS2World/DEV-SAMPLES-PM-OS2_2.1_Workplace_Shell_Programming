# IBM Developer's Workframe/2 Make File Creation run at 19:59:19 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP05\LIST05;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: ICON.EXE \
     ICON.RES

ICON.EXE:  \
  ICON.OBJ \
  ICON.RES \
  ICON.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"ICON.EXE" ICON.DEF 
ICON.OBJ
<<
   RC ICON.RES ICON.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include ICON.DEP
