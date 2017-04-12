#pragma once

#include <io/rom_file.h>
#include <console/Console.h>
#include <gameboy.h>

void registerGameBoyCommands(Console &console, gameboy &gbInst, rom_file &romFile);