# IBM Developer's Workframe/2 Make File Creation run at 22:10:22 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST09;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: COMBO.EXE \
     COMBO.RES

COMBO.EXE:  \
  COMBO.OBJ \
  COMBO.RES \
  COMBO.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"COMBO.EXE" COMBO.DEF 
COMBO.OBJ
<<
   RC COMBO.RES COMBO.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include COMBO.DEP
