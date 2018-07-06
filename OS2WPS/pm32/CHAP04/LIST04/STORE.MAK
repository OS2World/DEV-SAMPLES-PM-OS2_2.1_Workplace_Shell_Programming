# IBM Developer's Workframe/2 Make File Creation run at 16:19:06 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP04\LIST04;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

STORE.EXE:  \
  STORE.OBJ \
  STORE.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"STORE.EXE" STORE.DEF 
STORE.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include STORE.DEP
