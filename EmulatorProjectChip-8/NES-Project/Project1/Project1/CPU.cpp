#include "CPU.h"


void CPU::ConfigureSystemBehaviour()
{
	unsigned char NESValitdator[] = {
		'N', 'E', 'S', 0x1A
	};

	printf("NESValitdator::");

	for (int i = 0; i <= 0x03; i++)
	{
		std::string data_str;
		char val[8];
		snprintf(val, sizeof(val), "0x%d-", cartridge[i]);
		data_str.append(val);
		const char* data = data_str.c_str();
		printf(data);

		if (NESValitdator[i] != cartridge[i])
		{
			operable = FALSE;
			return;
		}
	}

	uint8_t numberOf16KRomBanks = cartridge[4]; //Size of PRG ROM in 16 KB units
	uint8_t numberOf8KVRomBanks = cartridge[5]; //Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
	uint8_t numberOf8KRamBanks = cartridge[8];	//Size of PRG RAM in 8 KB units (Value 0 infers 8 KB for compatibility; see PRG RAM circuit)
	if (numberOf8KRamBanks == 0)
	{
		numberOf8KRamBanks = 1;
	}

	char flag6 = cartridge[6];
	char flag7 = cartridge[7];
	char flag9 = cartridge[9];

	int Mirroring = ((flag6 >> 0) & 1);//1 for vertical mirroring, 0 for horizontal mirroring.
	int BatteryBackedPRGRAM = ((flag6 >> 1) & 1);//1 for battery - backed RAM at $6000 - $7FFF.
	int Trainer = ((flag6 >> 2) & 1);//1 for a 512 - byte trainer at $7000 - $71FF.
	int FourScreenVRAM = ((flag6 >> 3) & 1);//1 for a four - screen VRAM layout. (Ignore mirroring control or above mirroring bit; instead provide four-screen VRAM)
	int NESv2Format = 0;
	int VS = ((flag7 >> 0) & 1);
	int Region1 = 0;
	uint8_t mapper = 0x00;
	for (int n = 4; n < 7; n++)
	{

		//Four lower bits of ROM Mapper Type.
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		mapper |= ((flag6 >> n) & 1) << n - 4;
#else	
		mapper |= ((flag6 >> n) & 1) << n;
#endif
	}

	for (int n = 1; n < 7; n++)
	{
		if (n >= 1 && n <= 3) //RESERVED, MUST BE 0
		{
			if (n == 2) //NEW ADDITION
			{
				NESv2Format = ((flag7 >> n) & 1) + ((flag7 >> n + 1) & 1);
			}
		}
		else if (n >= 4 && n <= 7) //Four higher bits of ROM Mapper Type.
		{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			mapper |= ((flag7 >> n) & 1) << n;
#else	
			mapper |= ((flag7 >> n) & 1) << n-4;
#endif
		}
	}

	for (int n = 0; n < 7; n++)
	{
		if (n == 0) //REGION
		{
			Region1 = (flag9 >> n) & 1;
			switch (Region1)
			{
			case 1:
				//PAL
				printf("PAL REGION-");
				break;
			default:
				//NTSC
				printf("NTSC REGION-");
				break;
			}
		}
		else //RESERVED, MUST BE 0
		{
			if (((flag9 >> n) & 1) != 0)
			{
				operable = FALSE;
				return;
			}
		}
	}

	for (int i = 10; i < 15; i++) //RESERVED, MUST BE 0
	{
		if (cartridge[i] != 0 && NESv2Format == 0)
		{
			operable = FALSE;
			return;
		}
	}

	uint16_t prgLocation = 0x8000;
	int prgSize = numberOf16KRomBanks * 16 * 1024;
	uint16_t chrLocation = 0x0000;
	int chrSize = numberOf8KVRomBanks * 8 * 1024;
	if (numberOf8KVRomBanks = 0) chrSize = 1 * 8 * 1024; //supposingly it's only 8k in size

	cpuRegistry.systemBehaviour.BatteryBackedPRGRAM = BatteryBackedPRGRAM;
	cpuRegistry.systemBehaviour.chrLocation = chrLocation;
	cpuRegistry.systemBehaviour.chrSize = chrSize;
	cpuRegistry.systemBehaviour.FourScreenVRAM = FourScreenVRAM;
	cpuRegistry.systemBehaviour.Mirroring = (Mirroring == 0) ? Horizontal : Vertical;
	cpuRegistry.systemBehaviour.NESv2Format = NESv2Format;
	cpuRegistry.systemBehaviour.numberOf16KRomBanks = numberOf16KRomBanks;
	cpuRegistry.systemBehaviour.numberOf8KRamBanks = numberOf8KRamBanks;
	cpuRegistry.systemBehaviour.numberOf8KVRomBanks = numberOf8KVRomBanks;
	cpuRegistry.systemBehaviour.prgLocation = prgLocation;
	cpuRegistry.systemBehaviour.prgSize = prgSize;
	cpuRegistry.systemBehaviour.Region1 = Region1;
	cpuRegistry.systemBehaviour.Trainer = Trainer;
	cpuRegistry.systemBehaviour.VS = VS;
	cpuRegistry.systemBehaviour.mapper = mapper;
}

void CPU::ConfigureClockSpeed()
{
	//Todo: set clockspeed(s)
}

void CPU::Boot()
{
	ConfigureSystemBehaviour();

	if (operable == TRUE)
	{
		ConfigureClockSpeed();
		InitialiseOpCodes();
	}
}

void CPU::RESET()
{
	operable = TRUE;
	cpuRegistry.sp = 0x01FF; //Default start position of stackpointer
	cpuRegistry.pc = 0x8000; //
	cpuRegistry.registry.A = NULL;
	cpuRegistry.registry.X = NULL;
	cpuRegistry.registry.Y = NULL;
	opcode = NULL;

	Boot();
}

void CPU::__MemADD(const uint16_t & address, const uint8_t value)
{
	memory[address] += value;
}

void CPU::__MemSUB(const uint16_t & address, const uint8_t value)
{
	memory[address] -= value;
}

void CPU::__MemWrite(const uint16_t & address, const uint8_t value)
{
	memory[address] = value;
}

const uint8_t CPU::__MemRead(const uint16_t & address)
{
	return memory[address];
}

