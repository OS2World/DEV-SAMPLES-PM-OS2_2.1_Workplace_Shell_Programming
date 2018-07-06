# IBM Developer's Workframe/2 Make File Creation run at 19:49:28 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP05\LIST02;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

MOUSE.EXE:  \
  MOUSE.OBJ \
  MOUSE.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"MOUSE.EXE" MOUSE.DEF 
MOUSE.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include MOUSE.DEP
