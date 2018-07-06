# IBM Developer's Workframe/2 Make File Creation run at 21:12:26 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP06\LIST07;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: TWOMENUS.EXE \
     TWOMENUS.RES

TWOMENUS.EXE:  \
  TWOMENUS.OBJ \
  TWOMENUS.RES \
  TWOMENUS.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"TWOMENUS.EXE" TWOMENUS.DEF 
TWOMENUS.OBJ
<<
   RC TWOMENUS.RES TWOMENUS.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include TWOMENUS.DEP
