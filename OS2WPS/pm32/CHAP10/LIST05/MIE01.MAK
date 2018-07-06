# IBM Developer's Workframe/2 Make File Creation run at 09:06:06 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP10\LIST05;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

MIE01.DLL:  \
  MIE01.OBJ \
  MIE01.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo /noe"
 /Fe"MIE01.DLL" MIE01.DEF 
MIE01.OBJ
<<
  IMPLIB MIE01.LIB MIE01.DLL

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /Ge- /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /Ge- /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /Ge- /G4 /Gs /C   .\$*.cxx

!include MIE01.DEP
