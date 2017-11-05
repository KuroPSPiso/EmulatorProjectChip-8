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
			uint8_t mapper;				//Mapper NR.
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

	int_ptr opCodeTable[255]; //opcode list
	char* opCodeNameTable[255]; //Not Necessary, for testing purposes only.
	uint8_t opCodeCycleTable[255]; //minimal cycles
	void InitialiseOpCodes();

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

#pragma region Storage
	//Load A with M
	int CPU::LDAI();
	int CPU::LDAZP();
	int CPU::LDAZPX();
	int CPU::LDAA();
	int CPU::LDAAX();
	int CPU::LDAAY();
	int CPU::LDAIX();
	int CPU::LDAIY();

	//Load X with M
	int CPU::LDXI();
	int CPU::LDXZP();
	int CPU::LDXZPY();
	int CPU::LDXA();
	int CPU::LDXAY();


	//Load Y with M
	int CPU::LDYI();
	int CPU::LDYZP();
	int CPU::LDYZPX();
	int CPU::LDYA();
	int CPU::LDYAX();


	//Store A in M
	int CPU::STAZP();
	int CPU::STAZPX();
	int CPU::STAA();
	int CPU::STAAX();
	int CPU::STAAY();
	int CPU::STAIX();
	int CPU::STAIY();

	//Store X in M
	int CPU::STXZP();
	int CPU::STXZPY();
	int CPU::STXA();

	//Store Y in M
	int CPU::STYZP();
	int CPU::STYZPX();
	int CPU::STYA();

	int CPU::TAX(); //Transfer A to X											--Implied
	int CPU::TAY(); //Transfer A to Y											--Implied
	int CPU::TSX(); //Transfer Stack Pointer to X								--Implied
	int CPU::TXA(); //Transfer X to A											--Implied
	int CPU::TXS(); //Transfer X to Stack Pointer								--Implied
	int CPU::TYA(); //Transfer Y to A											--Implied
#pragma endregion Storage


#pragma region Math
					//Add M to A with Carry
	int CPU::ADCI();
	int CPU::ADCZP();
	int CPU::ADCZPX();
	int CPU::ADCA();
	int CPU::ADCAX();
	int CPU::ADCAY();
	int CPU::ADCIX();
	int CPU::ADCIY();

	//Decrement M by One
	int CPU::DECZP();
	int CPU::DECZPX();
	int CPU::DECA();
	int CPU::DECAX();

	int CPU::DEX(); //Decrement X by One											--Implied
	int CPU::DEY(); //Decrement Y by One											--Implied

					//Increment M by One
	int CPU::INCZP();
	int CPU::INCZPX();
	int CPU::INCA();
	int CPU::INCAX();

	int CPU::INX(); //Increment X by One											--Implied
	int CPU::INY(); //Increment Y by One											--Implied

					//Subtract M from A with Borrow
	int CPU::SBCI();
	int CPU::SBCZP();
	int CPU::SBCZPX();
	int CPU::SBCA();
	int CPU::SBCAX();
	int CPU::SBCAY();
	int CPU::SBCIX();
	int CPU::SBCIY();

#pragma endregion Math


#pragma region Bitwise
	//"AND" M with A
	int CPU::ANDI();
	int CPU::ANDZP();
	int CPU::ANDZPX();
	int CPU::ANDA();
	int CPU::ANDAX();
	int CPU::ANDAY();
	int CPU::ANDIX();
	int CPU::ANDIY();

	//Shift Left One Bit(M or A)
	int CPU::ASLACC();
	int CPU::ASLZP();
	int CPU::ASLZPX();
	int CPU::ASLA();
	int CPU::ASLAX();

	//Test Bits in M with A
	int CPU::BITZP();
	int CPU::BITA();

	//"Exclusive-Or" M with A
	int CPU::EORI();
	int CPU::EORZP();
	int CPU::EORZPX();
	int CPU::EORA();
	int CPU::EORAX();
	int CPU::EORAY();
	int CPU::EORIX();
	int CPU::EORIY();

	//Shift Right One Bit(M or A)
	int CPU::LSRACC();
	int CPU::LSRZP();
	int CPU::LSRZPX();
	int CPU::LSRA();
	int CPU::LSRAX();

	//"OR" M with A
	int CPU::ORAI();
	int CPU::ORAZP();
	int CPU::ORAZPX();
	int CPU::ORAA();
	int CPU::ORAAX();
	int CPU::ORAAY();
	int CPU::ORAIX();
	int CPU::ORAIY();

	//Rotate One Bit Left(M or A)
	int CPU::ROLACC();
	int CPU::ROLZP();
	int CPU::ROLZPX();
	int CPU::ROLA();
	int CPU::ROLAX();

	//Rotate One Bit Right(M or A)
	int CPU::RORACC();
	int CPU::RORZP();
	int CPU::RORZPX();
	int CPU::RORA();
	int CPU::RORAX();

