#pragma once
#ifndef NES_H_

#include "stdint.h"
#include <iostream>

class NES {
	typedef int (NES::*int_ptr)();
public:
#pragma region CPU registers and memory bus

	uint8_t opcode;							//current opcode (2-bit)
	unsigned char memory[65536];			//64k memory << 16k rombank (part of cartrigde) #0000, 16k switchable rombank (part of cartrigde) #4000, 8k video ram #8000, 8k switchable memory bus (ram) #A000, 8k internal Memory bus (ram) #C000, 7k echo of 8k ram (internal) #E000, 160b sprite attribute memory #FE00, 96b empty #FEA0, 252b i/o ports #FF00, 52b empty #FF4C, 126b internal ram #FF80, 1b interrupt enable register #FFFF
	unsigned char cartridge[65536];			//TODO: temp load
											//unsigned char V[16];					//cpu registers, 0-14 = general purpose registers, 15 = carry flag

											//Flag register GB

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

	/*On = Vertical, Off = Horizontal*/
	enum MirroringState {
		Vertical,
		Horizontal
	};

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

	union P //Processor status
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

	union SystemBehaviour //Based on data collected from cartridge
	{
		struct
		{
			uint8_t numberOf16KRomBanks; //Size of PRG ROM in 16 KB units
			uint8_t numberOf8KVRomBanks; //Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
			uint8_t numberOf8KRamBanks;	//Size of PRG RAM in 8 KB units (Value 0 infers 8 KB for compatibility; see PRG RAM circuit)
			MirroringState Mirroring;//1 for vertical mirroring, 0 for horizontal mirroring.
			bool BatteryBackedPRGRAM;//1 for battery - backed RAM at $6000 - $7FFF.
			bool Trainer;//1 for a 512 - byte trainer at $7000 - $71FF.
			bool FourScreenVRAM;//1 for a four - screen VRAM layout. (Ignore mirroring control or above mirroring bit; instead provide four-screen VRAM)
			bool NESv2Format;
			bool VS;
			bool Region1; //region 1 = PAL , all other should be NTCS
			uint16_t prgLocation; //CPU start
			int prgSize; //CPU space allocation
			uint16_t chrLocation; //PPU start
			int chrSize; //PPU space allocation
			int Mapper; //iNES Mapper
		};
	};

	struct Registers
	{
		uint16_t	pc;	//program counter
						//uint8_t stack[16]; //store current location before jump, up to 16 levels of stack
		uint16_t sp;	//stack pointer (current stack in use)
		P p;			//processor status - flags

		GeneralRegisters registry;
		SystemBehaviour systemBehaviour;

	};

	bool NMI_ENABLED, NMI_EDGE_DETECTED, INTERUPT = false;

	Registers cpuRegistry;

#pragma region Pin Functions
	uint16_t AddressBus;

#pragma endregion Pin Functions

#pragma endregion CPU registers and memory bus

#pragma region Graphics
	unsigned char bgGFX[256 * 256];	//virtual display
	unsigned char gfx[256 * 240];		//256x240 (32x30 tiles of 8x8 pixels) display

#pragma region Additional interupts / registers not part of the Chip8-chipset
									//Registers will have a 60hz refresh rate and will count back down to 0 once value is increased
	unsigned char delay_timer;
	unsigned char sound_timer;
#pragma endregion Additional interupts / registers not part of the Chip8-chipset

#pragma region Cycle Handling
	void NES::emulateCycle();
	void NES::RESET();
	
	int NES::powerUpSequence();
	bool NES::loadApplication(const char * filename);
	void NES::debugDisplay();
	const uint8_t& NES::read(const uint16_t &address);
	void NES::write(const uint16_t &address, const uint8_t value);
	void NES::add(const uint16_t & address, const uint8_t value);
	void NES::subtract(const uint16_t & address, const uint8_t value);
#pragma region Input register
	unsigned char key[16]; //current key state
#pragma endregion Input register

	bool drawFlag; //tick to update screen
#pragma endregion Cycle Handling

private:
	uint8_t NES::fetch();
	uint16_t NES::fetch16();
	//uint16_t NES::fetch16IX();
	//uint16_t NES::fetch16IY();
	//uint16_t NES::fetch16ZX();
	//uint16_t NES::fetch16ZY();
	//uint16_t NES::fetch16AX();

