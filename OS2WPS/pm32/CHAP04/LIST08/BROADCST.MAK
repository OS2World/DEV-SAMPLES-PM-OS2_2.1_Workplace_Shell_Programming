# IBM Developer's Workframe/2 Make File Creation run at 16:34:57 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP04\LIST08;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

BROADCST.EXE:  \
  BROADCST.OBJ \
  BROADCST.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"BROADCST.EXE" BROADCST.DEF 
BROADCST.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include BROADCST.DEP
