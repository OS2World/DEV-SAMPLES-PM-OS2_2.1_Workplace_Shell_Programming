# IBM Developer's Workframe/2 Make File Creation run at 14:09:28 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP11\LIST01;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: CLIPPUT.EXE \
     CLIPPUT.RES

CLIPPUT.EXE:  \
  CLIPPUT.OBJ \
  CLIPPUT.RES \
  CLIPPUT.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"CLIPPUT.EXE" CLIPPUT.DEF 
CLIPPUT.OBJ
<<
   RC CLIPPUT.RES CLIPPUT.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include CLIPPUT.DEP
