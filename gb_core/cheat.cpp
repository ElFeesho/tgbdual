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

void cheat::add_cheat(const std::string &code, cpu_writecb writecb) {
    cheat_map.emplace(extractAddress(code), code);
    if (extractCode(code) == 0) {
        writecb(extractAddress(code), extractData(code));
    }
}

uint8_t cheat::cheat_read(uint8_t ram_bank_num, uint16_t adr, uint16_t or_value) {
    if (cheat_map.find(adr) == cheat_map.end())
    {
        return or_value;
    }
    else
    {
        const auto &tmp = cheat_map.at(adr);
        if (tmp.code == 0x01)
        {
            return tmp.dat;
        }
        else if (tmp.code >= 0x90 && tmp.code <= 0x97)
        {
            if ((adr >= 0xD000) && (adr < 0xE000)) {
                if (ram_bank_num == (tmp.code - 0x90)) {
                    return tmp.dat;
                }
            } else {
                return tmp.dat;
            }
        }
    }

    return or_value;
}