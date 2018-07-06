# IBM Developer's Workframe/2 Make File Creation run at 21:56:40 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP07\LIST05;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc

ALL: LISTBOX.EXE \
     LISTBOX.RES

LISTBOX.EXE:  \
  LISTBOX.OBJ \
  LISTBOX.RES \
  LISTBOX.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"LISTBOX.EXE" LISTBOX.DEF 
LISTBOX.OBJ
<<
   RC LISTBOX.RES LISTBOX.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include LISTBOX.DEP
