TGB Dual - libertyernie's version
https://github.com/libertyernie/tgbdual

This is an update of TGB Dual designed primarily to get it compiling and
working correctly on new computers.

Changes made:
* The program now uses the DirectInput 8 API, not the old one which isn't
  available anymore.
* Minor bugfix: turbo mode is now off by default.
* All text files are now Unicode instead of using the local code page, which
  should enable Japanese text to display on any computer.
  * However, Windows uses UTF-16 and Git uses UTF-8. So I wrote a tiny program
    (endian_conv) that runs before and after TGB_Dual is built, and on project
    clean, so get the files encoded correctly.
  * I'm still working on changing Windows API dialogs to use UTF-16 strings,
    instead of UTF-8.

Additions:
* The "english" branch in Git contains an English-language version of the app.
  This is also still a work in progess. Someday, this may be merged into the
  main branch and a "japanese" branch might be created.

Original sources from
http://gigo.retrogames.com/download.html#tgb-dual
http://shinh.skr.jp/tgbdualsdl/
https://github.com/libretro/tgbdual-libretro
