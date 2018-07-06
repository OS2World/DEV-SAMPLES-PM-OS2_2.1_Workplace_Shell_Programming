# IBM Developer's Workframe/2 Make File Creation run at 14:20:44 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP11\LIST03;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: CLIENT.EXE \
     CLIENT.RES

CLIENT.EXE:  \
  CLIENT.OBJ \
  CLIENT.RES \
  CLIENT.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"CLIENT.EXE" CLIENT.DEF 
CLIENT.OBJ
<<
   RC CLIENT.RES CLIENT.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include CLIENT.DEP
