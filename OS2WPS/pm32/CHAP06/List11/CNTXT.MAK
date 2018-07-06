# IBM Developer's Workframe/2 Make File Creation run at 21:22:09 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP06\LIST11;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: CNTXT.EXE \
     CNTXT.RES

CNTXT.EXE:  \
  CNTXT.OBJ \
  CNTXT.RES \
  CNTXT.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"CNTXT.EXE" CNTXT.DEF 
CNTXT.OBJ
<<
   RC CNTXT.RES CNTXT.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include CNTXT.DEP