const uint16_t CPU::__MemRead16(const uint16_t & address)
{
	uint8_t slot0 = memory[address];
	uint8_t slot1 = memory[address + 1];
	uint16_t data = 0x0000;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	data |= slot0;
	data |= (slot1 << 8);
#else	
	data |= slot1;
	data |= (slot0 << 8);
#endif

	return memory[address];
}

uint8_t CPU::fetch()
{
	uint8_t data = __MemRead(this->cpuRegistry.pc);
	this->cpuRegistry.pc += 1;

	return data;
}

uint16_t CPU::fetch16()
{
	uint16_t data = __MemRead16(this->cpuRegistry.pc);
	this->cpuRegistry.pc += 2;

	return data;
}

void CPU::EmulateCycle()
{
	//TODO: emulate;
	uint16_t temp_pc = cpuRegistry.pc;
	opcode = fetch(); 

	uint8_t cycles = opCodeCycleTable[opcode];
	/*int additionalCycles = */opCodeTable[opcode];
	printf("%s", opCodeNameTable[opcode]);

	cycle += cycles;
}

CPU::CPU(CartridgeLDR* cartridgeSlot)
{
	cartridgeSlot->LoadIntoCartridgeSlot(cartridge);
	RESET();
}

//TODO: Opcode tables


void CPU::InitialiseOpCodes()
{
	//everycommand is either no operation or returns combining opcode.
	for (int i = 0x00; i < 0xFF; i++)
	{
		opCodeTable[i] = &CPU::NOP;
		opCodeNameTable[i] = "NOP";
		opCodeCycleTable[i] = 2;
	}

#pragma region OPCODE TABLE
	//opCodeTable[0x00] = &CPU::NOP;
	opCodeTable[0x00] = &CPU::BRK;
	opCodeTable[0x01] = &CPU::ORAIX;
	opCodeTable[0x05] = &CPU::ORAZP;
	opCodeTable[0x06] = &CPU::ASLZP;
	opCodeTable[0x08] = &CPU::PHP;
	opCodeTable[0x09] = &CPU::ORAI;
	opCodeTable[0x0A] = &CPU::ASLACC;
	opCodeTable[0x0D] = &CPU::ORAA;
	opCodeTable[0x0E] = &CPU::ASLA;
	opCodeTable[0x10] = &CPU::BPL;
	opCodeTable[0x11] = &CPU::ORAIY;
	opCodeTable[0x15] = &CPU::ORAZPX;
	opCodeTable[0x16] = &CPU::ASLZPX;
	opCodeTable[0x18] = &CPU::CLC;
	opCodeTable[0x19] = &CPU::ORAAY;
	opCodeTable[0x1D] = &CPU::ORAAX;
	opCodeTable[0x1E] = &CPU::ASLAX;
	opCodeTable[0x20] = &CPU::JSR;
	opCodeTable[0x21] = &CPU::ANDIX;
	opCodeTable[0x24] = &CPU::BITZP;
	opCodeTable[0x25] = &CPU::ANDZP;
	opCodeTable[0x26] = &CPU::ROLZP;
	opCodeTable[0x28] = &CPU::PLP;
	opCodeTable[0x29] = &CPU::ANDI;
	opCodeTable[0x2A] = &CPU::ROLACC;
	opCodeTable[0x2C] = &CPU::BITA;
	opCodeTable[0x2D] = &CPU::ANDA;
	opCodeTable[0x2E] = &CPU::ROLA;
	opCodeTable[0x30] = &CPU::BMI;
	opCodeTable[0x31] = &CPU::ANDIY;
	opCodeTable[0x35] = &CPU::ANDZPX;
	opCodeTable[0x36] = &CPU::ROLZPX;
	opCodeTable[0x38] = &CPU::SEC;
	opCodeTable[0x39] = &CPU::ANDAY;
	opCodeTable[0x3D] = &CPU::ANDAX;
	opCodeTable[0x3E] = &CPU::ROLAX;
	opCodeTable[0x40] = &CPU::RTI;
	opCodeTable[0x41] = &CPU::EORIX;
	opCodeTable[0x45] = &CPU::EORZP;
	opCodeTable[0x46] = &CPU::LSRZP;
	opCodeTable[0x48] = &CPU::PHA;
	opCodeTable[0x49] = &CPU::EORI;
	opCodeTable[0x4A] = &CPU::LSRACC;
	opCodeTable[0x4C] = &CPU::JMPA;
	opCodeTable[0x4D] = &CPU::EORA;
	opCodeTable[0x4E] = &CPU::LSRA;
	opCodeTable[0x50] = &CPU::BVC;
	opCodeTable[0x51] = &CPU::EORIY;
	opCodeTable[0x55] = &CPU::EORZPX;
	opCodeTable[0x56] = &CPU::LSRZPX;
	opCodeTable[0x58] = &CPU::CLI;
	opCodeTable[0x59] = &CPU::EORAY;
	opCodeTable[0x5D] = &CPU::EORAX;
	opCodeTable[0x5E] = &CPU::LSRAX;
	opCodeTable[0x60] = &CPU::RTS;
	opCodeTable[0x61] = &CPU::ADCIX;
	opCodeTable[0x65] = &CPU::ADCZP;
	opCodeTable[0x66] = &CPU::RORZP;
	opCodeTable[0x68] = &CPU::PLA;
	opCodeTable[0x69] = &CPU::ADCI;
	opCodeTable[0x6A] = &CPU::RORACC;
	opCodeTable[0x6C] = &CPU::JMPI;
	opCodeTable[0x6D] = &CPU::ADCA;
	opCodeTable[0x6E] = &CPU::RORA;
	opCodeTable[0x70] = &CPU::BVS;
	opCodeTable[0x71] = &CPU::ADCIY;
	opCodeTable[0x75] = &CPU::ADCZPX;
	opCodeTable[0x76] = &CPU::RORZPX;
	opCodeTable[0x78] = &CPU::SEI;
	opCodeTable[0x79] = &CPU::ADCAY;
	opCodeTable[0x7D] = &CPU::ADCAX;
	opCodeTable[0x7E] = &CPU::RORAX;
	opCodeTable[0x81] = &CPU::STAIX;
	opCodeTable[0x84] = &CPU::STYZP;
	opCodeTable[0x85] = &CPU::STAZP;
	opCodeTable[0x86] = &CPU::STXZP;
	opCodeTable[0x88] = &CPU::DEY;
	opCodeTable[0x8A] = &CPU::TXA;
	opCodeTable[0x8C] = &CPU::STYA;
	opCodeTable[0x8D] = &CPU::STAA;
	opCodeTable[0x8E] = &CPU::STXA;
	opCodeTable[0x90] = &CPU::BCC;
	opCodeTable[0x91] = &CPU::STAIY;
	opCodeTable[0x94] = &CPU::STYZPX;
	opCodeTable[0x95] = &CPU::STAZPX;
	opCodeTable[0x96] = &CPU::STXZPY;
	opCodeTable[0x98] = &CPU::TYA;
	opCodeTable[0x99] = &CPU::STAAY;
	opCodeTable[0x9A] = &CPU::TXS;
	opCodeTable[0x9D] = &CPU::STAAX;
	opCodeTable[0xA0] = &CPU::LDYI;
	opCodeTable[0xA1] = &CPU::LDAIX;
	opCodeTable[0xA2] = &CPU::LDXI;
	opCodeTable[0xA4] = &CPU::LDYZP;
	opCodeTable[0xA5] = &CPU::LDAZP;
	opCodeTable[0xA6] = &CPU::LDXZP;
	opCodeTable[0xA8] = &CPU::TAY;
	opCodeTable[0xA9] = &CPU::LDAI;
	opCodeTable[0xAA] = &CPU::TAX;
	opCodeTable[0xAC] = &CPU::LDYA;
	opCodeTable[0xAD] = &CPU::LDAA;
	opCodeTable[0xAE] = &CPU::LDXA;
	opCodeTable[0xB0] = &CPU::BCS;
	opCodeTable[0xB1] = &CPU::LDAIY;
	opCodeTable[0xB4] = &CPU::LDYZPX;
	opCodeTable[0xB5] = &CPU::LDAZPX;
	opCodeTable[0xB6] = &CPU::LDXZPY;
	opCodeTable[0xB8] = &CPU::CLV;
	opCodeTable[0xB9] = &CPU::LDAAY;
	opCodeTable[0xBA] = &CPU::TSX;
	opCodeTable[0xBC] = &CPU::LDYAX;
	opCodeTable[0xBD] = &CPU::LDAAX;
	opCodeTable[0xBE] = &CPU::LDXAY;
	opCodeTable[0xC0] = &CPU::CPYI;
	opCodeTable[0xC1] = &CPU::CMPIX;
	opCodeTable[0xC4] = &CPU::CPYZP;
	opCodeTable[0xC5] = &CPU::CMPZP;
	opCodeTable[0xC6] = &CPU::DECZP;
	opCodeTable[0xC8] = &CPU::INY;
	opCodeTable[0xC9] = &CPU::CMPI;
	opCodeTable[0xCA] = &CPU::DEX;
	opCodeTable[0xCC] = &CPU::CPYA;
	opCodeTable[0xCD] = &CPU::CMPA;
	opCodeTable[0xCE] = &CPU::DECA;
	opCodeTable[0xD0] = &CPU::BNE;
	opCodeTable[0xD1] = &CPU::CMPIY;
	opCodeTable[0xD5] = &CPU::CMPZPX;
	opCodeTable[0xD6] = &CPU::DECZPX;
	opCodeTable[0xD8] = &CPU::CLD;
	opCodeTable[0xD9] = &CPU::CMPAY;
	opCodeTable[0xDD] = &CPU::CMPAX;
	opCodeTable[0xDE] = &CPU::DECAX;
	opCodeTable[0xE0] = &CPU::CPXI;
	opCodeTable[0xE1] = &CPU::SBCIX;
	opCodeTable[0xE4] = &CPU::CPXZP;
	opCodeTable[0xE5] = &CPU::SBCZP;
	opCodeTable[0xE6] = &CPU::INCZP;
	opCodeTable[0xE8] = &CPU::INX;
	opCodeTable[0xE9] = &CPU::SBCI;
	opCodeTable[0xEA] = &CPU::NOP;
	opCodeTable[0xEC] = &CPU::CPXA;
	opCodeTable[0xED] = &CPU::SBCA;
	opCodeTable[0xEE] = &CPU::INCA;
	opCodeTable[0xF0] = &CPU::BEQ;
	opCodeTable[0xF1] = &CPU::SBCIY;
	opCodeTable[0xF5] = &CPU::SBCZPX;
	opCodeTable[0xF6] = &CPU::INCZPX;
	opCodeTable[0xF8] = &CPU::SED;
	opCodeTable[0xF9] = &CPU::SBCAY;
	opCodeTable[0xFD] = &CPU::SBCAX;
	opCodeTable[0xFE] = &CPU::INCAX;
#pragma endregion OPCODE TABLE
#pragma region OPCODE TABLENAMES
	opCodeNameTable[0x00] = "BRK";
	opCodeNameTable[0x01] = "ORAIX";
	opCodeNameTable[0x05] = "ORAZP";
	opCodeNameTable[0x06] = "ASLZP";
	opCodeNameTable[0x08] = "PHP";
	opCodeNameTable[0x09] = "ORAI";
	opCodeNameTable[0x0A] = "ASLACC";
	opCodeNameTable[0x0D] = "ORAA";
	opCodeNameTable[0x0E] = "ASLA";
	opCodeNameTable[0x10] = "BPL";
	opCodeNameTable[0x11] = "ORAIY";
	opCodeNameTable[0x15] = "ORAZPX";
	opCodeNameTable[0x16] = "ASLZPX";
	opCodeNameTable[0x18] = "CLC";
	opCodeNameTable[0x19] = "ORAAY";
	opCodeNameTable[0x1D] = "ORAAX";
	opCodeNameTable[0x1E] = "ASLAX";
	opCodeNameTable[0x20] = "JSR";
	opCodeNameTable[0x21] = "ANDIX";
	opCodeNameTable[0x24] = "BITZP";
	opCodeNameTable[0x25] = "ANDZP";
	opCodeNameTable[0x26] = "ROLZP";
	opCodeNameTable[0x28] = "PLP";
	opCodeNameTable[0x29] = "ANDI";
	opCodeNameTable[0x2A] = "ROLACC";
	opCodeNameTable[0x2C] = "BITA";
	opCodeNameTable[0x2D] = "ANDA";
	opCodeNameTable[0x2E] = "ROLA";
	opCodeNameTable[0x30] = "BMI";
	opCodeNameTable[0x31] = "ANDIY";
	opCodeNameTable[0x35] = "ANDZPX";
	opCodeNameTable[0x36] = "ROLZPX";
	opCodeNameTable[0x38] = "SEC";
	opCodeNameTable[0x39] = "ANDAY";
	opCodeNameTable[0x3D] = "ANDAX";
	opCodeNameTable[0x3E] = "ROLAX";
	opCodeNameTable[0x40] = "RTI";
	opCodeNameTable[0x41] = "EORIX";
	opCodeNameTable[0x45] = "EORZP";
	opCodeNameTable[0x46] = "LSRZP";
	opCodeNameTable[0x48] = "PHA";
	opCodeNameTable[0x49] = "EORI";
	opCodeNameTable[0x4A] = "LSRACC";
	opCodeNameTable[0x4C] = "JMPA";
	opCodeNameTable[0x4D] = "EORA";
	opCodeNameTable[0x4E] = "LSRA";
	opCodeNameTable[0x50] = "BVC";
	opCodeNameTable[0x51] = "EORIY";
	opCodeNameTable[0x55] = "EORZPX";
	opCodeNameTable[0x56] = "LSRZPX";
	opCodeNameTable[0x58] = "CLI";
	opCodeNameTable[0x59] = "EORAY";
	opCodeNameTable[0x5D] = "EORAX";
	opCodeNameTable[0x5E] = "LSRAX";
	opCodeNameTable[0x60] = "RTS";
	opCodeNameTable[0x61] = "ADCIX";
	opCodeNameTable[0x65] = "ADCZP";
	opCodeNameTable[0x66] = "RORZP";
	opCodeNameTable[0x68] = "PLA";
	opCodeNameTable[0x69] = "ADCI";
	opCodeNameTable[0x6A] = "RORACC";
	opCodeNameTable[0x6C] = "JMPI";
	opCodeNameTable[0x6D] = "ADCA";
	opCodeNameTable[0x6E] = "RORA";
	opCodeNameTable[0x70] = "BVS";
	opCodeNameTable[0x71] = "ADCIY";
	opCodeNameTable[0x75] = "ADCZPX";
	opCodeNameTable[0x76] = "RORZPX";
	opCodeNameTable[0x78] = "SEI";
	opCodeNameTable[0x79] = "ADCAY";
	opCodeNameTable[0x7D] = "ADCAX";
	opCodeNameTable[0x7E] = "RORAX";
	opCodeNameTable[0x81] = "STAIX";
	opCodeNameTable[0x84] = "STYZP";
	opCodeNameTable[0x85] = "STAZP";
	opCodeNameTable[0x86] = "STXZP";
	opCodeNameTable[0x88] = "DEY";
	opCodeNameTable[0x8A] = "TXA";
	opCodeNameTable[0x8C] = "STYA";
	opCodeNameTable[0x8D] = "STAA";
	opCodeNameTable[0x8E] = "STXA";
	opCodeNameTable[0x90] = "BCC";
	opCodeNameTable[0x91] = "STAIY";
	opCodeNameTable[0x94] = "STYZPX";
	opCodeNameTable[0x95] = "STAZPX";
	opCodeNameTable[0x96] = "STXZPY";
	opCodeNameTable[0x98] = "TYA";
	opCodeNameTable[0x99] = "STAAY";
	opCodeNameTable[0x9A] = "TXS";
	opCodeNameTable[0x9D] = "STAAX";
	opCodeNameTable[0xA0] = "LDYI";
	opCodeNameTable[0xA1] = "LDAIX";
	opCodeNameTable[0xA2] = "LDXI";
	opCodeNameTable[0xA4] = "LDYZP";
	opCodeNameTable[0xA5] = "LDAZP";
	opCodeNameTable[0xA6] = "LDXZP";
	opCodeNameTable[0xA8] = "TAY";
	opCodeNameTable[0xA9] = "LDAI";
	opCodeNameTable[0xAA] = "TAX";
	opCodeNameTable[0xAC] = "LDYA";
	opCodeNameTable[0xAD] = "LDAA";
	opCodeNameTable[0xAE] = "LDXA";
	opCodeNameTable[0xB0] = "BCS";
	opCodeNameTable[0xB1] = "LDAIY";
	opCodeNameTable[0xB4] = "LDYZPX";
	opCodeNameTable[0xB5] = "LDAZPX";
	opCodeNameTable[0xB6] = "LDXZPY";
	opCodeNameTable[0xB8] = "CLV";
	opCodeNameTable[0xB9] = "LDAAY";
	opCodeNameTable[0xBA] = "TSX";
	opCodeNameTable[0xBC] = "LDYAX";
	opCodeNameTable[0xBD] = "LDAAX";
	opCodeNameTable[0xBE] = "LDXAY";
	opCodeNameTable[0xC0] = "CPYI";
	opCodeNameTable[0xC1] = "CMPIX";
	opCodeNameTable[0xC4] = "CPYZP";
	opCodeNameTable[0xC5] = "CMPZP";
	opCodeNameTable[0xC6] = "DECZP";
	opCodeNameTable[0xC8] = "INY";
	opCodeNameTable[0xC9] = "CMPI";
	opCodeNameTable[0xCA] = "DEX";
	opCodeNameTable[0xCC] = "CPYA";
	opCodeNameTable[0xCD] = "CMPA";
	opCodeNameTable[0xCE] = "DECA";
	opCodeNameTable[0xD0] = "BNE";
	opCodeNameTable[0xD1] = "CMPIY";
	opCodeNameTable[0xD5] = "CMPZPX";
	opCodeNameTable[0xD6] = "DECZPX";
	opCodeNameTable[0xD8] = "CLD";
	opCodeNameTable[0xD9] = "CMPAY";
	opCodeNameTable[0xDD] = "CMPAX";
	opCodeNameTable[0xDE] = "DECAX";
	opCodeNameTable[0xE0] = "CPXI";
	opCodeNameTable[0xE1] = "SBCIX";
	opCodeNameTable[0xE4] = "CPXZP";
	opCodeNameTable[0xE5] = "SBCZP";
	opCodeNameTable[0xE6] = "INCZP";
	opCodeNameTable[0xE8] = "INX";
	opCodeNameTable[0xE9] = "SBCI";
	opCodeNameTable[0xEA] = "NOP";
	opCodeNameTable[0xEC] = "CPXA";
	opCodeNameTable[0xED] = "SBCA";
	opCodeNameTable[0xEE] = "INCA";
	opCodeNameTable[0xF0] = "BEQ";
	opCodeNameTable[0xF1] = "SBCIY";
	opCodeNameTable[0xF5] = "SBCZPX";
	opCodeNameTable[0xF6] = "INCZPX";
	opCodeNameTable[0xF8] = "SED";
	opCodeNameTable[0xF9] = "SBCAY";
	opCodeNameTable[0xFD] = "SBCAX";
	opCodeNameTable[0xFE] = "INCAX";
#pragma endregion OPCODE TABLENAMES
#pragma region OPCODE TABLECYCLES
	opCodeCycleTable[0x00] = 2;
	opCodeCycleTable[0x01] = 2;
	opCodeCycleTable[0x05] = 2;
	opCodeCycleTable[0x06] = 2;
	opCodeCycleTable[0x08] = 2;
	opCodeCycleTable[0x09] = 2;
	opCodeCycleTable[0x0A] = 2;
	opCodeCycleTable[0x0D] = 2;
	opCodeCycleTable[0x0E] = 2;
	opCodeCycleTable[0x10] = 2;
	opCodeCycleTable[0x11] = 2;
	opCodeCycleTable[0x15] = 2;
	opCodeCycleTable[0x16] = 2;
	opCodeCycleTable[0x18] = 2;
	opCodeCycleTable[0x19] = 2;
	opCodeCycleTable[0x1D] = 2;
	opCodeCycleTable[0x1E] = 2;
	opCodeCycleTable[0x20] = 2;
	opCodeCycleTable[0x21] = 2;
	opCodeCycleTable[0x24] = 2;
	opCodeCycleTable[0x25] = 2;
	opCodeCycleTable[0x26] = 2;
	opCodeCycleTable[0x28] = 2;
	opCodeCycleTable[0x29] = 2;
	opCodeCycleTable[0x2A] = 2;
	opCodeCycleTable[0x2C] = 2;
	opCodeCycleTable[0x2D] = 2;
	opCodeCycleTable[0x2E] = 2;
	opCodeCycleTable[0x30] = 2;
	opCodeCycleTable[0x31] = 2;
	opCodeCycleTable[0x35] = 2;
	opCodeCycleTable[0x36] = 2;
	opCodeCycleTable[0x38] = 2;
	opCodeCycleTable[0x39] = 2;
	opCodeCycleTable[0x3D] = 2;
	opCodeCycleTable[0x3E] = 2;
	opCodeCycleTable[0x40] = 2;
	opCodeCycleTable[0x41] = 2;
	opCodeCycleTable[0x45] = 2;
	opCodeCycleTable[0x46] = 2;
	opCodeCycleTable[0x48] = 2;
	opCodeCycleTable[0x49] = 2;
	opCodeCycleTable[0x4A] = 2;
	opCodeCycleTable[0x4C] = 2;
	opCodeCycleTable[0x4D] = 2;
	opCodeCycleTable[0x4E] = 2;
	opCodeCycleTable[0x50] = 2;
	opCodeCycleTable[0x51] = 2;
	opCodeCycleTable[0x55] = 2;
	opCodeCycleTable[0x56] = 2;
	opCodeCycleTable[0x58] = 2;
	opCodeCycleTable[0x59] = 2;
	opCodeCycleTable[0x5D] = 2;
	opCodeCycleTable[0x5E] = 2;
	opCodeCycleTable[0x60] = 2;
	opCodeCycleTable[0x61] = 2;
	opCodeCycleTable[0x65] = 2;
	opCodeCycleTable[0x66] = 2;
	opCodeCycleTable[0x68] = 2;
	opCodeCycleTable[0x69] = 2;
	opCodeCycleTable[0x6A] = 2;
	opCodeCycleTable[0x6C] = 2;
	opCodeCycleTable[0x6D] = 2;
	opCodeCycleTable[0x6E] = 2;
	opCodeCycleTable[0x70] = 2;
	opCodeCycleTable[0x71] = 2;
	opCodeCycleTable[0x75] = 2;
	opCodeCycleTable[0x76] = 2;
	opCodeCycleTable[0x78] = 2;
	opCodeCycleTable[0x79] = 2;
	opCodeCycleTable[0x7D] = 2;
	opCodeCycleTable[0x7E] = 2;
	opCodeCycleTable[0x81] = 2;
	opCodeCycleTable[0x84] = 2;
	opCodeCycleTable[0x85] = 2;
	opCodeCycleTable[0x86] = 2;
	opCodeCycleTable[0x88] = 2;
	opCodeCycleTable[0x8A] = 2;
	opCodeCycleTable[0x8C] = 2;
	opCodeCycleTable[0x8D] = 2;
	opCodeCycleTable[0x8E] = 2;
	opCodeCycleTable[0x90] = 2;
	opCodeCycleTable[0x91] = 2;
	opCodeCycleTable[0x94] = 2;
	opCodeCycleTable[0x95] = 2;
	opCodeCycleTable[0x96] = 2;
	opCodeCycleTable[0x98] = 2;
	opCodeCycleTable[0x99] = 2;
	opCodeCycleTable[0x9A] = 2;
	opCodeCycleTable[0x9D] = 2;
	opCodeCycleTable[0xA0] = 2;
	opCodeCycleTable[0xA1] = 2;
	opCodeCycleTable[0xA2] = 2;
	opCodeCycleTable[0xA4] = 2;
	opCodeCycleTable[0xA5] = 2;
	opCodeCycleTable[0xA6] = 2;
	opCodeCycleTable[0xA8] = 2;
	opCodeCycleTable[0xA9] = 2;
	opCodeCycleTable[0xAA] = 2;
	opCodeCycleTable[0xAC] = 2;
	opCodeCycleTable[0xAD] = 2;
	opCodeCycleTable[0xAE] = 2;
	opCodeCycleTable[0xB0] = 2;
	opCodeCycleTable[0xB1] = 2;
	opCodeCycleTable[0xB4] = 2;
	opCodeCycleTable[0xB5] = 2;
	opCodeCycleTable[0xB6] = 2;
	opCodeCycleTable[0xB8] = 2;
	opCodeCycleTable[0xB9] = 2;
	opCodeCycleTable[0xBA] = 2;
	opCodeCycleTable[0xBC] = 2;
	opCodeCycleTable[0xBD] = 2;
	opCodeCycleTable[0xBE] = 2;
	opCodeCycleTable[0xC0] = 2;
	opCodeCycleTable[0xC1] = 2;
	opCodeCycleTable[0xC4] = 2;
	opCodeCycleTable[0xC5] = 2;
	opCodeCycleTable[0xC6] = 2;
	opCodeCycleTable[0xC8] = 2;
	opCodeCycleTable[0xC9] = 2;
	opCodeCycleTable[0xCA] = 2;
	opCodeCycleTable[0xCC] = 2;
	opCodeCycleTable[0xCD] = 2;
	opCodeCycleTable[0xCE] = 2;
	opCodeCycleTable[0xD0] = 2;
	opCodeCycleTable[0xD1] = 2;
	opCodeCycleTable[0xD5] = 2;
	opCodeCycleTable[0xD6] = 2;
	opCodeCycleTable[0xD8] = 2;
	opCodeCycleTable[0xD9] = 2;
	opCodeCycleTable[0xDD] = 2;
	opCodeCycleTable[0xDE] = 2;
	opCodeCycleTable[0xE0] = 2;
	opCodeCycleTable[0xE1] = 2;
	opCodeCycleTable[0xE4] = 2;
	opCodeCycleTable[0xE5] = 2;
	opCodeCycleTable[0xE6] = 2;
	opCodeCycleTable[0xE8] = 2;
	opCodeCycleTable[0xE9] = 2;
	opCodeCycleTable[0xEA] = 2;
	opCodeCycleTable[0xEC] = 2;
	opCodeCycleTable[0xED] = 2;
	opCodeCycleTable[0xEE] = 2;
	opCodeCycleTable[0xF0] = 2;
	opCodeCycleTable[0xF1] = 2;
	opCodeCycleTable[0xF5] = 2;
	opCodeCycleTable[0xF6] = 2;
	opCodeCycleTable[0xF8] = 2;
	opCodeCycleTable[0xF9] = 2;
	opCodeCycleTable[0xFD] = 2;
	opCodeCycleTable[0xFE] = 2;
#pragma endregion OPCODE TABLECYCLES
}


