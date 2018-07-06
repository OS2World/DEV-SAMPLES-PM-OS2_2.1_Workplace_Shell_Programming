# IBM Developer's Workframe/2 Make File Creation run at 10:37:55 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP10\LIST09;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

MIECC.DLL:  \
  MIECC.OBJ \
  MIECC.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo /noe"
 /Fe"MIECC.DLL" MIECC.DEF 
MIECC.OBJ
<<
  IMPLIB MIECC.LIB MIECC.DLL

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /Ge- /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /Ge- /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /Ge- /G4 /Gs /C   .\$*.cxx

!include MIECC.DEP
