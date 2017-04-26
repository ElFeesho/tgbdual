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
#include "gb.h"


static unsigned int fromHexToInt(const std::string &hexString) {
    unsigned int val;
    std::stringstream ss;
    ss << std::hex << hexString;
    ss >> val;
    return val;
}

static uint16_t extractAddress(const std::string &cheatString)
{
    uint16_t adr = (uint16_t) fromHexToInt(cheatString.substr(4));
    adr = (uint16_t) (((adr << 8) & 0xff00) | ((adr >> 8) & 0x00ff));
    return adr;
}

static uint8_t extractData(const std::string &cheatString)
{
    return (uint8_t) fromHexToInt(cheatString.substr(2, 2));
}

static uint8_t extractCode(const std::string &cheatString)
{
    return (uint8_t) fromHexToInt(cheatString.substr(0, 2));
}

cheat_dat::cheat_dat(const std::string &cheat_code) {
    code = extractCode(cheat_code);
    dat = extractData(cheat_code);
    adr = extractAddress(cheat_code);
}

void cheat::add_cheat(const std::string &code, const cpu_writecb &writecb) {
    cheat_map.emplace(extractAddress(code), code);
    if (extractCode(code) == 0) {
        writecb(extractAddress(code), extractData(code));
    }
}

uint8_t cheat::cheat_read(uint8_t ram_bank_num, uint16_t adr, uint8_t or_value) {
    if (cheat_map.find(adr) == cheat_map.end())
    {
        return or_value;
    }
    
    const auto &tmp = cheat_map.at(adr);
    if (tmp.code == 0x01)
    {
        return tmp.dat;
    }

    if (tmp.code >= 0x90 && tmp.code <= 0x97)
    {
        if ((adr >= 0xD000) && (adr < 0xE000)) {
            if (ram_bank_num == (tmp.code - 0x90)) {
                return tmp.dat;
            }
        } else {
            return tmp.dat;
        }
    }


    return or_value;
}