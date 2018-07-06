# IBM Developer's Workframe/2 Make File Creation run at 16:26:15 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP04\LIST06;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

CLCOLOR.EXE:  \
  CLCOLOR.OBJ \
  CLCOLOR.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"CLCOLOR.EXE" CLCOLOR.DEF 
CLCOLOR.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include CLCOLOR.DEP