#pragma region Storage

	#define ld(registry, memory, location) registry = memory[location];
	//Load A with M
	int CPU::LDAI() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::LDAZP() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::LDAZPX() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::LDAA() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::LDAAX() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::LDAAY() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::LDAIX() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::LDAIY() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}

	//Load X with M
	int CPU::LDXI() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.X, memory, memloc);

		return 0;
	}
	int CPU::LDXZP() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.X, memory, memloc);

		return 0;
	}
	int CPU::LDXZPY() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.X, memory, memloc);

		return 0;
	}
	int CPU::LDXA() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.X, memory, memloc);

		return 0;
	}
	int CPU::LDXAY() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.X, memory, memloc);

		return 0;
	}

	//Load Y with M
	int CPU::LDYI() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.Y, memory, memloc);

		return 0;
	}
	int CPU::LDYZP() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.Y, memory, memloc);

		return 0;
	}
	int CPU::LDYZPX() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.Y, memory, memloc);

		return 0;
	}
	int CPU::LDYA() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.Y, memory, memloc);

		return 0;
	}
	int CPU::LDYAX() {

		uint16_t memloc = 0x0000;

		ld(cpuRegistry.registry.Y, memory, memloc);

		return 0;
	}

	#define st(registry, memory, location) memory[location] = registry;
	//Store A in M
	int CPU::STAZP() { 

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.A, memory, memloc);

		return 0; 
	}
	int CPU::STAZPX() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::STAA() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::STAAX() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::STAAY() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::STAIX() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}
	int CPU::STAIY() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.A, memory, memloc);

		return 0;
	}

	//Store X in M
	int CPU::STXZP() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.X, memory, memloc);

		return 0;
	}
	int CPU::STXZPY() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.X, memory, memloc);

		return 0;
	}
	int CPU::STXA() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.X, memory, memloc);

		return 0;
	}

	//Store Y in M
	int CPU::STYZP() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.Y, memory, memloc);

		return 0;
	}
	int CPU::STYZPX() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.Y, memory, memloc);

		return 0;
	}
	int CPU::STYA() {

		uint16_t memloc = 0x0000;

		st(cpuRegistry.registry.Y, memory, memloc);

		return 0;
	}

	#define ta(registryFrom, registryTo) registryTo = registryFrom;
	//Transfer A to X											--Implied
	int CPU::TAX() {

		ta(cpuRegistry.registry.A, cpuRegistry.registry.X);

		return 0;
	}

	//Transfer A to Y											--Implied
	int CPU::TAY() {

		ta(cpuRegistry.registry.A, cpuRegistry.registry.Y);

		return 0;
	}

	//Transfer Stack Pointer to X								--Implied
	int CPU::TSX() {

		ta(cpuRegistry.sp - 0x0100, cpuRegistry.registry.X);
		
		return 0;
	}

	//Transfer X to A											--Implied
	int CPU::TXA() {

		ta(cpuRegistry.registry.X, cpuRegistry.registry.A);

		return 0;
	}

	//Transfer X to Stack Pointer								--Implied
	int CPU::TXS() {

		ta(cpuRegistry.registry.X + 0x0100, cpuRegistry.sp);

		return 0;
	}

	//Transfer Y to A											--Implied
	int CPU::TYA() {

		ta(cpuRegistry.registry.Y, cpuRegistry.registry.A);

		return 0;
	}

