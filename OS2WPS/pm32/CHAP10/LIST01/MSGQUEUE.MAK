# IBM Developer's Workframe/2 Make File Creation run at 08:51:11 on 07/26/93

# Make File Creation run in directory:
#   F:\PM32\CHAP10\LIST01;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: MSGQUEUE.EXE \
     MSGQUEUE.RES

MSGQUEUE.EXE:  \
  MSGQUEUE.OBJ \
  MSGQUEUE.RES \
  MSGQUEUE.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"MSGQUEUE.EXE" MSGQUEUE.DEF 
MSGQUEUE.OBJ
<<
   RC MSGQUEUE.RES MSGQUEUE.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include MSGQUEUE.DEP
