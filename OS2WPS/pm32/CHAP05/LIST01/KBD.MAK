# IBM Developer's Workframe/2 Make File Creation run at 19:45:26 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP05\LIST01;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

KBD.EXE:  \
  KBD.OBJ \
  KBD.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"KBD.EXE" KBD.DEF 
KBD.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include KBD.DEP
