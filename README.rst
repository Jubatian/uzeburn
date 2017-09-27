
XMBurner for Uzebox
==============================================================================

:Author:    Sandor Zsuga (Jubatian)
:License:   GNU GPLv3 (version 3 of the GNU General Public License)




Overview
------------------------------------------------------------------------------


This is an XMBurner application for Uzebox, demonstrating the use of the
library in an embedded environment.

Note that this demonstration only incorporates XMBurner as a library. The
XMBurner library itself is covered by Version 2 of the Mozilla Public License
(so the strict copyleft enforced by GPLv3 doesn't apply to that).



Setting up and Building
------------------------------------------------------------------------------


You need the Uzebox kernel and tools, and XMBurner to compile this
application. They can be acquired from the following locations:

- Uzebox: https://github.com/uzebox/uzebox
- XMBurner: https://github.com/jubatian/xmburner

You should first build Uzebox, although this is not necessary. You only need
the packrom tool from it to generate a .uze file, which the bootloader of
Uzebox can load onto the real hardware (without it, you will only receive a
.hex file).

Modify the Makefile according to the paths of these components:

- UZEBOX_DIR: Point it to the Uzebox repository.
- XMBURNER_DIR: Point it to the XMBurner repository.

Then you should be able to compile this application.
