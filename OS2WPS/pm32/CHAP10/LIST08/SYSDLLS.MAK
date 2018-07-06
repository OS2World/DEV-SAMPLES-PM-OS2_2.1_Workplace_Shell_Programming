# IBM Developer's Workframe/2 Make File Creation run at 10:32:56 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP10\LIST08;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: SYSDLLS.EXE \
     SYSDLLS.RES

SYSDLLS.EXE:  \
  SYSDLLS.OBJ \
  SYSDLLS.RES \
  SYSDLLS.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"SYSDLLS.EXE" SYSDLLS.DEF 
SYSDLLS.OBJ
<<
   RC SYSDLLS.RES SYSDLLS.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include SYSDLLS.DEP
