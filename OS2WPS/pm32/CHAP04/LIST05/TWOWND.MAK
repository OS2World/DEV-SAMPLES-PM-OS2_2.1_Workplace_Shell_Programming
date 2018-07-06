# IBM Developer's Workframe/2 Make File Creation run at 16:24:03 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP04\LIST05;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

TWOWND.EXE:  \
  TWOWND.OBJ \
  TWOWND.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"TWOWND.EXE" TWOWND.DEF 
TWOWND.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include TWOWND.DEP
