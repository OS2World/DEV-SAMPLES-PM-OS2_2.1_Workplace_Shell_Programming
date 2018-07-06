# IBM Developer's Workframe/2 Make File Creation run at 14:12:28 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP11\LIST02;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: CLIPSHOW.EXE \
     CLIPSHOW.RES

CLIPSHOW.EXE:  \
  CLIPSHOW.OBJ \
  CLIPSHOW.RES \
  CLIPSHOW.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"CLIPSHOW.EXE" CLIPSHOW.DEF 
CLIPSHOW.OBJ
<<
   RC CLIPSHOW.RES CLIPSHOW.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include CLIPSHOW.DEP
