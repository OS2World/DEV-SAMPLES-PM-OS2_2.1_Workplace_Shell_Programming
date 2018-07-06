# IBM Developer's Workframe/2 Make File Creation run at 14:40:50 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP12\LIST02;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: DROP.EXE \
     DROP.RES

DROP.EXE:  \
  DROP.OBJ \
  DROP.RES \
  MIE01.LIB \
  DROP.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"DROP.EXE" MIE01.LIB DROP.DEF 
DROP.OBJ 
MIE01.LIB
<<
   RC DROP.RES DROP.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include DROP.DEP
