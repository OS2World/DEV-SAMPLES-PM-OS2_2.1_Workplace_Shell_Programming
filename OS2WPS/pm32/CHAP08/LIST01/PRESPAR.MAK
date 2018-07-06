# IBM Developer's Workframe/2 Make File Creation run at 22:48:19 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP08\LIST01;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: PRESPAR.EXE \
     prespar.RES

PRESPAR.EXE:  \
  prespar.OBJ \
  prespar.RES \
  PRESPAR.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"PRESPAR.EXE" PRESPAR.DEF 
prespar.OBJ
<<
   RC prespar.RES PRESPAR.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include PRESPAR.DEP
