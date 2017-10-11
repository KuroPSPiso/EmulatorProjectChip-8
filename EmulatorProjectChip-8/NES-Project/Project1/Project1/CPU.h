#pragma once
#ifndef NES_CPU_H_

#include "stdint.h"
#include <iostream>
#include "CartridgeLDR.h"
#include "type.h"

class CPU {

#define PAL_CLOCK "TO ADD CLOCKSPEED"
#define NTSC_CLOCK "To add clockspeed"

	typedef int (CPU::*int_ptr)();
public:
#pragma region Additional States & Modes - SIMPLIFIED

	/* 	The mirror state gets set after loading the cartridge:
	On(1) = Vertical
	Off(0) = Horizontal
	*/
	enum MirroringState {
		Vertical,
		Horizontal
	};

	/*	These help indicating the current state of the addressing-mode whilst reducing code redundancy.
	But this might have a negative effect because of an extra step.
	*/
	enum AddressingMode {
		Accumulator,
		Immediate,
		ZeroPage,
		ZeroPageX,
		ZeroPageY,
		Absolute,
		AbsoluteX,
		AbsoluteY,
		IndexedIndirectX,
		IndirectIndexedY,
		Indirect
	};

#pragma endregion Additional States & Modes - SIMPLIFIED

#pragma region CPU Registers and Memory

	//General purpose Registers
	union GeneralRegisters
	{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		struct
		{
			uint8_t A, X, Y;
		};
#else
		struct
		{
			uint8_t Y, X, A;
		};
#endif
	};

	//Processor status
	union P
	{
		struct
		{
			//FLAGS
			bool	C,	//carry					0
				Z,	//zero					1
				I,	//interupt disable		2
				D,	//decimal mode			3
				B,	//break command			4
				U,	//unused||empty state	5
				V,	//overflow				6
				N	//negative				7
				;
		};
	};

	//Based on data collected from cartridge
	union SystemBehaviour
	{
		struct
		{
			uint8_t numberOf16KRomBanks;//Size of PRG ROM in 16 KB units
			uint8_t numberOf8KVRomBanks;//Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
			uint8_t numberOf8KRamBanks;	//Size of PRG RAM in 8 KB units (Value 0 infers 8 KB for compatibility; see PRG RAM circuit)
			MirroringState Mirroring;	//1 for vertical mirroring, 0 for horizontal mirroring.
			bool BatteryBackedPRGRAM;	//1 for battery - backed RAM at $6000 - $7FFF.
			bool Trainer;				//1 for a 512 - byte trainer at $7000 - $71FF.
			bool FourScreenVRAM;		//1 for a four - screen VRAM layout. (Ignore mirroring control or above mirroring bit; instead provide four-screen VRAM)
			bool NESv2Format;
			bool VS;
			uint8_t Region1; 			//region 1 = PAL , all other should be NTCS
			uint16_t prgLocation; 		//CPU start
			int prgSize; 				//CPU space allocation
			uint16_t chrLocation; 		//PPU start
			int chrSize; 				//PPU space allocation
		};
	};

	//All registers and data combined.
	struct Registers
	{
		uint16_t pc;	//program counter, current position
						//uint8_t stack[0xFF]; //256 slots of capacity on the stack (Alternative stack solution instead of on the cartridge, if this stack gets used push/pull $00FF - $0000)
		uint16_t sp;	//stack pointer (current stack in use), the range of the stackpointer is $01FF - $0100
		P p;			//processor status - flags (Mostly reference as P or PS in documents)

		GeneralRegisters registry;
		SystemBehaviour systemBehaviour;
	};

	Registers cpuRegistry;			//All Registers
	uint16_t clockspeed;			//Cycles per full rotation
	uint16_t cycle;					//Current Cycle of machine (for draw states like VBlank)
	uint8_t opcode;					//Current Operation Code
	unsigned char memory[65536];	//The data you read of a single cartridge (64kb)
	unsigned char cartridge[65536];	//The data you read of a single cartridge (64kb)
	BOOL operable;					//Change if the cartridge does/does not load.

#pragma endregion CPU Registies and Memory


#pragma region Methods

	///<summary>
	///Set the System data based on data from the cartridge.
	///</summary>
	void CPU::ConfigureSystemBehaviour();
	///<summary>
	///After/during the configuration of the system behaviour you can set the clockspeed.
	///Use the region data as reference.
	///</summary>
	void CPU::ConfigureClockSpeed();
	void CPU::Boot();
	void CPU::RESET();

	void CPU::__MemADD(const uint16_t &address, const uint8_t value);
	void CPU::__MemSUB(const uint16_t &address, const uint8_t value);
	void CPU::__MemWrite(const uint16_t &address, const uint8_t value);
	const uint8_t CPU::__MemRead(const uint16_t &address);
	const uint16_t CPU::__MemRead16(const uint16_t &address);

	///<summary>
	///Read a 8bit value from memory on pos PC. Up PC by 1.
	///</summary>
	uint8_t CPU::fetch();
	///<summary>
	///Read a 16bit value from memory on pos PC & PC1. Up PC by 2.
	///</summary>
	uint16_t CPU::fetch16();

	///<summary>
	///Run a cycle.
	///</summary>
	void EmulateCycle();

	//constructor
	CPU(CartridgeLDR* cartridgeSlot);
#pragma endregion Methods

#pragma region OPCodes
	//TODO: OPcode-Table, Cycle-Table (lowest known value), OPcode methods
#pragma endregion OPCodes
};

#endif // !NES_CPU_H_