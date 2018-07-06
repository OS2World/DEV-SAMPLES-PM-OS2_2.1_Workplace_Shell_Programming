# IBM Developer's Workframe/2 Make File Creation run at 08:21:37 on 07/27/93

# Make File Creation run in directory:
#   F:\PM32\CHAP13\LIST03;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: INSTALL.EXE \
     install.RES

INSTALL.EXE:  \
  install.OBJ \
  install.RES \
  mie02.lib \
  where.OBJ \
  INSTALL.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"INSTALL.EXE" MIE02.LIB INSTALL.DEF 
install.OBJ 
mie02.lib 
where.OBJ
<<
   RC install.RES INSTALL.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include INSTALL.DEP
