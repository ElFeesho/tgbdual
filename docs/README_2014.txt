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
* All text files are now Unicode instead of using the local code page, which
  should enable Japanese text to display on any computer.
  * However, Windows uses UTF-16 and Git uses UTF-8. So I wrote a tiny program
    (endian_conv) that runs before TGB_Dual is built and when it the project
    is cleaned, to get the files encoded correctly.

Original sources from
http://gigo.retrogames.com/download.html#tgb-dual
http://shinh.skr.jp/tgbdualsdl/
https://github.com/libretro/tgbdual-libretro
