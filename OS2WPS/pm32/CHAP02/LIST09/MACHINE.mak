# IBM Developer's Workframe/2 Make File Creation run at 15:42:45 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP02\LIST09;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

MACHINE.EXE:  \
  MACHINE.OBJ \
  MACHINE.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"MACHINE.EXE" MACHINE.DEF 
MACHINE.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Wpartrduse /Fi /Ti /G4 /Gs   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Wpartrduse /Fi /Ti /G4 /Gs   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Wpartrduse /Fi /Ti /G4 /Gs   .\$*.cxx

!include MACHINE.DEP
