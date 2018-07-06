PROJ = MACHINE
PROJFILE = MACHINE.MAK
DEBUG = 1

PWBRMAKE  = pwbrmake
NMAKEBSC1  = set
NMAKEBSC2  = nmake
LINKER	= link
ILINK  = ilink
LRF  = echo > NUL
BIND  = bind
RC	= rc
IMPLIB	= implib
LFLAGS_G  =  /NOI /ST:8192	/BATCH
LFLAGS_D  =  /CO /INC /F /PACKC /PACKD	/PMTYPE:PM
LFLAGS_R  =  /E /F /PACKC /PACKD  /PMTYPE:PM
MAPFILE_D  = NUL
MAPFILE_R  = NUL
CC	= cl
CFLAGS_G  = /W3 /G2 /Zp /BATCH /FR$*.sbr
CFLAGS_D  = /qc /Gs /Gi$(PROJ).mdt /Zr /Zi /Od
CFLAGS_R  = /Ot /Oi /Ol /Oe /Og /Gs
ASM  = masm
AFLAGS_G  = /Mx /T
AFLAGS_D  = /Zi
BRFLAGS  =	/o $(PROJ).bsc
BROWSE	= 1
CVFLAGS  =	/50

OBJS  = MACHINE.obj
SBRS  = MACHINE.sbr

all: $(PROJ).exe

.SUFFIXES: .c .sbr .obj

MACHINE.obj : MACHINE.C C:\C600\INCLUDE\os2.h C:\C600\INCLUDE\stddef.h\
		machine.h C:\C600\INCLUDE\os2def.h C:\C600\INCLUDE\bse.h\
		C:\C600\INCLUDE\pm.h C:\C600\INCLUDE\bsedos.h C:\C600\INCLUDE\bsesub.h\
		C:\C600\INCLUDE\bseerr.h C:\C600\INCLUDE\bsedev.h\
		C:\C600\INCLUDE\pmwin.h C:\C600\INCLUDE\pmgpi.h\
		C:\C600\INCLUDE\pmdev.h C:\C600\INCLUDE\pmavio.h\
		C:\C600\INCLUDE\pmpic.h C:\C600\INCLUDE\pmord.h\
		C:\C600\INCLUDE\pmbitmap.h C:\C600\INCLUDE\pmfont.h\
		C:\C600\INCLUDE\pmtypes.h C:\C600\INCLUDE\pmmle.h\
		C:\C600\INCLUDE\pmshl.h C:\C600\INCLUDE\pmerr.h\
		C:\C600\INCLUDE\pmhelp.h

MACHINE.sbr : MACHINE.C C:\C600\INCLUDE\os2.h C:\C600\INCLUDE\stddef.h\
		machine.h C:\C600\INCLUDE\os2def.h C:\C600\INCLUDE\bse.h\
		C:\C600\INCLUDE\pm.h C:\C600\INCLUDE\bsedos.h C:\C600\INCLUDE\bsesub.h\
		C:\C600\INCLUDE\bseerr.h C:\C600\INCLUDE\bsedev.h\
		C:\C600\INCLUDE\pmwin.h C:\C600\INCLUDE\pmgpi.h\
		C:\C600\INCLUDE\pmdev.h C:\C600\INCLUDE\pmavio.h\
		C:\C600\INCLUDE\pmpic.h C:\C600\INCLUDE\pmord.h\
		C:\C600\INCLUDE\pmbitmap.h C:\C600\INCLUDE\pmfont.h\
		C:\C600\INCLUDE\pmtypes.h C:\C600\INCLUDE\pmmle.h\
		C:\C600\INCLUDE\pmshl.h C:\C600\INCLUDE\pmerr.h\
		C:\C600\INCLUDE\pmhelp.h


$(PROJ).bsc : $(SBRS)
		$(PWBRMAKE) @<<
$(BRFLAGS) $(SBRS)
<<

$(PROJ).exe : $(OBJS)
!IF $(DEBUG)
		$(LRF) @<<$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJS: = +^
)
$@
$(MAPFILE_D)
$(LLIBS_G: = +^
) +
$(LLIBS_D: = +^
) +
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_D);
<<
!ELSE
		$(LRF) @<<$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJS: = +^
)
$@
$(MAPFILE_R)
$(LLIBS_G: = +^
) +
$(LLIBS_R: = +^
) +
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_R);
<<
!ENDIF
!IF $(DEBUG)
		$(ILINK) -a -e "$(LINKER) @$(PROJ).lrf" $@
!ELSE
		$(LINKER) @$(PROJ).lrf
!ENDIF
		$(NMAKEBSC1) MAKEFLAGS=
		$(NMAKEBSC2) $(NMFLAGS) -f $(PROJFILE) $(PROJ).bsc


.c.sbr :
!IF $(DEBUG)
		$(CC) /Zs $(CFLAGS_G) $(CFLAGS_D) /FR$@ $<
!ELSE
		$(CC) /Zs $(CFLAGS_G) $(CFLAGS_R) /FR$@ $<
!ENDIF

.c.obj :
!IF $(DEBUG)
		$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo$@ $<
!ELSE
		$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo$@ $<
!ENDIF


run: $(PROJ).exe
		$(PROJ).exe $(RUNFLAGS)

debug: $(PROJ).exe
		CVP $(CVFLAGS) $(PROJ).exe $(RUNFLAGS)
