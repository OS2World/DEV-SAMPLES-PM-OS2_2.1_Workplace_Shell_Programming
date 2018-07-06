# IBM Developer's Workframe/2 Make File Creation run at 20:18:24 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP05\LIST09;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: ASSOCT.EXE \
     ASSOCT.RES

ASSOCT.EXE:  \
  ASSOCT.OBJ \
  ASSOCT.RES \
  ASSOCT.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"ASSOCT.EXE" ASSOCT.DEF 
ASSOCT.OBJ
<<
   RC ASSOCT.RES ASSOCT.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include ASSOCT.DEP
