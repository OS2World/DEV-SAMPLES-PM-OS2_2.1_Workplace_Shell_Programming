# IBM Developer's Workframe/2 Make File Creation run at 22:25:06 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST12;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: SLIDER.EXE \
     SLIDER.RES

SLIDER.EXE:  \
  SLIDER.OBJ \
  SLIDER.RES \
  SLIDER.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"SLIDER.EXE" SLIDER.DEF 
SLIDER.OBJ
<<
   RC SLIDER.RES SLIDER.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include SLIDER.DEP
