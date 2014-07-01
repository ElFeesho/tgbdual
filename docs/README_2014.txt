TGB Dual - libertyernie's version
https://github.com/libertyernie/tgbdual

This is an update of TGB Dual designed primarily to get it compiling and
working correctly on new computers.

Bugfixes:
* The program now uses the DirectInput 8 API, not the old one which isn't
  available anymore.
* Turbo mode is now off by default.
* .sgb is now a supported file extension.

New features:
* Along with the ROM and SRAM folders, users can now specify the SRAM file
  extensions used for Slot1 and Slot2 (the defaults are still .sav and .sa2.)
* TGB Dual can now load ROMs built with Goomba and Goomba Color (GB emulators
  for the GBA), and can read and write to those emulators' save files. See
  goomba.txt for more information.
* All source code is now Unicode instead of Shift-JIS, which enables Japanese
  text to display on any computer.

Original sources from
http://gigo.retrogames.com/download.html#tgb-dual
http://shinh.skr.jp/tgbdualsdl/
https://github.com/libretro/tgbdual-libretro