#pragma endregion Storage


#pragma region Math
	//Add M to A with Carry
	int CPU::ADCI() { return 0; }
	int CPU::ADCZP() { return 0; }
	int CPU::ADCZPX() { return 0; }
	int CPU::ADCA() { return 0; }
	int CPU::ADCAX() { return 0; }
	int CPU::ADCAY() { return 0; }
	int CPU::ADCIX() { return 0; }
	int CPU::ADCIY() { return 0; }

#define de(registryOrMemory, val) registryOrMemory -= val;
	//Decrement M by One
	int CPU::DECZP() {

		uint16_t memloc = 0x0000;

		de(memory[memloc], 1);
		return 0;
	}
	int CPU::DECZPX() {

		uint16_t memloc = 0x0000;

		de(memory[memloc], 1);
		return 0;
	}
	int CPU::DECA() {

		uint16_t memloc = 0x0000;

		de(memory[memloc], 1);
		return 0;
	}
	int CPU::DECAX() {

		uint16_t memloc = 0x0000;

		de(memory[memloc], 1);
		return 0;
	}

	//Decrement X by One											--Implied
	int CPU::DEX() { 
		de(cpuRegistry.registry.X, 1);
		return 0; 
	}

	//Decrement Y by One											--Implied
	int CPU::DEY() {
		de(cpuRegistry.registry.Y, 1);
		return 0; 
	}

