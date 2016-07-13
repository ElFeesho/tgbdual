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

#include "cheat.h"
#include <ctype.h>
#include <string.h>
#include "gb.h"

cheat::cheat(gb *ref) {
    ref_gb = ref;   
}

void cheat::add_cheat(const std::string &code) {
    cheat_map.emplace(extractAddress(code), code);
    if (extractCode(code) == 0) {
        ref_gb->get_cpu()->write(extractAddress(code), extractData(code));
    }
}

uint8_t cheat::cheat_read(uint16_t adr) {
    if (cheat_map.find(adr) == cheat_map.end())
    {
        return ref_gb->get_cpu()->read_direct(adr);
    }
    else
    {
        const auto &tmp = cheat_map.at(adr);
        
        switch (tmp.code) {
            case 0x01:
                return tmp.dat;
            case 0x90:
            case 0x91:
            case 0x92:
            case 0x93:
            case 0x94:
            case 0x95:
            case 0x96:
            case 0x97:
                if ((adr >= 0xD000) && (adr < 0xE000)) {
                    if (((ref_gb->get_cpu()->get_ram_bank() - ref_gb->get_cpu()->get_ram()) / 0x1000) == (tmp.code - 0x90)) {
                        return tmp.dat;
                    }
                } else {
                    return tmp.dat;
                }
            default:
                break;
        }
    }

    return ref_gb->get_cpu()->read_direct(adr);
}