	//fetches data
	uint8_t NES::getImmeditate_u8();
	//fetches address space
	uint8_t NES::getZeroPage_u8();
	//fetches address space
	uint8_t NES::getZeroPage_u8(char registry);
	//fetches address space
	uint16_t NES::getAbsolute_u16();
	//fetches address space
	uint16_t NES::getAbsolute_u16(char registry);
	//fetches address space
	uint8_t NES::getIndirectX_u8();
	//fetches address space
	uint8_t NES::getIndirectY_u8();
	uint16_t NES::getDataFromMode_u16(const AddressingMode &mode);


	void NES::push(uint16_t);
	uint16_t NES::pull();

	void NES::SET_ZN(const char &value);
	void NES::SET_NEGATIVE(const char &value);
	void NES::SET_NEGATIVE();
	void NES::UNSET_NEGATIVE();
	void NES::SET_ZERO(const char &value);
	uint8_t NES::P_READ();							//Transform custom P to byte.
	void NES::P_WRITE(const uint8_t &M);		//Transform byte to custom P.

	void NES::InitilizeOpCodeTable();
	uint8_t NES::decodeOPCode(const uint8_t &opcode);

	int_ptr NESOpCodeTable[256];
	char* NESOpCodeTableNames[256];

	//OP
#pragma region Storage
	int LDA(const AddressingMode &mode); //Load A with M
	int LDAI();
	int LDAZP();
	int LDAZPX();
	int LDAA();
	int LDAAX();
	int LDAAY();
	int LDAIX();
	int LDAIY();

	int LDX(const AddressingMode &mode); //Load X with M
	int LDXI();
	int LDXZP();
	int LDXZPY();
	int LDXA();
	int LDXAY();
	
	
	int LDY(const AddressingMode &mode); //Load Y with M
	int LDYI();
	int LDYZP();
	int LDYZPX();
	int LDYA();
	int LDYAX();
	
	
	int STA(const AddressingMode &mode); //Store A in M
	int STAZP();
	int STAZPX();
	int STAA();
	int STAAX();
	int STAAY();
	int STAIX();
	int STAIY();
	
	int STX(const AddressingMode &mode); //Store X in M
	int STXZP();
	int STXZPY();
	int STXA();
	
	int STY(const AddressingMode &mode); //Store Y in M
	int STYZP();
	int STYZPX();
	int STYA();
	
	int TAX(); //Transfer A to X											--Implied
	int TAY(); //Transfer A to Y											--Implied
	int TSX(); //Transfer Stack Pointer to X								--Implied
	int TXA(); //Transfer X to A											--Implied
	int TXS(); //Transfer X to Stack Pointer								--Implied
	int TYA(); //Transfer Y to A											--Implied
#pragma endregion Storage


#pragma region Math
	int ADC(const AddressingMode &mode); //Add M to A with Carry
	int ADCI();
	int ADCZP();
	int ADCZPX();
	int ADCA();
	int ADCAX();
	int ADCAY();
	int ADCIX();
	int ADCIY();

	int DEC(const AddressingMode &mode); //Decrement M by One
	int DECZP();
	int DECZPX();
	int DECA();
	int DECAX();

	int DEX(); //Decrement X by One											--Implied
	int DEY(); //Decrement Y by One											--Implied

	int INC(const AddressingMode &mode); //Increment M by One
	int INCZP();
	int INCZPX();
	int INCA();
	int INCAX();

	int INX(); //Increment X by One											--Implied
	int INY(); //Increment Y by One											--Implied

	int SBC(const AddressingMode &mode); //Subtract M from A with Borrow
	int SBCI();
	int SBCZP();
	int SBCZPX();
	int SBCA();
	int SBCAX();
	int SBCAY();
	int SBCIX();
	int SBCIY();

#pragma endregion Math


#pragma region Bitwise
	int AND(const AddressingMode &mode); //"AND" M with A
	int ANDI();
	int ANDZP();
	int ANDZPX();
	int ANDA();
	int ANDAX();
	int ANDAY();
	int ANDIX();
	int ANDIY();

	int ASL(const AddressingMode &mode); //Shift Left One Bit(M or A)
	int ASLACC();
	int ASLZP();
	int ASLZPX();
	int ASLA();
	int ASLAX();

