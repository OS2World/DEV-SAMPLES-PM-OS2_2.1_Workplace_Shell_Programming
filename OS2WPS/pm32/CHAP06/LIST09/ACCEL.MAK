# IBM Developer's Workframe/2 Make File Creation run at 21:17:29 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP06\LIST09;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: ACCEL.EXE \
     ACCEL.RES

ACCEL.EXE:  \
  ACCEL.OBJ \
  ACCEL.RES \
  ACCEL.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"ACCEL.EXE" ACCEL.DEF 
ACCEL.OBJ
<<
   RC ACCEL.RES ACCEL.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include ACCEL.DEP
