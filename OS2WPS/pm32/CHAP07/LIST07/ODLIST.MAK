# IBM Developer's Workframe/2 Make File Creation run at 22:03:52 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST07;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: ODLIST.EXE \
     ODLIST.RES

ODLIST.EXE:  \
  ODLIST.OBJ \
  ODLIST.RES \
  ODLIST.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"ODLIST.EXE" ODLIST.DEF 
ODLIST.OBJ
<<
   RC ODLIST.RES ODLIST.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include ODLIST.DEP