	int BIT(const AddressingMode &mode); //Test Bits in M with A
	int BITZP();
	int BITA();

	int EOR(const AddressingMode &mode); //"Exclusive-Or" M with A
	int EORI();
	int EORZP();
	int EORZPX();
	int EORA();
	int EORAX();
	int EORAY();
	int EORIX();
	int EORIY();

	int LSR(const AddressingMode &mode); //Shift Right One Bit(M or A)
	int LSRACC();
	int LSRZP();
	int LSRZPX();
	int LSRA();
	int LSRAX();

	int ORA(const AddressingMode &mode); //"OR" M with A
	int ORAI();
	int ORAZP();
	int ORAZPX();
	int ORAA();
	int ORAAX();
	int ORAAY();
	int ORAIX();
	int ORAIY();

	int ROL(const AddressingMode &mode); //Rotate One Bit Left(M or A)
	int ROLACC();
	int ROLZP();
	int ROLZPX();
	int ROLA();
	int ROLAX();

	int ROR(const AddressingMode &mode); //Rotate One Bit Right(M or A)
	int RORACC();
	int RORZP();
	int RORZPX();
	int RORA();
	int RORAX();

#pragma endregion Bitwise


#pragma region Branch
	int BCC(); //Branch on Carry Clear										--RELATIVE
	int BCS(); //Branch on Carry Set										--RELATIVe
	int BEQ(); //Branch on Result Zero										--RELATIVE
	int BMI(); //Branch on Result Minus										--RELATIVE
	int BNE(); //Branch on Result not Zero									--RELATIVE
	int BPL(); //Branch on Result Plus										--RELATIVE
	int BVC(); //Branch on Overflow Clear									--RELATIVE
	int BVS(); //Branch on Overflow Set										--RELATIVE
#pragma endregion Branch


#pragma region Jump
	int JMP(const AddressingMode &mode); //Jump to Location
	int JMPA();
	int JMPI();

	int JSR(); //Jump to Location Save Return Address						--ABSOLUTE
	int RTI(); //Return from Interrupt										--Implied
	int RTS(); //Return from Subroutine										--Implied
#pragma endregion Jump


#pragma region Registers
	int CLC(); //Clear Carry Flag											--Implied
	int CLD(); //Clear Decimal Mode											--Implied
	int CLI(); //Clear interrupt Disable Bit								--Implied
	int CLV(); //Clear Overflow Flag										--Implied

	int NES::CMP_ALL(const AddressingMode &mode, char reg); //COMPARE ALL -- Unofficial

	int CMP(const AddressingMode &mode); //Compare M AND A
	int CMPI();
	int CMPZP();
	int CMPZPX();
	int CMPA();
	int CMPAX();
	int CMPAY();
	int CMPIX();
	int CMPIY();

	int CPX(const AddressingMode &mode); //Compare M and X
	int CPXI();
	int CPXZP();
	int CPXA();

	int CPY(const AddressingMode &mode); //Compare M and Y
	int CPYI();
	int CPYZP();
	int CPYA();

	int SEC(); //Set Carry Flag												--Implied
	int SED(); //Set Decimal Mode											--Implied
	int SEI(); //Set Interrupt Disable Status								--Implied
	int SEV(); //Set overflow --UNOFFICIAL
#pragma endregion Registers


#pragma region Stack
	int PHA(); //Push A on Stack											--Implied
	int PHP(); //Push Processor Status on Stack								--Implied
	int PLA(); //Pull A from Stack											--Implied
	int PLP(); //Pull Processor Status from Stack							--Implied
#pragma endregion Stack


#pragma region Interrupts
	int NOP();		//No Operation											--Implied
	int NMI();		//Non-Maskable Intteruption
	int Reset();	//Reset
	int IRQ();		//Interrupt Request
	int BRK();		//Break													--Implied
#pragma endregion


#pragma region Unofficial/Illegal
	//Do not integrate (now)
	int KILL();
	int SLO();
	int ASL();
	int ANC();
	int RLA();
	int RRA();
	int ALR();
	int ARR();
	int XAA();
	int LAX();
	int AXS();
	int DCP();
	int SAX();
	int SRE();
	int ISC();
	int SHX();
	int SHY();
#pragma endregion Unofficial/Illegal

};

#endif // !NES_H_