# IBM Developer's Workframe/2 Make File Creation run at 00:20:39 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP09\LIST03;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: SNOOPER.EXE \
     SNOOPER.RES

SNOOPER.EXE:  \
  SNOOPER.OBJ \
  SNOOPER.RES \
  SNOOPER.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"SNOOPER.EXE" SNOOPER.DEF 
SNOOPER.OBJ
<<
   RC SNOOPER.RES SNOOPER.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include SNOOPER.DEP
