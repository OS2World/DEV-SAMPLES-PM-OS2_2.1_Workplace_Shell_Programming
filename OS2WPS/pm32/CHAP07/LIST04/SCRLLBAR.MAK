# IBM Developer's Workframe/2 Make File Creation run at 21:53:26 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST04;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: SCRLLBAR.EXE \
     SCRLLBAR.RES

SCRLLBAR.EXE:  \
  SCRLLBAR.OBJ \
  SCRLLBAR.RES \
  SCRLLBAR.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"SCRLLBAR.EXE" SCRLLBAR.DEF 
SCRLLBAR.OBJ
<<
   RC SCRLLBAR.RES SCRLLBAR.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include SCRLLBAR.DEP
