# IBM Developer's Workframe/2 Make File Creation run at 14:46:43 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP12\LIST03;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: PANEL.EXE \
     PANEL.RES

PANEL.EXE:  \
  MIE02.LIB \
  PANEL.OBJ \
  PANEL.RES \
  PANEL.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"PANEL.EXE" MIE02.LIB PANEL.DEF 
MIE02.LIB 
PANEL.OBJ
<<
   RC PANEL.RES PANEL.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include PANEL.DEP
