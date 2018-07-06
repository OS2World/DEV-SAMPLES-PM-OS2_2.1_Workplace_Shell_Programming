# IBM Developer's Workframe/2 Make File Creation run at 22:16:28 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST11;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: FOLDER.EXE \
     FOLDER.RES

FOLDER.EXE:  \
  FOLDER.OBJ \
  FOLDER.RES \
  FOLDER.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"FOLDER.EXE" FOLDER.DEF 
FOLDER.OBJ
<<
   RC FOLDER.RES FOLDER.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include FOLDER.DEP
