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

//--------------------------------------------------
// キー名

static wchar_t keyboad_map[0x100][7]={
	L"",L"ESC",L"1",L"2",L"3",L"4",L"5",L"6",L"7",L"8",L"9",L"0",L"-",L"",L"BS",L"TAB",
	L"Q",L"W",L"E",L"R",L"T",L"Y",L"U",L"I",L"O",L"P",L"[",L"]",L"Enter",L"L-Ctrl",L"A",L"S",
	L"D",L"F",L"G",L"H",L"J",L"K",L"L",L";",L"",L"",L"L-Shft",L"\\",L"Z",L"X",L"C",L"V",
	L"B",L"N",L"M",L",L",L".",L"/",L"R-Shft",L"Num-*",L"L-Alt",L"Space",L"英数",L"F1",L"F2",L"F3",L"F4",L"F5",
	L"F6",L"F7",L"F8",L"F9",L"F10",L"Num-Lk",L"Scrl",L"Num-7",L"Num-8",L"Num-9",L"Num -",L"Num-4",L"Num-5",L"Num-6",L"Num +",L"Num-1",
	L"Num-2",L"Num-3",L"Num-0",L"Num .",L"",L"",L"",L"F11",L"F12",L"",L"",L"",L"",L"",L"",L"",
	L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
	L"かな",L"",L"",L"",L"",L"",L"",L"",L"",L"変換",L"",L"無変換",L"",L"¥",L"",L"",
	L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
	L"^",L"@",L":",L"",L"漢字",L"",L"",L"",L"",L"",L"",L"",L"NuEnt",L"R-Ctrl",L"",L"",
	L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
	L"",L"",L"",L"",L"",L"Num /",L"",L"PrScr",L"R-Alt",L"",L"",L"",L"",L"",L"",L"",
	L"",L"",L"",L"",L"",L"",L"",L"Home",L"↑",L"Pg-up",L"",L"←",L"",L"→",L"",L"End",
	L"↓",L"Pg-dn",L"INS",L"DEL",L"",L"",L"",L"",L"",L"",L"",L"L-Win",L"R-Win",L"R-Menu",L"",L"",
	L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
	L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L""
};

