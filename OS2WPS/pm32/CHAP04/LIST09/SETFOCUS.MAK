# IBM Developer's Workframe/2 Make File Creation run at 16:37:09 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP04\LIST09;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

SETFOCUS.EXE:  \
  SETFOCUS.OBJ \
  SETFOCUS.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"SETFOCUS.EXE" SETFOCUS.DEF 
SETFOCUS.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include SETFOCUS.DEP
