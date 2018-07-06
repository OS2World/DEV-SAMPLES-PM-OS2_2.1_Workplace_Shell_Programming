# IBM Developer's Workframe/2 Make File Creation run at 16:12:38 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP04\LIST02;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

OWNER.EXE:  \
  OWNER.OBJ \
  OWNER.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"OWNER.EXE" OWNER.DEF 
OWNER.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include OWNER.DEP