#define in(registryOrMemory, val) registryOrMemory += val;
	//Increment M by One
	int CPU::INCZP() {

		uint16_t memloc = 0x0000;

		in(memory[memloc], 1);
		return 0;
	}
	int CPU::INCZPX() {

		uint16_t memloc = 0x0000;

		in(memory[memloc], 1);
		return 0;
	}
	int CPU::INCA() {

		uint16_t memloc = 0x0000;

		in(memory[memloc], 1);
		return 0;
	}
	int CPU::INCAX() {

		uint16_t memloc = 0x0000;

		in(memory[memloc], 1);
		return 0;
	}

	//Increment X by One											--Implied
	int CPU::INX() {
		in(cpuRegistry.registry.X, 1);
		return 0;
	}

	//Increment Y by One											--Implied
	int CPU::INY() {
		in(cpuRegistry.registry.Y, 1);
		return 0; 
	}

	//Subtract M from A with Borrow
	int CPU::SBCI() { return 0; }
	int CPU::SBCZP() { return 0; }
	int CPU::SBCZPX() { return 0; }
	int CPU::SBCA() { return 0; }
	int CPU::SBCAX() { return 0; }
	int CPU::SBCAY() { return 0; }
	int CPU::SBCIX() { return 0; }
	int CPU::SBCIY() { return 0; }

