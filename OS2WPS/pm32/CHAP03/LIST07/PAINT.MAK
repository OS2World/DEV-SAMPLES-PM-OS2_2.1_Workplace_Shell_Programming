# IBM Developer's Workframe/2 Make File Creation run at 16:03:56 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP03\LIST07;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

PAINT.EXE:  \
  PAINT.OBJ \
  PAINT.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"PAINT.EXE" PAINT.DEF 
PAINT.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include PAINT.DEP
