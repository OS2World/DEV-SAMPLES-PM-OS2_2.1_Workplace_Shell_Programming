# IBM Developer's Workframe/2 Make File Creation run at 08:58:14 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP10\LIST03;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: SUPERCLS.EXE \
     SUPERCLS.RES

SUPERCLS.EXE:  \
  SUPERCLS.OBJ \
  SUPERCLS.RES \
  SUPERCLS.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"SUPERCLS.EXE" SUPERCLS.DEF 
SUPERCLS.OBJ
<<
   RC SUPERCLS.RES SUPERCLS.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include SUPERCLS.DEP
