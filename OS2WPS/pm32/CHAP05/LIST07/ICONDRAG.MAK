# IBM Developer's Workframe/2 Make File Creation run at 20:10:50 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP05\LIST07;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: ICONDRAG.EXE \
     ICONDRAG.RES

ICONDRAG.EXE:  \
  ICONDRAG.OBJ \
  ICONDRAG.RES \
  ICONDRAG.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"ICONDRAG.EXE" ICONDRAG.DEF 
ICONDRAG.OBJ
<<
   RC ICONDRAG.RES ICONDRAG.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include ICONDRAG.DEP
