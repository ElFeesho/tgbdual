#pragma once

#include <RomFile.h>
#include <console/Console.h>
#include <gameboy.h>


void registerGameBoyCommands(Console &console, gameboy &gbInst, RomFile &romFile);