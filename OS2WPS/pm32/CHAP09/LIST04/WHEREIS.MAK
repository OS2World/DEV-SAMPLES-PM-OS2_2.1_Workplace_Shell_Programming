# IBM Developer's Workframe/2 Make File Creation run at 00:22:26 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP09\LIST04;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: WHEREIS.EXE \
     WHEREIS.RES

WHEREIS.EXE:  \
  WHERE.OBJ \
  WHEREIS.OBJ \
  WHEREIS.RES \
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
   ICC.EXE /Ss /Q /Wparprotrduse /Fi /Si /Ti /Gm /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprotrduse /Fi /Si /Ti /Gm /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprotrduse /Fi /Si /Ti /Gm /G4 /Gs /C   .\$*.cxx

!include WHEREIS.DEP
