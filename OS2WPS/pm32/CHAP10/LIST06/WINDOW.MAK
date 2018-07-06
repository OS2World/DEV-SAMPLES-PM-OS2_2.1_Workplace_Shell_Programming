# IBM Developer's Workframe/2 Make File Creation run at 10:28:05 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP10\LIST06;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: WINDOW.EXE \
     WINDOW.RES

WINDOW.EXE:  \
  WINDOW.OBJ \
  WINDOW.RES \
  WINDOW.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"WINDOW.EXE" MIE02.LIB WINDOW.DEF 
WINDOW.OBJ
<<
   RC WINDOW.RES WINDOW.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include WINDOW.DEP
