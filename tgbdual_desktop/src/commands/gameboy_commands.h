#pragma once

#include <io/rom_file.h>
#include <console/console.h>
#include <gameboy.h>

void registerGameBoyCommands(console &console, gameboy &gbInst, rom_file &romFile);