# IBM Developer's Workframe/2 Make File Creation run at 15:02:17 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP12\LIST04;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: WPSFLDR.EXE \
     WPSFLDR.RES

WPSFLDR.EXE:  \
  WPSFLDR.OBJ \
  WPSFLDR.RES \
  WPSFLDR.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"WPSFLDR.EXE" WPSFLDR.DEF 
WPSFLDR.OBJ
<<
   RC WPSFLDR.RES WPSFLDR.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include WPSFLDR.DEP
