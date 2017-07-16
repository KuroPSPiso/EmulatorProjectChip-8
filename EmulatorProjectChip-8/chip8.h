#pragma once
#ifndef CHIP8_H_

#include "chip8.h"
#include <io.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <Windows.h>
#include <errno.h>

class chip8 {
	#pragma region CPU registers and memory bus

	unsigned short opcode; //current opcode (2-bit)
	unsigned char memory[4096]; //4k Memory bus
	unsigned char V[16]; //cpu registers, 0-14 = general purpose registers, 15 = carry flag

	unsigned short I; //index register
	unsigned short pc; //program counter

	//not associated with Chip8 but needed to jump to the next adress/subroutine
	unsigned short stack[16]; //store current location before jump, up to 16 levels of stack
	unsigned short sp; //stack pointer (current stack in use)
	
	#pragma endregion CPU registers and memory bus

	#pragma region System memory map

	//TODO: fill

	#pragma endregion System memory map

	#pragma region Graphics

	unsigned char gfx[64 * 32]; //black and white (64 x 32) display
	HANDLE hdl_output;

	#pragma region Additional interupts / registers not part of the Chip8-chipset
	//Registers will have a 60hz refresh rate and will count back down to 0 once value is increased
	unsigned char delay_timer;
	unsigned char sound_timer;
	#pragma endregion Additional interupts / registers not part of the Chip8-chipset

	#pragma endregion Graphics

public:
	void chip8::initilize();
	void chip8::emulateCycle();
	bool chip8::loadApplication(const char * filename);
	void chip8::debugDisplay();

	#pragma region Input register
	unsigned char key[16]; //current key state
	#pragma endregion Input register

	#pragma region Graphics
	bool drawFlag; //tick to update screen
	#pragma endregion Graphics

private:
	void chip8::decodeOPCode();
};

#endif // !CHIP8_H_
