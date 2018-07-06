PROJ = machine
PROJFILE = machine.mak
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
LFLAGS_G  = /STACK:8192 /NOI /BATCH
LFLAGS_D  = /CO /INC /FAR /PACKC /PACKD /PMTYPE:PM
LFLAGS_R  = /EXE /FAR /PACKC /PACKD /PMTYPE:PM
MAPFILE_D  = NUL
MAPFILE_R  = NUL
CC	= cl
CFLAGS_G  = /W3 /G2 /Zp /BATCH
CFLAGS_D  = /qc /Gs /Gi$(PROJ).mdt /Zr /Zi /Od
CFLAGS_R  = /Ot /Oi /Ol /Oe /Og /Gs
ASM  = masm
AFLAGS_G  = /Mx /T
AFLAGS_D  = /Zi

OBJS  = machine.obj
SBRS  = machine.sbr

all: $(PROJ).exe

.SUFFIXES: .c .sbr .obj

machine.obj : machine.c

machine.sbr : machine.c


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
