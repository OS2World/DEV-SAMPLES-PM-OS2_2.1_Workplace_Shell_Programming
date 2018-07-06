# IBM Developer's Workframe/2 Make File Creation run at 15:52:07 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP03\LIST04;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

SHOW.EXE:  \
  SHOW.OBJ \
  SHOW.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"SHOW.EXE" SHOW.DEF 
SHOW.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Ti /G4 /Gs /C   .\$*.cxx

!include SHOW.DEP
