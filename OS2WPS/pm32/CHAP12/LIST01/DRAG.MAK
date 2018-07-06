# IBM Developer's Workframe/2 Make File Creation run at 14:32:25 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP12\LIST01;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: DRAG.EXE \
     DRAG.RES

DRAG.EXE:  \
  DRAG.OBJ \
  DRAG.RES \
  DRAG.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"DRAG.EXE" DRAG.DEF 
DRAG.OBJ
<<
   RC DRAG.RES DRAG.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include DRAG.DEP