#pragma endregion Math


#pragma region Bitwise
	//"AND" M with A
	int CPU::ANDI() { return 0; }
	int CPU::ANDZP() { return 0; }
	int CPU::ANDZPX() { return 0; }
	int CPU::ANDA() { return 0; }
	int CPU::ANDAX() { return 0; }
	int CPU::ANDAY() { return 0; }
	int CPU::ANDIX() { return 0; }
	int CPU::ANDIY() { return 0; }

	//Shift Left One Bit(M or A)
	int CPU::ASLACC() { return 0; }
	int CPU::ASLZP() { return 0; }
	int CPU::ASLZPX() { return 0; }
	int CPU::ASLA() { return 0; }
	int CPU::ASLAX() { return 0; }


	//Test Bits in M with A
	int CPU::BITZP() { return 0; }
	int CPU::BITA() { return 0; }

	//"Exclusive-Or" M with A
	int CPU::EORI() { return 0; }
	int CPU::EORZP() { return 0; }
	int CPU::EORZPX() { return 0; }
	int CPU::EORA() { return 0; }
	int CPU::EORAX() { return 0; }
	int CPU::EORAY() { return 0; }
	int CPU::EORIX() { return 0; }
	int CPU::EORIY() { return 0; }

	//Shift Right One Bit(M or A)
	int CPU::LSRACC() { return 0; }
	int CPU::LSRZP() { return 0; }
	int CPU::LSRZPX() { return 0; }
	int CPU::LSRA() { return 0; }
	int CPU::LSRAX() { return 0; }


	//"OR" M with A
	int CPU::ORAI() { return 0; }
	int CPU::ORAZP() { return 0; }
	int CPU::ORAZPX() { return 0; }
	int CPU::ORAA() { return 0; }
	int CPU::ORAAX() { return 0; }
	int CPU::ORAAY() { return 0; }
	int CPU::ORAIX() { return 0; }
	int CPU::ORAIY() { return 0; }

	//Rotate One Bit Left(M or A)
	int CPU::ROLACC() { return 0; }
	int CPU::ROLZP() { return 0; }
	int CPU::ROLZPX() { return 0; }
	int CPU::ROLA() { return 0; }
	int CPU::ROLAX() { return 0; }


	//Rotate One Bit Right(M or A)
	int CPU::RORACC() { return 0; }
	int CPU::RORZP() { return 0; }
	int CPU::RORZPX() { return 0; }
	int CPU::RORA() { return 0; }
	int CPU::RORAX() { return 0; }

