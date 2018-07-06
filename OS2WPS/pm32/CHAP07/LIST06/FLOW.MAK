# IBM Developer's Workframe/2 Make File Creation run at 22:00:17 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST06;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: FLOW.EXE \
     FLOW.RES

FLOW.EXE:  \
  FLOW.OBJ \
  FLOW.RES \
  FLOW.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"FLOW.EXE" FLOW.DEF 
FLOW.OBJ
<<
   RC FLOW.RES FLOW.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include FLOW.DEP
