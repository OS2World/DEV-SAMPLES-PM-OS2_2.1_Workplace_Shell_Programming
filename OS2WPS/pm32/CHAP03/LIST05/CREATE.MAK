# IBM Developer's Workframe/2 Make File Creation run at 15:54:56 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP03\LIST05;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

CREATE.EXE:  \
  CREATE.OBJ \
  CREATE.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"CREATE.EXE" CREATE.DEF 
CREATE.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouniuse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouniuse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouniuse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include CREATE.DEP
