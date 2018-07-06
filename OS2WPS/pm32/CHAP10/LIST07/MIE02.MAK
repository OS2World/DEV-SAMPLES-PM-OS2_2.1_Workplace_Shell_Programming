# IBM Developer's Workframe/2 Make File Creation run at 09:26:47 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP10\LIST07;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: MIE02.DLL \
     MIE02.RES

MIE02.DLL:  \
  MIE02.OBJ \
  MIE02.RES \
  MIE02.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"MIE02.DLL" MIE02.DEF 
MIE02.OBJ
<<
   RC MIE02.RES MIE02.DLL
  IMPLIB MIE02.LIB MIE02.DLL

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /Ge- /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /Ge- /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /Ge- /G4 /Gs /C   .\$*.cxx

!include MIE02.DEP
