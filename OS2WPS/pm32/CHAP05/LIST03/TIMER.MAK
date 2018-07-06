# IBM Developer's Workframe/2 Make File Creation run at 19:52:48 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP05\LIST03;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

TIMER.EXE:  \
  timer.OBJ \
  TIMER.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"TIMER.EXE" TIMER.DEF 
timer.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include TIMER.DEP
