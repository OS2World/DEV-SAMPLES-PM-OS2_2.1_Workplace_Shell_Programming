# IBM Developer's Workframe/2 Make File Creation run at 22:34:23 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST14;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: VALUESET.EXE \
     VALUESET.RES

VALUESET.EXE:  \
  VALUESET.OBJ \
  VALUESET.RES \
  VALUESET.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"VALUESET.EXE" VALUESET.DEF 
VALUESET.OBJ
<<
   RC VALUESET.RES VALUESET.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include VALUESET.DEP
