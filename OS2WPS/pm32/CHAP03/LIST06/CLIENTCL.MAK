# IBM Developer's Workframe/2 Make File Creation run at 15:57:30 on 07/25/93

# Make File Creation run in directory:
#   F:\PM32\CHAP03\LIST06;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

CLIENTCL.EXE:  \
  CLIENTCL.OBJ \
  CLIENTCL.MAK
   ICC.EXE @<<
 /B" /de /pmtype:pm /nologo"
 /Fe"CLIENTCL.EXE" CLIENTCL.DEF 
CLIENTCL.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /Q /Wparprouniuse /Fi /Si /Ti /G4 /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /Wparprouniuse /Fi /Si /Ti /G4 /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /Wparprouniuse /Fi /Si /Ti /G4 /Gs /C   .\$*.cxx

!include CLIENTCL.DEP
