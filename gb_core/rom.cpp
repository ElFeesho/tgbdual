/*--------------------------------------------------
   TGB Dual - Gameboy Emulator -
   Copyright (C) 2001  Hii, 2014 libertyernie

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

//-----------------------------------------------
// ROMイメージ管理部 (含SRAM) // ROM image management unit (SRAM included)

#include "gb.h"
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>
#include "../goomba/goombarom.h"
#include "../goomba/goombasav.h"

std::list<byte*> rom::goomba_srams;

rom::rom()
{
	b_loaded=false;

	dat=NULL;
	sram=NULL;
	goomba_sram = NULL;
}

rom::~rom()
{
	free(dat);
	free(sram);
}

bool rom::has_battery()
{
	static const int has_bat[]=
		{0,0,0,1,0, 0,1,0,0,1,
		 0,0,1,1,0, 1,1,0,0,1,
		 0,0,0,0,0, 0,0,1,0,1,
		 1,0, 0,0,0,0,0,0,0,0}; // 0x20以下
	return has_bat[(info.cart_type>0x20)?3:info.cart_type]==1;
}

int rom::get_sram_size()
{
	static const int tbl_ram[]={1,1,1,4,16,8};//0と1は保険
	return 0x2000*tbl_ram[info.ram_size];
}

byte *rom::get_goomba_sram()
{
	if (goomba_sram != NULL) {
		void* new_sram = goomba_new_sav(goomba_sram, stateheader_for(goomba_sram, info.cart_name), sram, get_sram_size());
		memcpy(goomba_sram, new_sram, GOOMBA_COLOR_SRAM_SIZE);
		free(new_sram);
		return goomba_sram;
	} else {
		return sram;
	}
}

int rom::get_goomba_sram_size()
{
	return goomba_sram != NULL ? GOOMBA_COLOR_SRAM_SIZE : get_sram_size();
}

bool rom::load_rom(byte *buf,int size,byte *ram,int ram_size)
{
	int tbl[]={2,4,8,16,32,64,128,256,512};
	int has_bat[]={0,0,0,1,0,0,1,0,0,1,0,0,1,1,0,1,1,0,0,1,0,0,0,0,0,0,0,1,0,1,1,0};//0x20以下

	byte momocol_title[16]={0x4D,0x4F,0x4D,0x4F,0x43,0x4F,0x4C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	if (b_loaded){
		free(dat);
		free(sram);
		if (goomba_sram != NULL) {
			// using the list as a miniature garbage detector
			std::list<byte*>::iterator first_entry = std::find(goomba_srams.begin(), goomba_srams.end(), goomba_sram);
			if (first_entry != goomba_srams.end()) {
				goomba_srams.erase(first_entry);
			} else {
				throw new std::exception("programming error");
			}
			std::list<byte*>::iterator second_entry = std::find(goomba_srams.begin(), goomba_srams.end(), goomba_sram);
			if (second_entry == goomba_srams.end()) {
				// no other GB is using this sram
				free(goomba_sram);
			}
			goomba_sram = NULL;
		}
	}

	// If the file contains multiple ROMs, find the first one.
	// If not, pretend it's at the start of the file (fake Nintendo logo?)
	const void* first_rom = gb_first_rom(buf, size);
	if (first_rom != NULL && first_rom != buf) {
		buf = (byte*)first_rom;
		size = gb_rom_size(first_rom);
	}

	memcpy(info.cart_name,buf+0x134,16);
	info.cart_name[16]='\0';
	info.cart_name[17]='\0';
	info.cart_type=buf[0x147];
	info.rom_size=buf[0x148];
	info.ram_size=buf[0x149];

	void* extracted = NULL;
	goomba_size_t extracted_size;
	if (ram_size == GOOMBA_COLOR_SRAM_SIZE && goomba_is_sram(ram)) {
		void* cleaned = goomba_cleanup(ram);
		if (cleaned == NULL) return false;

		for (std::list<byte*>::iterator it = goomba_srams.begin(); it != goomba_srams.end(); it++) {
			if (memcmp(*it, cleaned, GOOMBA_COLOR_SRAM_SIZE) == 0) {
				// Same Goomba SRAM file is already loaded in the other GB - share the stored copy in memory
				goomba_sram = *it;
				break;
			}
		}
		if (goomba_sram == NULL) { // still
			goomba_sram = (byte*)malloc(GOOMBA_COLOR_SRAM_SIZE);
			memcpy(goomba_sram, cleaned, GOOMBA_COLOR_SRAM_SIZE);
		}
		if (cleaned != ram) free(cleaned);

		goomba_srams.push_back(goomba_sram);

		stateheader* sh = stateheader_for(goomba_sram, info.cart_name);
		if (sh == NULL) return false;
		extracted = goomba_extract(goomba_sram, sh, &extracted_size);
		if (extracted == NULL) return false;
	}

	if (memcmp(info.cart_name,momocol_title,16)==0){
		info.cart_type=0x100;//mmm01
	}

	word tmp=(buf[0x14E]<<8)|buf[0x14F];
	byte tmp2=buf[0x143];

	info.gb_type=(tmp2&0x80)?3:1;

	if (info.rom_size>8)
		return false;

	dat=(byte*)malloc(size);
	memcpy(dat,buf,size);
	first_page=dat;

	word sum=0;

	sram=(byte*)malloc(get_sram_size());
	if (extracted) {
		memcpy(sram, extracted, extracted_size);
		free(extracted);
	} else if (ram) {
		memcpy(sram,ram,ram_size&0xffffff00);
	}

	b_loaded=true;

	return true;
}

void rom::serialize(serializer &s)
{
	s_VAR(info);
	s.process(sram, get_sram_size());
}

