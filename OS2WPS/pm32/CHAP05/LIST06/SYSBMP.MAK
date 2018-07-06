# IBM Developer's Workframe/2 Make File Creation run at 20:01:17 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP05\LIST06;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: SYSBMP.EXE \
     SYSBMP.RES

SYSBMP.EXE:  \
  SYSBMP.OBJ \
  SYSBMP.RES \
  SYSBMP.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"SYSBMP.EXE" SYSBMP.DEF 
SYSBMP.OBJ
<<
   RC SYSBMP.RES SYSBMP.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include SYSBMP.DEP
