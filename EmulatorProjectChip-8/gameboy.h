#pragma once
#ifndef GAMEBOY_H_

#include "stdint.h"
#include <iostream>
	
class Gameboy {
	//typedef int(Gameboy::*int_ptr)(int);
	typedef int (Gameboy::*int_ptr)();
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
			uint16_t sp; //stack pointer (current stack in use)

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
		unsigned char gfx[160 * 144];		//160x144 (20x18 tiles) display

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
		void Gameboy::add(const uint16_t & address, const uint8_t value);
		void Gameboy::subtract(const uint16_t & address, const uint8_t value);
		void Gameboy::ADD_SetFlag(uint8_t result);
		void Gameboy::SUB_SetFlag(uint8_t result);
		void Gameboy::AND_SetFlag(uint8_t result);
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
		
		void Gameboy::InitilizeOpCodeTable();

		int_ptr gbOpCodeTable[255];

		//?
		int NOP();
		//LD_r_r
		int LD_A_A();
		int LD_A_B();
		int LD_A_C();
		int LD_A_D();
		int LD_A_E();
		int LD_A_H();
		int LD_A_L();
		int LD_B_A();
		int LD_B_B();
		int LD_B_C();
		int LD_B_D();
		int LD_B_E();
		int LD_B_H();
		int LD_B_L();
		int LD_C_A();
		int LD_C_B();
		int LD_C_C();
		int LD_C_D();
		int LD_C_E();
		int LD_C_H();
		int LD_C_L();
		int LD_D_A();
		int LD_D_B();
		int LD_D_C();
		int LD_D_D();
		int LD_D_E();
		int LD_D_H();
		int LD_D_L();
		int LD_E_A();
		int LD_E_B();
		int LD_E_C();
		int LD_E_D();
		int LD_E_E();
		int LD_E_H();
		int LD_E_L();
		int LD_H_A();
		int LD_H_B();
		int LD_H_C();
		int LD_H_D();
		int LD_H_E();
		int LD_H_H();
		int LD_H_L();
		int LD_L_A();
		int LD_L_B();
		int LD_L_C();
		int LD_L_D();
		int LD_L_E();
		int LD_L_H();
		int LD_L_L();
		//LD_r_n
		int LD_A_n();
		int LD_B_n();
		int LD_C_n();
		int LD_D_n();
		int LD_E_n();
		int LD_H_n();
		int LD_L_n();
		//LD_r_(HL)
		int LD_A_HL();
		int LD_B_HL();
		int LD_C_HL();
		int LD_D_HL();
		int LD_E_HL();
		int LD_H_HL();
		int LD_L_HL();
		//LD_(HL)_r
		int LD_HL_A();
		int LD_HL_B();
		int LD_HL_C();
		int LD_HL_D();
		int LD_HL_E();
		int LD_HL_H();
		int LD_HL_L();
		int LD_HL_n();
		//misc A get-set
		int LD_nn_A();
		int LD_BC_A();
		int LD_DE_A();
		int LD_A_BC();
		int LD_A_DE();

		//LD_r_(nn)
		int LD_A_nn();

		//Custom NonZ80 opcodes
		int LDD_A_HL();
		int LDD_HL_A();
		int LDI_A_HL();
		int LDI_HL_A();
		int LDH_FF00_PLUS_n_A();
		int LDH_A_FF00_PLUS_n();
		int LD_A_FF00_PLUS_C();
		int LD_FF00_PLUS_C_A();

		//IndexPointer (Not implemented in the gameboy system architecture)
		int LD_I();
		int LD_r_IX_PLUS_d();
		int LD_r_IY_PLUS_d();

		//LD n, nn
		int LD_SP_HL();
		int LD_BC_nn();
		int LD_DE_nn();
		int LD_HL_nn();
		int LD_SP_nn();
		int LD_HL_SP_PLUS_n();
		int LD_nn_SP();

		//PUSH
		int PUSH_AF();
		int PUSH_BC();
		int PUSH_DE();
		int PUSH_HL();

		//POP
		int POP_AF();
		int POP_BC();
		int POP_DE();
		int POP_HL();

		//ADD
		int ADD_A_A();
		int ADD_A_B();
		int ADD_A_C();
		int ADD_A_D();
		int ADD_A_E();
		int ADD_A_H();
		int ADD_A_L();
		int ADD_A_HL();
		int ADD_A_n();

		//ADC
		int ADC_A_A();
		int ADC_A_B();
		int ADC_A_C();
		int ADC_A_D();
		int ADC_A_E();
		int ADC_A_H();
		int ADC_A_L();
		int ADC_A_HL();
		int ADC_A_n();

		//SUB
		int SUB_A_A();
		int SUB_A_B();
		int SUB_A_C();
		int SUB_A_D();
		int SUB_A_E();
		int SUB_A_H();
		int SUB_A_L();
		int SUB_A_HL();
		int SUB_A_n();

		//SBC
		int SBC_A_A();
		int SBC_A_B();
		int SBC_A_C();
		int SBC_A_D();
		int SBC_A_E();
		int SBC_A_H();
		int SCB_A_L();
		int SCB_A_HL();
		int SCB_A_n(); //? unknown

		//AND
		int AND_A_A();
		int AND_A_B();
		int AND_A_C();
		int AND_A_D();
		int AND_A_E();
		int AND_A_H();
		int AND_A_L();
		int AND_A_HL();
		int AND_A_n();

		//OR


		//XOR


		//CP


		//INC


		//DEC

	};

#endif // !GAMEBOY_H_