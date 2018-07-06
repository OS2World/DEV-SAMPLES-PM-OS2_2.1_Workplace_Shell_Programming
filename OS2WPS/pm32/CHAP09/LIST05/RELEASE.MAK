PROJ = release
PROJFILE = release.mak
DEBUG = 0

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
LFLAGS_D  =  /CO /F /PACKC /PACKD  /PMTYPE:PM
LFLAGS_R  = /EXE /FAR /PACKC /PACKD /PMTYPE:PM
MAPFILE_D  = NUL
MAPFILE_R  = NUL
CC	= cl
CFLAGS_G  = /Alfw /W3 /G2 /D_MT /Zp /MT /BATCH
CFLAGS_D  = /Od /FPc87 /Zi /Gs
CFLAGS_R  = /Ot /Oi /Ol /Oe /Og /Gs
LLIBS_R  =	/NOD:LLIBCE LLIBCMT
LLIBS_D  =	/NOD:LLIBC7 LLIBCMT
ASM  = masm
AFLAGS_G  = /Mx /T
AFLAGS_D  = /Zi

OBJS  = whereis.obj WHERE.obj
RESS  = WHEREIS.res
SBRS  = whereis.sbr WHERE.sbr

all: $(PROJ).exe

.SUFFIXES: .c .sbr .rc .res .obj

whereis.obj : whereis.c

whereis.sbr : whereis.c

WHERE.obj : WHERE.C

WHERE.sbr : WHERE.C

WHEREIS.res : WHEREIS.RC


$(PROJ).bsc : $(SBRS)
		$(PWBRMAKE) @<<
$(BRFLAGS) $(SBRS)
<<

$(PROJ).exe : $(OBJS) $(RESS)
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
		$(LINKER) @$(PROJ).lrf
!ELSE
		$(LINKER) @$(PROJ).lrf
!ENDIF
		$(RC) $(RESS) $@


.c.sbr :
!IF $(DEBUG)
		$(CC) /Zs $(CFLAGS_G) $(CFLAGS_D) /FR$@ $<
!ELSE
		$(CC) /Zs $(CFLAGS_G) $(CFLAGS_R) /FR$@ $<
!ENDIF

.rc.res :
		$(RC) /r $< $@

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
