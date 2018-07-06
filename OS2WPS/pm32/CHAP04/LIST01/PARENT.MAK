# IBM Developer's Workframe/2 Make File Creation run at 16:09:39 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP04\LIST01;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

PARENT.EXE:  \
  PARENT.OBJ \
  PARENT.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"PARENT.EXE" PARENT.DEF 
PARENT.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include PARENT.DEP