#pragma endregion Bitwise


#pragma region Branch
	//Branch on Carry Clear										--RELATIVE
	int CPU::BCC() { return 0; }

	//Branch on Carry Set										--RELATIVE
	int CPU::BCS() { return 0; }

	//Branch on Result Zero										--RELATIVE
	int CPU::BEQ() { return 0; }

	//Branch on Result Minus									--RELATIVE
	int CPU::BMI() { return 0; }

	//Branch on Result not Zero									--RELATIVE
	int CPU::BNE() { return 0; }

	//Branch on Result Plus										--RELATIVE
	int CPU::BPL() { return 0; }

	//Branch on Overflow Clear									--RELATIVE
	int CPU::BVC() { return 0; }

	//Branch on Overflow Set									--RELATIVE
	int CPU::BVS() { return 0; }
#pragma endregion Branch


#pragma region Jump
	int CPU::JMP(const AddressingMode &mode) {
		int cycleTime = 2;
		uint16_t PC = cpuRegistry.pc;
		uint8_t PCl = fetch();
		uint8_t PCh = 0x00;
		uint16_t read = 0x0000;

		switch (mode)
		{
		case Absolute:
			cycleTime = 0;
			PCh = fetch();
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			read |= PCl;
			read |= (PCh << 8);
#else
			read |= PCh;
			read |= (PCl << 8);
#endif
			//pointerVal = fetch16();
			break;
		case Indirect:
			cycleTime = 0;
			PC |= (PC >> 8); //right shift low bytes away;
			PC |= (PC << 8); //left shift low bytes to high;
			cpuRegistry.pc = PC; //set temp pc based on high bytes;
			PCh = fetch();

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			read |= PCl;
			read |= (PCh << 8);
#else
			read |= PCh;
			read |= (PCl << 8);
#endif

			//TODO: Absolute-Indirect
			break;
		}

		cpuRegistry.pc = read;

		return cycleTime;
	} //Jump to Location
	int CPU::JMPA() { return JMP(Absolute); }
	int CPU::JMPI() { return JMP(Indirect); }

	int CPU::JSR() {
		int cycleTime = 2;
		//check if you need to PUSH (PC+2) | (PC-1) and if it needs to be the PC on load - 1 / + 2 or after reading OP+1 & OP+2.

		uint16_t PC = cpuRegistry.pc;
		uint8_t PCl = fetch();
		uint8_t PCh = 0x00;
		uint16_t read = 0x0000;

		cycleTime = 3;
		PCh = fetch();
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		read |= PCl;
		read |= (PCh << 8);
#else
		read |= PCh;
		read |= (PCl << 8);
#endif

		cpuRegistry.pc = read;

		return cycleTime;
	} //Jump to Location Save Return Address						--ABSOLUTE
	int CPU::RTI() { return 0; } //Return from Interrupt										--Implied
	int CPU::RTS() { return 0; } //Return from Subroutine									--Implied