#pragma endregion Bitwise


#pragma region Branch
	int CPU::BCC(); //Branch on Carry Clear										--RELATIVE
	int CPU::BCS(); //Branch on Carry Set										--RELATIVe
	int CPU::BEQ(); //Branch on Result Zero										--RELATIVE
	int CPU::BMI(); //Branch on Result Minus										--RELATIVE
	int CPU::BNE(); //Branch on Result not Zero									--RELATIVE
	int CPU::BPL(); //Branch on Result Plus										--RELATIVE
	int CPU::BVC(); //Branch on Overflow Clear									--RELATIVE
	int CPU::BVS(); //Branch on Overflow Set										--RELATIVE
#pragma endregion Branch


#pragma region Jump
					//Jump to Location
	int CPU::JMP(const AddressingMode &mode);
	int CPU::JMPA();
	int CPU::JMPI();

	int CPU::JSR(); //Jump to Location Save Return Address						--ABSOLUTE
	int CPU::RTI(); //Return from Interrupt										--Implied
	int CPU::RTS(); //Return from Subroutine										--Implied
#pragma endregion Jump


#pragma region Registers
	int CPU::CLC(); //Clear Carry Flag											--Implied
	int CPU::CLD(); //Clear Decimal Mode											--Implied
	int CPU::CLI(); //Clear interrupt Disable Bit								--Implied
	int CPU::CLV(); //Clear Overflow Flag										--Implied

	int CPU::CMP_ALL(const AddressingMode &mode, char reg); //COMPARE ALL -- Unofficial

															//Compare M AND A
	int CPU::CMPI();
	int CPU::CMPZP();
	int CPU::CMPZPX();
	int CPU::CMPA();
	int CPU::CMPAX();
	int CPU::CMPAY();
	int CPU::CMPIX();
	int CPU::CMPIY();

	//Compare M and X
	int CPU::CPXI();
	int CPU::CPXZP();
	int CPU::CPXA();

	//Compare M and Y
	int CPU::CPYI();
	int CPU::CPYZP();
	int CPU::CPYA();

	int CPU::SEC(); //Set Carry Flag												--Implied
	int CPU::SED(); //Set Decimal Mode											--Implied
	int CPU::SEI(); //Set Interrupt Disable Status								--Implied
	int CPU::SEV(); //Set overflow --UNOFFICIAL
#pragma endregion Registers


#pragma region Stack
	int CPU::PHA(); //Push A on Stack											--Implied
	int CPU::PHP(); //Push Processor Status on Stack								--Implied
	int CPU::PLA(); //Pull A from Stack											--Implied
	int CPU::PLP(); //Pull Processor Status from Stack							--Implied
#pragma endregion Stack


#pragma region Interrupts
	int CPU::NOP();		//No Operation											--Implied
	int CPU::NMI();		//Non-Maskable Intteruption
	int CPU::Reset();	//Reset
	int CPU::IRQ();		//Interrupt Request
	int CPU::BRK();		//Break													--Implied
#pragma endregion


#pragma region Unofficial/Illegal
						//Do not integrate (now)
	int CPU::KILL();
	int CPU::SLO();
	int CPU::ASL();
	int CPU::ANC();
	int CPU::RLA();
	int CPU::RRA();
	int CPU::ALR();
	int CPU::ARR();
	int CPU::XAA();
	int CPU::LAX();
	int CPU::AXS();
	int CPU::DCP();
	int CPU::SAX();
	int CPU::SRE();
	int CPU::ISC();
	int CPU::SHX();
	int CPU::SHY();
#pragma endregion Unofficial/Illegal

#pragma region Custom
	//Do not integrate (now)
	void CPU::SET_ZN(const char &value);
	void CPU::SEN(const char &value); //set negative
	void CPU::SEN(); //set negative
	void CPU::CLN(); //clear negative
	void CPU::SEZ(const char &value); //set zero
#pragma endregion Custom

#pragma endregion OPCodes
};

#endif // !NES_CPU_H_