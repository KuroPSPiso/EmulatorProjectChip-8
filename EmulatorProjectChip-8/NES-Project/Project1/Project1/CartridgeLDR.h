#pragma once
#ifndef NES_CLDR_H_

#include <iostream>
#include <direct.h> 
#include "type.h"

class CartridgeLDR
{
	unsigned char cartridge[65536]; //rom to load
public:
	
	CartridgeLDR();
	void Dispose();

	/// <summary>Data from the cartridge gets loaded into memory.
	///	<para>Output reference to memory slot</para>
	/// </summary>
	void LoadIntoCartridgeSlot(unsigned char* memory);

	/// <summary>Load a rom into a temporary location. (this is necessary to properly load the cpu as well as the memory bank(s))
	/// <para>Path to ROM file</para>
	/// <returns>If the cartridge is valid and loaded correctly it will return 1, else 0</returns>
	/// </summary>
	int LoadCartridge(char* path);

};


#endif // !NES_CLDR_H_