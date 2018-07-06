# IBM Developer's Workframe/2 Make File Creation run at 10:40:05 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP10\LIST10;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: USECTL.EXE \
     USECTL.RES

USECTL.EXE:  \
  USECTL.OBJ \
  USECTL.RES \
  USECTL.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"USECTL.EXE" USECTL.DEF 
USECTL.OBJ
<<
   RC USECTL.RES USECTL.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include USECTL.DEP