#pragma endregion Jump


#pragma region Registers
	int CPU::CLC() { cpuRegistry.p.C = 0; return 0; } //Clear Carry Flag											--Implied
	int CPU::CLD() { cpuRegistry.p.D = 0; return 0; } //Clear Decimal Mode										--Implied
	int CPU::CLI() { cpuRegistry.p.I = 0; return 0; } //Clear interrupt Disable Bit								--Implied
	int CPU::CLV() { cpuRegistry.p.V = 0; return 0; } //Clear Overflow Flag										--Implied

	int CPU::CMP_ALL(const AddressingMode &mode, char registry)
	{
		int cycleTime = 2;
		uint8_t reg;
		uint8_t M = NULL;
		uint8_t result = 0xFF;

		switch (registry)
		{
		case 'X':
			reg = cpuRegistry.registry.X;
			break;
		case 'Y':
			reg = cpuRegistry.registry.Y;
			break;
		case 'A':
			reg = cpuRegistry.registry.A;
			break;
		}

		switch (mode)
		{
		case ZeroPage:
			break;
		case ZeroPageX:
			break;
		case Absolute:
			break;
		case AbsoluteX:
			break;
		case AbsoluteY:
			break;
		case IndexedIndirectX:
			break;
		case IndirectIndexedY:
			break;
		default:
			break;
		}

		(reg >= M) ? SEC() : CLC();
		result = reg;
		result -= M; //if result = 0 then Z is 1. Z is the comparers result.
		SET_ZN(result);

		return cycleTime;
	}

	int CPU::CMPI() { return CMP_ALL(Immediate, 'A'); }
	int CPU::CMPZP() { return CMP_ALL(ZeroPage, 'A'); }
	int CPU::CMPZPX() { return CMP_ALL(ZeroPageX, 'A'); }
	int CPU::CMPA() { return CMP_ALL(Absolute, 'A'); }
	int CPU::CMPAX() { return CMP_ALL(AbsoluteX, 'A'); }
	int CPU::CMPAY() { return CMP_ALL(AbsoluteY, 'A'); }
	int CPU::CMPIX() { return CMP_ALL(IndexedIndirectX, 'A'); }
	int CPU::CMPIY() { return CMP_ALL(IndirectIndexedY, 'A'); }

	//Compare M and X*/
	int CPU::CPXI() { return CMP_ALL(Immediate, 'X'); }
	int CPU::CPXZP() { return CMP_ALL(ZeroPage, 'X'); }
	int CPU::CPXA() { return CMP_ALL(Absolute, 'X'); }

	//Compare M and Y*/
	int CPU::CPYI() { return CMP_ALL(Immediate, 'Y'); }
	int CPU::CPYZP() { return CMP_ALL(Immediate, 'Y'); }
	int CPU::CPYA() { return CMP_ALL(Absolute, 'Y'); }

	int CPU::SEC() { cpuRegistry.p.C = 1; return 0; } //Set Carry Flag											--Implied
	int CPU::SED() { cpuRegistry.p.D = 1; return 0; } //Set Decimal Mode											--Implied
	int CPU::SEI() { cpuRegistry.p.I = 1; return 0; } //Set Interrupt Disable Status								--Implied
	int CPU::SEV() { cpuRegistry.p.V = 1; return 0; } //Unofficial OP
#pragma endregion Registers


#pragma region Stack
	//Push A on Stack											--Implied
	int CPU::PHA() {


		return 0;
	} 

	//Push Processor Status on Stack							--Implied
	int CPU::PHP() {


		return 0;
	} 
	
	//Pull A from Stack											--Implied
	int CPU::PLA() {


		return 0;
	} 
	
	//Pull Processor Status from Stack							--Implied
	int CPU::PLP() {


		return 0;
	} 
#pragma endregion Stack


#pragma region Interrupts
	int CPU::NOP() { return 0; }		//No Operation											--Implied
	int CPU::NMI() { return 0; }		//Non-Maskable Intteruption
	int CPU::Reset() { return 0; }		//Reset
	int CPU::IRQ() { return 0; }		//Interrupt Request
	int CPU::BRK() { return 0; }		//Break													--Implied
#pragma endregion


#pragma region Custom
										//Do not integrate (now)
	void CPU::SET_ZN(const char &value)
	{
		SEZ(value);
		SEN(value);
	}
	//set negative
	void CPU::SEN(const char &value)
	{
		if (value & 0x80 != 0)
		{
			cpuRegistry.p.N = 1;
		}
		else
		{
			cpuRegistry.p.N = 0;
		}
	}
	//set negative
	void CPU::SEN()
	{
		cpuRegistry.p.N = 1;
	}
	//clear negative
	void CPU::CLN()
	{
		cpuRegistry.p.N = 0;
	}
	//set zero
	void CPU::SEZ(const char &value)
	{
		if (value == 0)
		{
			cpuRegistry.p.Z = 1;
		}
		else
		{
			cpuRegistry.p.Z = 0;
		}
	}
#pragma endregion Custom