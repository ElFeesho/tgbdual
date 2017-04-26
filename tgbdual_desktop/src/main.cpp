/*--------------------------------------------------
   TGB Dual - Gameboy Emulator -
   Copyright (C) 2001  Hii

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifdef USE_SDL

#include <SDL/SDL.h>

#endif

#include <gameboy.h>

#include "tgbdual.h"

#include <linkcable/link_cable_source_provider.h>
#include <io/rom_file.h>
#include <script_commands.h>

int main(int argc, char *argv[]) {

    if (argc == 1) {
        std::cerr << "Usage: " << argv[0] << " rom [-s|-m|-c client-address]" << std::endl;
        return 0;
    }

    std::unique_ptr<link_cable_source> cable_source{provideLinkCableSource(&argc, &argv)};

    auto services = createCoreServices();

    rom_file rom{argv[0]};
    tgbdual tgb{services.get(), cable_source.get(), &rom};

    tgb.addConsoleCommand("quit", std::bind(&tgbdual::quit, &tgb));

    registerScriptCommands(tgb); // These commands use fs.

    while (tgb.limit());

    tgb.saveSram();

    return 0;
}

