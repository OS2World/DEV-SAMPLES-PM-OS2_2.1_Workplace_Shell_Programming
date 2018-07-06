# IBM Developer's Workframe/2 Make File Creation run at 12:05:11 on 10/06/93

# Make File Creation run in directory:
#   C:\PM32BK\CHAP09\LIST05;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: WHEREIS.EXE \
     WHEREIS.RES

WHEREIS.EXE:  \
  WHERE.OBJ \
  WHEREIS.OBJ \
  WHEREIS.RES \
  WHEREIS.DEF \
  WHEREIS.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"WHEREIS.EXE" WHEREIS.DEF 
WHERE.OBJ 
WHEREIS.OBJ
<<
   RC WHEREIS.RES WHEREIS.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprotrduse /Ti /Gm /G4 /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprotrduse /Ti /Gm /G4 /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprotrduse /Ti /Gm /G4 /C   .\$*.cxx

!include WHEREIS.DEP
