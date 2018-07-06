#------------------
# MACHINE make file
#------------------

machine.obj: machine.c
     cl -c -G2sw -W3 -Zipe -Od machine.c

machine.exe: machine.obj machine.def
     link machine, /CO /align:16, NUL, os2, machine
