# IBM Developer's Workframe/2 Make File Creation run at 22:13:22 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST10;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: NOTEBOOK.EXE \
     NOTEBOOK.RES

NOTEBOOK.EXE:  \
  NOTEBOOK.OBJ \
  NOTEBOOK.RES \
  NOTEBOOK.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"NOTEBOOK.EXE" NOTEBOOK.DEF 
NOTEBOOK.OBJ
<<
   RC NOTEBOOK.RES NOTEBOOK.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include NOTEBOOK.DEP
