# IBM Developer's Workframe/2 Make File Creation run at 22:30:08 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST13;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: SPIN.EXE \
     SPIN.RES

SPIN.EXE:  \
  SPIN.OBJ \
  SPIN.RES \
  SPIN.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"SPIN.EXE" SPIN.DEF 
SPIN.OBJ
<<
   RC SPIN.RES SPIN.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include SPIN.DEP
