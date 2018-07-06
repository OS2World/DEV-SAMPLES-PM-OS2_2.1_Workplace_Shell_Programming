# IBM Developer's Workframe/2 Make File Creation run at 23:45:13 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP08\LIST04;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: NEWOPEN.EXE \
     NEWOPEN.RES

NEWOPEN.EXE:  \
  NEWOPEN.OBJ \
  NEWOPEN.RES \
  NEWOPEN.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"NEWOPEN.EXE" NEWOPEN.DEF 
NEWOPEN.OBJ
<<
   RC NEWOPEN.RES NEWOPEN.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include NEWOPEN.DEP
