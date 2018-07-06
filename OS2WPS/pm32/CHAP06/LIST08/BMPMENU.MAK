# IBM Developer's Workframe/2 Make File Creation run at 21:15:18 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP06\LIST08;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: BMPMENU.EXE \
     BMPMENU.RES

BMPMENU.EXE:  \
  BMPMENU.OBJ \
  BMPMENU.RES \
  BMPMENU.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"BMPMENU.EXE" BMPMENU.DEF 
BMPMENU.OBJ
<<
   RC BMPMENU.RES BMPMENU.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include BMPMENU.DEP
