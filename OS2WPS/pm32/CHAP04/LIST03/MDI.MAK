# IBM Developer's Workframe/2 Make File Creation run at 16:15:06 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP04\LIST03;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

MDI.EXE:  \
  MDI.OBJ \
  MDI.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"MDI.EXE" MDI.DEF 
MDI.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include MDI.DEP
