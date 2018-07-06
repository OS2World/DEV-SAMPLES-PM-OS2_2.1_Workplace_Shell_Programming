# IBM Developer's Workframe/2 Make File Creation run at 14:25:27 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP11\LIST04;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: SERVER.EXE \
     SERVER.RES

SERVER.EXE:  \
  SERVER.OBJ \
  SERVER.RES \
  SERVER.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"SERVER.EXE" SERVER.DEF 
SERVER.OBJ
<<
   RC SERVER.RES SERVER.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include SERVER.DEP
