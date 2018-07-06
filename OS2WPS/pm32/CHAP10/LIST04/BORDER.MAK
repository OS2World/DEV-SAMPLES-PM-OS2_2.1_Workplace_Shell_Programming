# IBM Developer's Workframe/2 Make File Creation run at 09:23:02 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP10\LIST04;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: BORDER.EXE \
     BORDER.RES

BORDER.EXE:  \
  BORDER.OBJ \
  BORDER.RES \
  BORDER.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"BORDER.EXE" MIE01.LIB BORDER.DEF 
BORDER.OBJ
<<
   RC BORDER.RES BORDER.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include BORDER.DEP
