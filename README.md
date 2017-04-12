# TGBDual ![Build status](https://travis-ci.org/ElFeesho/tgbdual.svg?branch=master)
## About this fork
This fork originally started out as an investigation into how much I could refactor the original project but has since grown far beyond this.

Checkout the [Wiki](https://github.com/ElFeesho/tgbdual/wiki) for more information on new features and functionality.

### New Features
The following is an incomplete list of new features added:
 * Link cable support (buggy/broken)
   * Ability to advertise a session (host)
   * Ability to join a session
   * Ability to use some link cable functionality (pokemon trades seem to work the first time in a session)
 * OSD
   * Notify when state has been saved, when state has been loaded etc
 * Console
   * In game console can be used to modify game state
     * Search for values in memory
     * Write values in memory
     * Relative search for values (greater, lesser, unchanged, changed)
     * Tab completion on commands (very basic)
 * Script VM support to augment game functionality and add console commands
   * Lua support
   * [Wren](https://github.com/munificent/wren) support
 
### Missing Features
I removed several features that were too difficult to maintain without a full understanding as to how they worked. The following is an incomplete list of removed features.

 * Configuration
   * There is no way to customise controls anymore
 * Super Gameboy support may be broken or missing
 * No turbo button ability
 
### Upcoming Features
In no priority order:
 * Android application 
 * [ ] More unit test coverage
 * [ ] Fixed link cable support
 * [x] Tab completion on console commands that have come from scripts
 * [ ] Enhanced debugging capabilities
   * [ ] Graphic memory introspection
   * [ ] Disassembly output (with stepping and breakpoints)
   
# Original Project
The original [TGBDual repository can be found here](https://github.com/libertyernie/tgbdual).
