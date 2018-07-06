# IBM Developer's Workframe/2 Make File Creation run at 16:31:08 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP04\LIST07;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

ENUM.EXE:  \
  ENUM.OBJ \
  ENUM.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"ENUM.EXE" ENUM.DEF 
ENUM.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include ENUM.DEP
