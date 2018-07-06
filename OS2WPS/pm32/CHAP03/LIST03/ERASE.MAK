# IBM Developer's Workframe/2 Make File Creation run at 15:48:28 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP03\LIST03;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

ERASE.EXE:  \
  ERASE.OBJ \
  ERASE.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"ERASE.EXE" ERASE.DEF 
ERASE.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs   .\$*.cxx

!include ERASE.DEP
