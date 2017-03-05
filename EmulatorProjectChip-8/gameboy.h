#pragma once
#ifndef GAMEBOY_H_

#include "stdint.h"
#include <iostream>

class Gameboy {
public:
#pragma region CPU registers and memory bus

	uint8_t opcode;							//current opcode (2-bit)
	unsigned char memory[65536];			//64k memory << 16k rombank (part of cartrigde) #0000, 16k switchable rombank (part of cartrigde) #4000, 8k video ram #8000, 8k switchable memory bus (ram) #A000, 8k internal Memory bus (ram) #C000, 7k echo of 8k ram (internal) #E000, 160b sprite attribute memory #FE00, 96b empty #FEA0, 252b i/o ports #FF00, 52b empty #FF4C, 126b internal ram #FF80, 1b interrupt enable register #FFFF
	
	//unsigned char V[16];					//cpu registers, 0-14 = general purpose registers, 15 = carry flag

	//Flag register GB
	uint8_t flag_Z = 0x10000000;	//zero flag
	uint8_t flag_N = 0x01000000;	//zero flag
	uint8_t flag_H = 0x00100000;	//(add/)subtract
	uint8_t flag_C = 0x00010000;	//carry flag

	//unused flag register Z80 interp
	uint8_t flag_Z80_S = 0x10000000;	//sign flag
	uint8_t flag_Z80_Z = 0x01000000;	//zero flag
	uint8_t flag_Z80_H = 0x00010000;	//half carry flag
	uint8_t flag_Z80_PV = 0x00000100;	//Parity/Overflow flag
	uint8_t flag_Z80_N = 0x00000010;	//Add/Subtract
	uint8_t flag_Z80_C = 0x00000000;	//Carry flag


	union GeneralRegisters
	{
		struct
		{
			uint16_t AF,	//Accumulator, Flag register
				BC,			//16-bit pair
				DE,			//16-bit pair
				HL;			//16-bit pair
		};
		#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		struct
		{
			uint8_t F, A, C, B, E, D, L, H;
		};
		#else
		struct 
		{
			uint8_t A, F, B, C, D, E, H, L;
		};
		#endif
	};

	struct Index
	{
		struct
		{
			uint16_t IX,	//Index register 1
				IY;			//1ndex register 2
		};
		#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		struct
		{
			uint8_t IXl, IXh, IYl, IYh;
		};
		#else
		struct
		{
			uint8_t IXh, IXl, IYh, IYl;
		};
		#endif
	};
	
	struct Registers
	{
		uint16_t	pc;	//program counter
		//uint8_t stack[16]; //store current location before jump, up to 16 levels of stack
		uint8_t sp; //stack pointer (current stack in use)

		GeneralRegisters registery;
		GeneralRegisters altRegistery;

		Index index;
	};

	Registers cpuRegistery;

	#pragma region Pin Functions
	uint16_t AddressBus;
	bool BUSACK;
	bool HALT;
	bool NMI;

	#pragma endregion Pin Functions

#pragma endregion CPU registers and memory bus

#pragma region Graphics
	unsigned char bgGFX[256 * 256];	//virtual display
	unsigned char gfx[64 * 32];		//160x144 (20x18 tiles) display

#pragma region Additional interupts / registers not part of the Chip8-chipset
								//Registers will have a 60hz refresh rate and will count back down to 0 once value is increased
	unsigned char delay_timer;
	unsigned char sound_timer;
#pragma endregion Additional interupts / registers not part of the Chip8-chipset

#pragma endregion Graphics
	void Gameboy::emulateCycle();
	void Gameboy::RESET();

	bool Gameboy::loadApplication(const char * filename);
	void Gameboy::debugDisplay();
	const uint8_t& Gameboy::read(const uint16_t &address);
	void Gameboy::write(const uint16_t &address, const uint8_t value);
#pragma region Input register
	unsigned char key[16]; //current key state
#pragma endregion Input register

#pragma region Graphics
	bool drawFlag; //tick to update screen
#pragma endregion Graphics

private:
	uint8_t Gameboy::fetch();
	uint8_t Gameboy::decodeOPCode(const uint8_t &opcode);
	uint8_t Gameboy::decodeOPCodeI__d(const uint8_t &opcode, uint16_t index); //IX+d / IY+d
	uint8_t Gameboy::PUSH(const uint16_t &qq);
};

#endif // !CHIP8_H_
