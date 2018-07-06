# IBM Developer's Workframe/2 Make File Creation run at 20:14:22 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP05\LIST08;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: BITMAP.EXE \
     BITMAP.RES

BITMAP.EXE:  \
  BITMAP.OBJ \
  BITMAP.RES \
  BITMAP.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"BITMAP.EXE" BITMAP.DEF 
BITMAP.OBJ
<<
   RC BITMAP.RES BITMAP.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include BITMAP.DEP
