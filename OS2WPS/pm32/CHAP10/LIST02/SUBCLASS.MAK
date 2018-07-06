# IBM Developer's Workframe/2 Make File Creation run at 08:55:38 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP10\LIST02;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: SUBCLASS.EXE \
     SUBCLASS.RES

SUBCLASS.EXE:  \
  SUBCLASS.OBJ \
  SUBCLASS.RES \
  SUBCLASS.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"SUBCLASS.EXE" SUBCLASS.DEF 
SUBCLASS.OBJ
<<
   RC SUBCLASS.RES SUBCLASS.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include SUBCLASS.DEP
