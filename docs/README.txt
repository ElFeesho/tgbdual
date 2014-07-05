TGB Dual - libertyernie's version
https://github.com/libertyernie/tgbdual

TGB Dual is an open source (GPLv2) GB/GBC emulator with link cable support.
There are three frontends to the emulator core:
* Win32 - originally written by Hii in 2000-2004 (http://gigo.retrogames.com/download.html#tgb-dual) and updated by me (https://github.com/libertyernie/tgbdual)
* SDL - written in 2004 by shinichiro.h (http://shinh.skr.jp/tgbdualsdl)
  (Note: The SDL port can only emulate one Game Boy at a time.)
* libretro - written in 2012 by lifning (https://github.com/libretro/tgbdual-libretro)

New features:
* Windows version only:
  * All dialogs/menus have been translated to English at the source code level.
  * All source code is now Unicode instead of Shift-JIS, which enables Japanese
    text to display on any computer.
  * Along with the ROM and SRAM folders, users can now specify the SRAM file
    extensions used for Slot1 and Slot2 (the defaults are still .sav and .sa2.)
* Windows and SDL versions:
  * TGB Dual can now load ROMs built with Goomba and Goomba Color (GB emulators
    for the GBA), and can read and write to those emulators' save files. This
    is helpful if you want to run a Goomba-based ROM on your PC, because the
    ROMs run faster in a GB emulator than in a GBA emulator trying to run
	Goomba. (Also, you can use link cable emulation for games like Pokémon.)
    See goomba.txt for more information.
* Goomba support is not implemented in libretro - RetroArch uses direct access
  to a memory pointer to load/save SRAM, and I'm not that interested in coming
  up with a workaround.

Bugfixes (Windows version):
* The program now uses the DirectInput 8 API, not the old one which isn't
  available anymore.
* Turbo mode is now off by default.
* .sgb is now a supported file extension.
