# IBM Developer's Workframe/2 Make File Creation run at 23:07:17 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP08\LIST03;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: OLDOPEN.EXE \
     OLDOPEN.RES

OLDOPEN.EXE:  \
  OLDOPEN.OBJ \
  OLDOPEN.RES \
  OLDOPEN.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"OLDOPEN.EXE" OLDOPEN.DEF 
OLDOPEN.OBJ
<<
   RC OLDOPEN.RES OLDOPEN.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include OLDOPEN.DEP
