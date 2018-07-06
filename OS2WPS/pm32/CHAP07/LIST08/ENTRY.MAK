# IBM Developer's Workframe/2 Make File Creation run at 22:07:41 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST08;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: ENTRY.EXE \
     ENTRY.RES

ENTRY.EXE:  \
  ENTRY.OBJ \
  ENTRY.RES \
  ENTRY.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"ENTRY.EXE" ENTRY.DEF 
ENTRY.OBJ
<<
   RC ENTRY.RES ENTRY.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include ENTRY.DEP
