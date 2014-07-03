TGB Dual - libertyernie's version
https://github.com/libertyernie/tgbdual

TGB Dual is an open source (GPLv2) GB/GBC emulator with link cable support.
There are three frontends to the emulator core:
* Win32 - originally written by Hii in 2000-2004 (http://gigo.retrogames.com/download.html#tgb-dual)
  and updated by me (https://github.com/libertyernie/tgbdual)
* SDL - written in 2004 by shinichiro.h (http://shinh.skr.jp/tgbdualsdl)
* libretro - written in 2012 by lifning (https://github.com/libretro/tgbdual-libretro)

New features (in the Windows version only):
* All dialogs/menus have been translated to English at the source code level.
* All source code is now Unicode instead of Shift-JIS, which enables Japanese
  text to display on any computer.
* Along with the ROM and SRAM folders, users can now specify the SRAM file
  extensions used for Slot1 and Slot2 (the defaults are still .sav and .sa2.)
* TGB Dual can now load ROMs built with Goomba and Goomba Color (GB emulators
  for the GBA), and can read and write to those emulators' save files. This is
  helpful if you want to run a Goomba-based ROM on your PC, because the ROMs
  run faster in a GB emulator than in a GBA emulator trying to run Goomba.
  (Also, you can use the link cable emulation for games like Pokémon.)
  See goomba.txt for more information.

Bugfixes:
* The program now uses the DirectInput 8 API, not the old one which isn't
  available anymore.
* Turbo mode is now off by default.
* .sgb is now a supported file extension.

--------------------

Git branches:
* The "master" branch contains all the bugfixes, English-language dialogs
  and menus, and new features. This includes automatic support for Goomba ROMS
  (see docs/goomba.txt.)
* The "japanese" branch contains the above, but uses the original Japanese
  text (features that I added later are still in English.)
  
GIT NOTE:
All source code in the repo is now encoded in UTF-8; however, Windows uses
UTF-16, which Git doesn't recognize as text. I wrote a tiny program
(endian_conv) that runs before TGB_Dual is built and again when the project is
cleaned, to get the files encoded correctly. (When you make a Git commit, you
should first clean the project in Visual Studio.)
