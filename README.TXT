OS/2 2.1 Workplace Shell Programming
Stefano Maruzzi - Random House 1993


Disk contents
------------

BOOKINST.EXE		Installation program
MIE02.DLL		DLL required for installation
LH.EXE        		Shareware compression utility
OS2WPS.LZH       	Book examples 
PRJS.LZH		Project files
README.TXT          This document
LIST.TXT		Summary of program listings



OS2WPS.LHZ
----------
This file contains sample source listings for the book. The files are divided in 13 directories, one for each chapter. Each chapter
directory has a variable number of subdirectories, one for each code
sample.
Once the installation process is over, the tree structure on the chosen
hard disk will resemble this:

z:\PM32
  |
  CHAP02
       |
       LIST01
            xxxxx0.yyy
            xxxxx1.yyy
            xxxxx2.yyy

where z is the target drive and xxxxx0.yyy is a generic file.
For each sample code I provided both the .MAK and the .DEP files. I strongly
recommend you recreate both files if you experience any problem during
compilation time or if using a different version of the C Set++ compiler
(or another compiler.)


Installation procedure
----------------------
Start BOOKINST.EXE either from a full screen OS/2 prompt or 
directly by double-clicking on the Drive A Workplace Shell object.
BOOKINST create a window on the screen with the book cover as its
background. Simply click the right mouse button on any location on the
client window to let the window context menu pop-up. Choose install
to start the installation procedure. A listbox appears on the bottom
of the window showing all the available drives in the system.
Select the target drive to start the installation procedure.
When it terminates you will be notified by a beep.
BOOKINST creates the \PM32 directory on the target drive and displays
a shadow copy on the desktop.

You can always interrupt the installation procedure by selecting
the Close menuitem in the window context menu.

Notice
------
All the example have been written on a IBM PS/2 model 90, 24MB RAM,
2 SCSI 400MB drives and XGA card. The use of at least a 800 x 600 video resolution is strongly recommended.

Disk enhancements
-----------------
Some examples have been slightly modified and enhanced with new
features and functionalities. Although all the information provided
in the book is still valid and correct, some samples provide
more advanced features and programming hints to better exploit WPS.


C Set++ 2.0 compiler
--------------------
All the examples have been written and tested using the IBM C Set++ 2.0
compiler. The installation procedure looks for the \IBMWF directory to
appropriately store the .PRJ files for each example provided in the book.
Later versions of this compiler might require a different location for the
.PRJ files. For your convenience all the .PRJ files are located in each
subdirectory too.


Other compilers
-------------
The example provided don't have any specific dependencies on the C Set++
compiler. Recompilation with other OS/2 C/C++ compilers requires only 
the creation of appropriate project files.
