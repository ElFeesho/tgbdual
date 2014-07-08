TGB Dual - libertyernie's version
https://github.com/libertyernie/tgbdual

TGB Dual (originally written by Hii in 2000–2004) is an open-source Game Boy
Color emulator with link cable support.

TGB Dual makes it possible to emulate two Game Boy consoles at once (Slot 1
and Slot 2), using two different ROMs and save files. Opening a ROM in Slot 2
will open a new window. The two slots have different button mappings; you can
check or change them with Options > Keys.

Note: I have not checked whether network play (File > Netplay) works in this
version. If it doesn't, download the original version at:
http://gigo.retrogames.com/download.html#tgb-dual

Besides Win32, there are two other frontends to the emulator core:
* SDL - written in 2004 by shinichiro.h (http://shinh.skr.jp/tgbdualsdl) - no link cable support
* libretro - written in 2012 by lifning (https://github.com/libretro/tgbdual-libretro)

New features:
* Windows and SDL versions:
  * TGB Dual can now load ROMs built with Goomba and Goomba Color (GB emulators
    for the GBA), and can read and write to those emulators' save files. This
    is helpful if you want to share your save data between PC and GBA, because
    the ROMs run faster in a GB emulator than they would in a GBA emulator
    trying to run Goomba. (Also, you can use link cable emulation for games
    like Pokémon.) See goomba.txt for more information.
	* Because RetroArch loads/saves SRAM directly to a memory address instead
	  of calling a function, implementing this feature in the libretro port
	  would be more difficult.
  * Supported file extensions added: .gba (for Goomba/Goomba Color) and .sgb.
* Windows version only:
  * All dialogs/menus have been translated to English at the source code level.
  * Along with the ROM and SRAM folders, users can now specify the SRAM file
    extensions used for Slot1 and Slot2 (the defaults are still .sav and .sa2.)

Bugfixes:
* Windows version:
  * All source code is now Unicode instead of Shift-JIS, which enables Japanese
    text to display on any computer.
  * The program now uses the DirectInput 8 API, not the old one which isn't
    available anymore.
  * Turbo mode is now off by default.
* SDL version:
  * The program will no longer crash when checking file extentions.

Note: the SDL port saves SRAM data to ~/.tgbdual/save, and (unless it's Goomba
data) compresses it with gzip. To use Goomba save files stored somewhere else,
symbolic links are probably the way to go.
