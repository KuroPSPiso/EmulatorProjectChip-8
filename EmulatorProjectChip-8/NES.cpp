#include "NES.h"

void NES::write(const uint16_t &address, const uint8_t value)
{
	memory[address] = value;
}
void NES::add(const uint16_t &address, const uint8_t value)
{
	memory[address] += value;
}
void NES::subtract(const uint16_t &address, const uint8_t value)
{
	memory[address] -= value;
}
const uint8_t& NES::read(const uint16_t &address)
{
	return memory[address];
}

void NES::emulateCycle()
{
	opcode = fetch();
	decodeOPCode(opcode);

	//TODO: remove here
	printf(NESOpCodeTableNames[opcode]);
}

void NES::RESET()
{
	NES::InitilizeOpCodeTable();
}

bool NES::loadApplication(const char * filename)
{
	return false;
}

void NES::debugDisplay()
{
}

uint8_t NES::fetch()
{
	uint8_t data = memory[cpuRegistery.pc];
	cpuRegistery.pc++; //next process step
	return data;
}


void NES::SET_ZN(const char &value)
{
	this->SET_ZERO(value);
	this->SET_NEGATIVE(value);
}
void NES::SET_NEGATIVE(const char &value)
{
	if (value & 0x80 != 0)
	{
		this->cpuRegistery.p.N = 1;
	}
	else
	{
		this->cpuRegistery.p.N = 0;
	}
}
void NES::SET_ZERO(const char &value) 
{
	if (value == 0)
	{
		cpuRegistery.p.Z = 1;
	}
	else 
	{
		cpuRegistery.p.Z = 0;
	}
}

uint8_t NES::decodeOPCode(const uint8_t &opcode)
{
	int tickStates = 0;
	//uint8_t n, readVal, subOpCode = NULL;

	try
	{
		tickStates = (this->*(NESOpCodeTable[opcode]))();
	}
	catch (std::exception &e)
	{
		printf(e.what());
	}

	return tickStates;
}


void NES::InitilizeOpCodeTable()
{
	//everycommand is either no operation or returns combining opcode.
	for (int i = 0x00; i < 0xFF; i++)
	{
		NESOpCodeTable[i] = &NES::NOP;
	}

#pragma region OPCODE TABLE
	//NESOpCodeTable[0x00] = &NES::NOP;
	NESOpCodeTable[0x00] = &NES::BRK;
	NESOpCodeTable[0x01] = &NES::ORAIX;
	NESOpCodeTable[0x05] = &NES::ORAZP;
	NESOpCodeTable[0x06] = &NES::ASLZP;
	NESOpCodeTable[0x08] = &NES::PHP;
	NESOpCodeTable[0x09] = &NES::ORAI;
	NESOpCodeTable[0x0A] = &NES::ASLACC;
	NESOpCodeTable[0x0D] = &NES::ORAA;
	NESOpCodeTable[0x0E] = &NES::ASLA;
	NESOpCodeTable[0x10] = &NES::BPL;
	NESOpCodeTable[0x11] = &NES::ORAIY;
	NESOpCodeTable[0x15] = &NES::ORAZPX;
	NESOpCodeTable[0x16] = &NES::ASLZPX;
	NESOpCodeTable[0x18] = &NES::CLC;
	NESOpCodeTable[0x19] = &NES::ORAAY;
	NESOpCodeTable[0x1D] = &NES::ORAAX;
	NESOpCodeTable[0x1E] = &NES::ASLAX;
	NESOpCodeTable[0x20] = &NES::JSR;
	NESOpCodeTable[0x21] = &NES::ANDIX;
	NESOpCodeTable[0x24] = &NES::BITZP;
	NESOpCodeTable[0x25] = &NES::ANDZP;
	NESOpCodeTable[0x26] = &NES::ROLZP;
	NESOpCodeTable[0x28] = &NES::PLP;
	NESOpCodeTable[0x29] = &NES::ANDI;
	NESOpCodeTable[0x2A] = &NES::ROLACC;
	NESOpCodeTable[0x2C] = &NES::BITA;
	NESOpCodeTable[0x2D] = &NES::ANDA;
	NESOpCodeTable[0x2E] = &NES::ROLA;
	NESOpCodeTable[0x30] = &NES::BMI;
	NESOpCodeTable[0x31] = &NES::ANDIY;
	NESOpCodeTable[0x35] = &NES::ANDZPX;
	NESOpCodeTable[0x36] = &NES::ROLZPX;
	NESOpCodeTable[0x38] = &NES::SEC;
	NESOpCodeTable[0x39] = &NES::ANDAY;
	NESOpCodeTable[0x3D] = &NES::ANDAX;
	NESOpCodeTable[0x3E] = &NES::ROLAX;
	NESOpCodeTable[0x40] = &NES::RTI;
	NESOpCodeTable[0x41] = &NES::EORIX;
	NESOpCodeTable[0x45] = &NES::EORZP;
	NESOpCodeTable[0x46] = &NES::LSRZP;
	NESOpCodeTable[0x48] = &NES::PHA;
	NESOpCodeTable[0x49] = &NES::EORI;
	NESOpCodeTable[0x4A] = &NES::LSRACC;
	NESOpCodeTable[0x4C] = &NES::JMPA;
	NESOpCodeTable[0x4D] = &NES::EORA;
	NESOpCodeTable[0x4E] = &NES::LSRA;
	NESOpCodeTable[0x50] = &NES::BVC;
	NESOpCodeTable[0x51] = &NES::EORIY;
	NESOpCodeTable[0x55] = &NES::EORZPX;
	NESOpCodeTable[0x56] = &NES::LSRZPX;
	NESOpCodeTable[0x58] = &NES::CLI;
	NESOpCodeTable[0x59] = &NES::EORAY;
	NESOpCodeTable[0x5D] = &NES::EORAX;
	NESOpCodeTable[0x5E] = &NES::LSRAX;
	NESOpCodeTable[0x60] = &NES::RTS;
	NESOpCodeTable[0x61] = &NES::ADCIX;
	NESOpCodeTable[0x65] = &NES::ADCZP;
	NESOpCodeTable[0x66] = &NES::RORZP;
	NESOpCodeTable[0x68] = &NES::PLA;
	NESOpCodeTable[0x69] = &NES::ADCI;
	NESOpCodeTable[0x6A] = &NES::RORACC;
	NESOpCodeTable[0x6C] = &NES::JMPI;
	NESOpCodeTable[0x6D] = &NES::ADCA;
	NESOpCodeTable[0x6E] = &NES::RORA;
	NESOpCodeTable[0x70] = &NES::BVS;
	NESOpCodeTable[0x71] = &NES::ADCIY;
	NESOpCodeTable[0x75] = &NES::ADCZPX;
	NESOpCodeTable[0x76] = &NES::RORZPX;
	NESOpCodeTable[0x78] = &NES::SEI;
	NESOpCodeTable[0x79] = &NES::ADCAY;
	NESOpCodeTable[0x7D] = &NES::ADCAX;
	NESOpCodeTable[0x7E] = &NES::RORAX;
	NESOpCodeTable[0x81] = &NES::STAIX;
	NESOpCodeTable[0x84] = &NES::STYZP;
	NESOpCodeTable[0x85] = &NES::STAZP;
	NESOpCodeTable[0x86] = &NES::STXZP;
	NESOpCodeTable[0x88] = &NES::DEY;
	NESOpCodeTable[0x8A] = &NES::TXA;
	NESOpCodeTable[0x8C] = &NES::STYA;
	NESOpCodeTable[0x8D] = &NES::STAA;
	NESOpCodeTable[0x8E] = &NES::STXA;
	NESOpCodeTable[0x90] = &NES::BCC;
	NESOpCodeTable[0x91] = &NES::STAIY;
	NESOpCodeTable[0x94] = &NES::STYZPX;
	NESOpCodeTable[0x95] = &NES::STAZPX;
	NESOpCodeTable[0x96] = &NES::STXZPY;
	NESOpCodeTable[0x98] = &NES::TYA;
	NESOpCodeTable[0x99] = &NES::STAAY;
	NESOpCodeTable[0x9A] = &NES::TXS;
	NESOpCodeTable[0x9D] = &NES::STAAX;
	NESOpCodeTable[0xA0] = &NES::LDYI;
	NESOpCodeTable[0xA1] = &NES::LDAIX;
	NESOpCodeTable[0xA2] = &NES::LDXI;
	NESOpCodeTable[0xA4] = &NES::LDYZP;
	NESOpCodeTable[0xA5] = &NES::LDAZP;
	NESOpCodeTable[0xA6] = &NES::LDXZP;
	NESOpCodeTable[0xA8] = &NES::TAY;
	NESOpCodeTable[0xA9] = &NES::LDAI;
	NESOpCodeTable[0xAA] = &NES::TAX;
	NESOpCodeTable[0xAC] = &NES::LDYA;
	NESOpCodeTable[0xAD] = &NES::LDAA;
	NESOpCodeTable[0xAE] = &NES::LDXA;
	NESOpCodeTable[0xB0] = &NES::BCS;
	NESOpCodeTable[0xB1] = &NES::LDAIY;
	NESOpCodeTable[0xB4] = &NES::LDYZPX;
	NESOpCodeTable[0xB5] = &NES::LDAZPX;
	NESOpCodeTable[0xB6] = &NES::LDXZPY;
	NESOpCodeTable[0xB8] = &NES::CLV;
	NESOpCodeTable[0xB9] = &NES::LDAAY;
	NESOpCodeTable[0xBA] = &NES::TSX;
	NESOpCodeTable[0xBC] = &NES::LDYAX;
	NESOpCodeTable[0xBD] = &NES::LDAAX;
	NESOpCodeTable[0xBE] = &NES::LDXAY;
	NESOpCodeTable[0xC0] = &NES::CPYI;
	NESOpCodeTable[0xC1] = &NES::CMPIX;
	NESOpCodeTable[0xC4] = &NES::CPYZP;
	NESOpCodeTable[0xC5] = &NES::CMPZP;
	NESOpCodeTable[0xC6] = &NES::DECZP;
	NESOpCodeTable[0xC8] = &NES::INY;
	NESOpCodeTable[0xC9] = &NES::CMPI;
	NESOpCodeTable[0xCA] = &NES::DEX;
	NESOpCodeTable[0xCC] = &NES::CPYA;
	NESOpCodeTable[0xCD] = &NES::CMPA;
	NESOpCodeTable[0xCE] = &NES::DECA;
	NESOpCodeTable[0xD0] = &NES::BNE;
	NESOpCodeTable[0xD1] = &NES::CMPIY;
	NESOpCodeTable[0xD5] = &NES::CMPZPX;
	NESOpCodeTable[0xD6] = &NES::DECZPX;
	NESOpCodeTable[0xD8] = &NES::CLD;
	NESOpCodeTable[0xD9] = &NES::CMPAY;
	NESOpCodeTable[0xDD] = &NES::CMPAX;
	NESOpCodeTable[0xDE] = &NES::DECAX;
	NESOpCodeTable[0xE0] = &NES::CPXI;
	NESOpCodeTable[0xE1] = &NES::SBCIX;
	NESOpCodeTable[0xE4] = &NES::CPXZP;
	NESOpCodeTable[0xE5] = &NES::SBCZP;
	NESOpCodeTable[0xE6] = &NES::INCZP;
	NESOpCodeTable[0xE8] = &NES::INX;
	NESOpCodeTable[0xE9] = &NES::SBCI;
	NESOpCodeTable[0xEA] = &NES::NOP;
	NESOpCodeTable[0xEC] = &NES::CPXA;
	NESOpCodeTable[0xED] = &NES::SBCA;
	NESOpCodeTable[0xEE] = &NES::INCA;
	NESOpCodeTable[0xF0] = &NES::BEQ;
	NESOpCodeTable[0xF1] = &NES::SBCIY;
	NESOpCodeTable[0xF5] = &NES::SBCZPX;
	NESOpCodeTable[0xF6] = &NES::INCZPX;
	NESOpCodeTable[0xF8] = &NES::SED;
	NESOpCodeTable[0xF9] = &NES::SBCAY;
	NESOpCodeTable[0xFD] = &NES::SBCAX;
	NESOpCodeTable[0xFE] = &NES::INCAX;
#pragma endregion OPCODE TABLE
#pragma region OPCODE TABLENAMES
	NESOpCodeTableNames[0x00] = "BRK";
	NESOpCodeTableNames[0x01] = "ORAIX";
	NESOpCodeTableNames[0x05] = "ORAZP";
	NESOpCodeTableNames[0x06] = "ASLZP";
	NESOpCodeTableNames[0x08] = "PHP";
	NESOpCodeTableNames[0x09] = "ORAI";
	NESOpCodeTableNames[0x0A] = "ASLACC";
	NESOpCodeTableNames[0x0D] = "ORAA";
	NESOpCodeTableNames[0x0E] = "ASLA";
	NESOpCodeTableNames[0x10] = "BPL";
	NESOpCodeTableNames[0x11] = "ORAIY";
	NESOpCodeTableNames[0x15] = "ORAZPX";
	NESOpCodeTableNames[0x16] = "ASLZPX";
	NESOpCodeTableNames[0x18] = "CLC";
	NESOpCodeTableNames[0x19] = "ORAAY";
	NESOpCodeTableNames[0x1D] = "ORAAX";
	NESOpCodeTableNames[0x1E] = "ASLAX";
	NESOpCodeTableNames[0x20] = "JSR";
	NESOpCodeTableNames[0x21] = "ANDIX";
	NESOpCodeTableNames[0x24] = "BITZP";
	NESOpCodeTableNames[0x25] = "ANDZP";
	NESOpCodeTableNames[0x26] = "ROLZP";
	NESOpCodeTableNames[0x28] = "PLP";
	NESOpCodeTableNames[0x29] = "ANDI";
	NESOpCodeTableNames[0x2A] = "ROLACC";
	NESOpCodeTableNames[0x2C] = "BITA";
	NESOpCodeTableNames[0x2D] = "ANDA";
	NESOpCodeTableNames[0x2E] = "ROLA";
	NESOpCodeTableNames[0x30] = "BMI";
	NESOpCodeTableNames[0x31] = "ANDIY";
	NESOpCodeTableNames[0x35] = "ANDZPX";
	NESOpCodeTableNames[0x36] = "ROLZPX";
	NESOpCodeTableNames[0x38] = "SEC";
	NESOpCodeTableNames[0x39] = "ANDAY";
	NESOpCodeTableNames[0x3D] = "ANDAX";
	NESOpCodeTableNames[0x3E] = "ROLAX";
	NESOpCodeTableNames[0x40] = "RTI";
	NESOpCodeTableNames[0x41] = "EORIX";
	NESOpCodeTableNames[0x45] = "EORZP";
	NESOpCodeTableNames[0x46] = "LSRZP";
	NESOpCodeTableNames[0x48] = "PHA";
	NESOpCodeTableNames[0x49] = "EORI";
	NESOpCodeTableNames[0x4A] = "LSRACC";
	NESOpCodeTableNames[0x4C] = "JMPA";
	NESOpCodeTableNames[0x4D] = "EORA";
	NESOpCodeTableNames[0x4E] = "LSRA";
	NESOpCodeTableNames[0x50] = "BVC";
	NESOpCodeTableNames[0x51] = "EORIY";
	NESOpCodeTableNames[0x55] = "EORZPX";
	NESOpCodeTableNames[0x56] = "LSRZPX";
	NESOpCodeTableNames[0x58] = "CLI";
	NESOpCodeTableNames[0x59] = "EORAY";
	NESOpCodeTableNames[0x5D] = "EORAX";
	NESOpCodeTableNames[0x5E] = "LSRAX";
	NESOpCodeTableNames[0x60] = "RTS";
	NESOpCodeTableNames[0x61] = "ADCIX";
	NESOpCodeTableNames[0x65] = "ADCZP";
	NESOpCodeTableNames[0x66] = "RORZP";
	NESOpCodeTableNames[0x68] = "PLA";
	NESOpCodeTableNames[0x69] = "ADCI";
	NESOpCodeTableNames[0x6A] = "RORACC";
	NESOpCodeTableNames[0x6C] = "JMPI";
	NESOpCodeTableNames[0x6D] = "ADCA";
	NESOpCodeTableNames[0x6E] = "RORA";
	NESOpCodeTableNames[0x70] = "BVS";
	NESOpCodeTableNames[0x71] = "ADCIY";
	NESOpCodeTableNames[0x75] = "ADCZPX";
	NESOpCodeTableNames[0x76] = "RORZPX";
	NESOpCodeTableNames[0x78] = "SEI";
	NESOpCodeTableNames[0x79] = "ADCAY";
	NESOpCodeTableNames[0x7D] = "ADCAX";
	NESOpCodeTableNames[0x7E] = "RORAX";
	NESOpCodeTableNames[0x81] = "STAIX";
	NESOpCodeTableNames[0x84] = "STYZP";
	NESOpCodeTableNames[0x85] = "STAZP";
	NESOpCodeTableNames[0x86] = "STXZP";
	NESOpCodeTableNames[0x88] = "DEY";
	NESOpCodeTableNames[0x8A] = "TXA";
	NESOpCodeTableNames[0x8C] = "STYA";
	NESOpCodeTableNames[0x8D] = "STAA";
	NESOpCodeTableNames[0x8E] = "STXA";
	NESOpCodeTableNames[0x90] = "BCC";
	NESOpCodeTableNames[0x91] = "STAIY";
	NESOpCodeTableNames[0x94] = "STYZPX";
	NESOpCodeTableNames[0x95] = "STAZPX";
	NESOpCodeTableNames[0x96] = "STXZPY";
	NESOpCodeTableNames[0x98] = "TYA";
	NESOpCodeTableNames[0x99] = "STAAY";
	NESOpCodeTableNames[0x9A] = "TXS";
	NESOpCodeTableNames[0x9D] = "STAAX";
	NESOpCodeTableNames[0xA0] = "LDYI";
	NESOpCodeTableNames[0xA1] = "LDAIX";
	NESOpCodeTableNames[0xA2] = "LDXI";
	NESOpCodeTableNames[0xA4] = "LDYZP";
	NESOpCodeTableNames[0xA5] = "LDAZP";
	NESOpCodeTableNames[0xA6] = "LDXZP";
	NESOpCodeTableNames[0xA8] = "TAY";
	NESOpCodeTableNames[0xA9] = "LDAI";
	NESOpCodeTableNames[0xAA] = "TAX";
	NESOpCodeTableNames[0xAC] = "LDYA";
	NESOpCodeTableNames[0xAD] = "LDAA";
	NESOpCodeTableNames[0xAE] = "LDXA";
	NESOpCodeTableNames[0xB0] = "BCS";
	NESOpCodeTableNames[0xB1] = "LDAIY";
	NESOpCodeTableNames[0xB4] = "LDYZPX";
	NESOpCodeTableNames[0xB5] = "LDAZPX";
	NESOpCodeTableNames[0xB6] = "LDXZPY";
	NESOpCodeTableNames[0xB8] = "CLV";
	NESOpCodeTableNames[0xB9] = "LDAAY";
	NESOpCodeTableNames[0xBA] = "TSX";
	NESOpCodeTableNames[0xBC] = "LDYAX";
	NESOpCodeTableNames[0xBD] = "LDAAX";
	NESOpCodeTableNames[0xBE] = "LDXAY";
	NESOpCodeTableNames[0xC0] = "CPYI";
	NESOpCodeTableNames[0xC1] = "CMPIX";
	NESOpCodeTableNames[0xC4] = "CPYZP";
	NESOpCodeTableNames[0xC5] = "CMPZP";
	NESOpCodeTableNames[0xC6] = "DECZP";
	NESOpCodeTableNames[0xC8] = "INY";
	NESOpCodeTableNames[0xC9] = "CMPI";
	NESOpCodeTableNames[0xCA] = "DEX";
	NESOpCodeTableNames[0xCC] = "CPYA";
	NESOpCodeTableNames[0xCD] = "CMPA";
	NESOpCodeTableNames[0xCE] = "DECA";
	NESOpCodeTableNames[0xD0] = "BNE";
	NESOpCodeTableNames[0xD1] = "CMPIY";
	NESOpCodeTableNames[0xD5] = "CMPZPX";
	NESOpCodeTableNames[0xD6] = "DECZPX";
	NESOpCodeTableNames[0xD8] = "CLD";
	NESOpCodeTableNames[0xD9] = "CMPAY";
	NESOpCodeTableNames[0xDD] = "CMPAX";
	NESOpCodeTableNames[0xDE] = "DECAX";
	NESOpCodeTableNames[0xE0] = "CPXI";
	NESOpCodeTableNames[0xE1] = "SBCIX";
	NESOpCodeTableNames[0xE4] = "CPXZP";
	NESOpCodeTableNames[0xE5] = "SBCZP";
	NESOpCodeTableNames[0xE6] = "INCZP";
	NESOpCodeTableNames[0xE8] = "INX";
	NESOpCodeTableNames[0xE9] = "SBCI";
	NESOpCodeTableNames[0xEA] = "NOP";
	NESOpCodeTableNames[0xEC] = "CPXA";
	NESOpCodeTableNames[0xED] = "SBCA";
	NESOpCodeTableNames[0xEE] = "INCA";
	NESOpCodeTableNames[0xF0] = "BEQ";
	NESOpCodeTableNames[0xF1] = "SBCIY";
	NESOpCodeTableNames[0xF5] = "SBCZPX";
	NESOpCodeTableNames[0xF6] = "INCZPX";
	NESOpCodeTableNames[0xF8] = "SED";
	NESOpCodeTableNames[0xF9] = "SBCAY";
	NESOpCodeTableNames[0xFD] = "SBCAX";
	NESOpCodeTableNames[0xFE] = "INCAX";
#pragma endregion OPCODE TABLENAMES

}

#pragma region OPCODE Methods

#pragma region Storage
//Load A with M
int NES::LDA(const AddressingMode &mode) { 
	int cycleTime = 2; 

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		break;
	case ZeroPageX:
		cycleTime = 4;
		break;
	case Absolute:
		cycleTime = 4;
		break;
	case AbsoluteX:
		cycleTime = 4;
		break;
	case AbsoluteY:
		cycleTime = 4;
		break;
	case IndexedIndirectX:
		cycleTime = 6;
		break;
	case IndirectIndexedY:
		cycleTime = 5;
		break;
	default:
		break;
	}

	return cycleTime; 
} 
int NES::LDAI() { return LDA(Inmediate); }
int NES::LDAZP() { return LDA(ZeroPage); }
int NES::LDAZPX() { return LDA(ZeroPageX); }
int NES::LDAA() { return LDA(Absolute); }
int NES::LDAAX() { return LDA(AbsoluteX); }
int NES::LDAAY() { return LDA(AbsoluteY); }
int NES::LDAIX() { return LDA(IndexedIndirectX); }
int NES::LDAIY() { return LDA(IndirectIndexedY); }

//Load X with M
int NES::LDX(const AddressingMode &mode) { 
	int cycleTime = 2;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		break;
	case ZeroPageY:
		cycleTime = 4;
		break;
	case Absolute:
		cycleTime = 4;
		break;
	case AbsoluteY:
		cycleTime = 4;
		break;
	default:
		break;
	}
	return cycleTime; 
}
int NES::LDXI() { return LDX(Inmediate); }
int NES::LDXZP() { return LDX(ZeroPage); }
int NES::LDXZPY() { return LDX(ZeroPageY); }
int NES::LDXA() { return LDX(Absolute); }
int NES::LDXAY() { return LDX(AbsoluteY); }

//Load Y with M
int NES::LDY(const AddressingMode &mode) { 
	int cycleTime = 2;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		break;
	case ZeroPageX:
		cycleTime = 4;
		break;
	case Absolute:
		cycleTime = 4;
		break;
	case AbsoluteX:
		cycleTime = 4;
		break;
	default:
		break;
	}
	return cycleTime; 
} 
int NES::LDYI() { return LDY(Inmediate); }
int NES::LDYZP() { return LDY(ZeroPage); }
int NES::LDYZPX() { return LDY(ZeroPageX); }
int NES::LDYA() { return LDY(Absolute); }
int NES::LDYAX() { return LDY(AbsoluteX); }

//Store A in M
int NES::STA(const AddressingMode &mode) { 
	int cycleTime = 0;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		break;
	case ZeroPageX:
		cycleTime = 4;
		break;
	case Absolute:
		cycleTime = 4;
		break;
	case AbsoluteX:
		cycleTime = 5;
		break;
	case AbsoluteY:
		cycleTime = 5;
		break;
	case IndexedIndirectX:
		cycleTime = 6;
		break;
	case IndirectIndexedY:
		cycleTime = 6;
		break;
	}
	return cycleTime;
} 
int NES::STAZP() { return STA(ZeroPage); }
int NES::STAZPX() { return STA(ZeroPageX); }
int NES::STAA() { return STA(Absolute); }
int NES::STAAX() { return STA(AbsoluteX); }
int NES::STAAY() { return STA(AbsoluteY); }
int NES::STAIX() { return STA(IndexedIndirectX); }
int NES::STAIY() { return STA(IndirectIndexedY); }

//Store X in M
int NES::STX(const AddressingMode &mode) {
	int cycleTime = 0;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		break;
	case ZeroPageY:
		cycleTime = 4;
		break;
	case Absolute:
		cycleTime = 4;
		break;
	}
	return cycleTime;
} 
int NES::STXZP() { return STX(ZeroPage); }
int NES::STXZPY() { return STX(ZeroPageY); }
int NES::STXA() { return STX(Absolute); }

//Store Y in M
int NES::STY(const AddressingMode &mode) { 
	int cycleTime = 0;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		break;
	case ZeroPageX:
		cycleTime = 4;
		break;
	case Absolute:
		cycleTime = 4;
		break;
	}
	return cycleTime; 
} 
int NES::STYZP() { return STY(ZeroPage); }
int NES::STYZPX() { return STY(ZeroPageX); }
int NES::STYA() { return STY(Absolute); }

//Transfer A to X											--Implied
int NES::TAX() { 
	int cycleTime = 2; 
	return cycleTime; 
} 

//Transfer A to Y											--Implied
int NES::TAY() { 
	int cycleTime = 2; 
	return cycleTime; 
} 

//Transfer Stack Pointer to X								--Implied
int NES::TSX() { 
	int cycleTime = 2; 
	return cycleTime; 
} 

//Transfer X to A											--Implied
int NES::TXA() { 
	int cycleTime = 2; 
	return cycleTime; 
} 

//Transfer X to Stack Pointer								--Implied
int NES::TXS() { 
	int cycleTime = 2; 
	return cycleTime; 
} 

//Transfer Y to A											--Implied
int NES::TYA() { 
	int cycleTime = 2; 
	return cycleTime; 
} 

#pragma endregion Storage


#pragma region Math
//Add M to A with Carry
int NES::ADC(const AddressingMode &mode) { 
	int cycleTime = 2; 
	return cycleTime; 
} 
int NES::ADCI() { return ADC(Inmediate); }
int NES::ADCZP() { return ADC(ZeroPage); }
int NES::ADCZPX() { return ADC(ZeroPageX); }
int NES::ADCA() { return ADC(Absolute); }
int NES::ADCAX() { return ADC(AbsoluteX); }
int NES::ADCAY() { return ADC(AbsoluteY); }
int NES::ADCIX() { return ADC(IndexedIndirectX); }
int NES::ADCIY() { return ADC(IndirectIndexedY); }

//Decrement M by One
int NES::DEC(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } 
int NES::DECZP() { return DEC(ZeroPage); }
int NES::DECZPX() { return DEC(ZeroPageX); }
int NES::DECA() { return DEC(Absolute); }
int NES::DECAX() { return DEC(AbsoluteX); }

//Decrement X by One											--Implied
int NES::DEX() { int cycleTime = 2; return cycleTime; } 

//Decrement Y by One											--Implied
int NES::DEY() { int cycleTime = 2; return cycleTime; } 

//Increment M by One
int NES::INC(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } 
int NES::INCZP() { return INC(ZeroPage); }
int NES::INCZPX() { return INC(ZeroPageX); }
int NES::INCA() { return INC(Absolute); }
int NES::INCAX() { return INC(AbsoluteX); }

//Increment X by One											--Implied
int NES::INX() { int cycleTime = 2; return cycleTime; } 

//Increment Y by One											--Implied
int NES::INY() { int cycleTime = 2; return cycleTime; } 

//Subtract M from A with Borrow
int NES::SBC(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } 
int NES::SBCI() { return SBC(Inmediate); }
int NES::SBCZP() { return SBC(ZeroPage); }
int NES::SBCZPX() { return SBC(ZeroPageX); }
int NES::SBCA() { return SBC(Absolute); }
int NES::SBCAX() { return SBC(AbsoluteX); }
int NES::SBCAY() { return SBC(AbsoluteY); }
int NES::SBCIX() { return SBC(IndexedIndirectX); }
int NES::SBCIY() { return SBC(IndirectIndexedY); }

#pragma endregion Math


#pragma region Bitwise

int NES::AND(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //"AND" M with A
int NES::ANDI() { return AND(Inmediate); }
int NES::ANDZP() { return AND(ZeroPage); }
int NES::ANDZPX() { return AND(ZeroPageX); }
int NES::ANDA() { return AND(Absolute); }
int NES::ANDAX() { return AND(AbsoluteX); }
int NES::ANDAY() { return AND(AbsoluteY); }
int NES::ANDIX() { return AND(IndexedIndirectX); }
int NES::ANDIY() { return AND(IndirectIndexedY); }


int NES::ASL(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //Shift Left One Bit(M or A)
int NES::ASLACC() { return ASL(Accumulator); }
int NES::ASLZP() { return ASL(ZeroPage); }
int NES::ASLZPX() { return ASL(ZeroPageX); }
int NES::ASLA() { return ASL(Absolute); }
int NES::ASLAX() { return ASL(AbsoluteX); }


int NES::BIT(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //Test Bits in M with A
int NES::BITZP() { return BIT(ZeroPage); }
int NES::BITA() { return BIT(Absolute); }


int NES::EOR(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //"Exclusive-Or" M with A
int NES::EORI() { return EOR(Inmediate); }
int NES::EORZP() { return EOR(ZeroPage); }
int NES::EORZPX() { return EOR(ZeroPageX); }
int NES::EORA() { return EOR(Absolute); }
int NES::EORAX() { return EOR(AbsoluteX); }
int NES::EORAY() { return EOR(AbsoluteY); }
int NES::EORIX() { return EOR(IndexedIndirectX); }
int NES::EORIY() { return EOR(IndirectIndexedY); }

int NES::LSR(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //Shift Right One Bit(M or A)
int NES::LSRACC() { return LSR(Accumulator); }
int NES::LSRZP() { return LSR(ZeroPage); }
int NES::LSRZPX() { return LSR(ZeroPageX); }
int NES::LSRA() { return LSR(Absolute); }
int NES::LSRAX() { return LSR(AbsoluteX); }


int NES::ORA(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //"OR" M with A
int NES::ORAI() { return ORA(Inmediate); }
int NES::ORAZP() { return ORA(ZeroPage); }
int NES::ORAZPX() { return ORA(ZeroPageX); }
int NES::ORAA() { return ORA(Absolute); }
int NES::ORAAX() { return ORA(AbsoluteX); }
int NES::ORAAY() { return ORA(AbsoluteY); }
int NES::ORAIX() { return ORA(IndexedIndirectX); }
int NES::ORAIY() { return ORA(IndirectIndexedY); }

int NES::ROL(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //Rotate One Bit Left(M or A)
int NES::ROLACC() { return ROL(Accumulator); }
int NES::ROLZP() { return ROL(ZeroPage); }
int NES::ROLZPX() { return ROL(ZeroPageX); }
int NES::ROLA() { return ROL(Absolute); }
int NES::ROLAX() { return ROL(AbsoluteX); }


int NES::ROR(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //Rotate One Bit Right(M or A)
int NES::RORACC() { return ROR(Accumulator); }
int NES::RORZP() { return ROR(ZeroPage); }
int NES::RORZPX() { return ROR(ZeroPageX); }
int NES::RORA() { return ROR(Absolute); }
int NES::RORAX() { return ROR(AbsoluteX); }

#pragma endregion Bitwise


#pragma region Branch
//Branch on Carry Clear										--RELATIVE
int NES::BCC() { int cycleTime = 2; return cycleTime; } 

//Branch on Carry Set										--RELATIVE
int NES::BCS() { int cycleTime = 2; return cycleTime; } 

//Branch on Result Zero										--RELATIVE
int NES::BEQ() { int cycleTime = 2; return cycleTime; } 

//Branch on Result Minus									--RELATIVE
int NES::BMI() { int cycleTime = 2; return cycleTime; }

//Branch on Result not Zero									--RELATIVE
int NES::BNE() { int cycleTime = 2; return cycleTime; } 

//Branch on Result Plus										--RELATIVE
int NES::BPL() { int cycleTime = 2; return cycleTime; } 

//Branch on Overflow Clear									--RELATIVE
int NES::BVC() { int cycleTime = 2; return cycleTime; } 

//Branch on Overflow Set									--RELATIVE
int NES::BVS() { int cycleTime = 2; return cycleTime; } 
#pragma endregion Branch


#pragma region Jump
int NES::JMP(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //Jump to Location
int NES::JMPA() { return JMP(Absolute); }
int NES::JMPI() { return JMP(Indirect);  }

int NES::JSR() { int cycleTime = 2; return cycleTime; } //Jump to Location Save Return Address						--ABSOLUTE
int NES::RTI() { int cycleTime = 2; return cycleTime; } //Return from Interrupt										--Implied
int NES::RTS() { int cycleTime = 2; return cycleTime; } //Return from Subroutine									--Implied
#pragma endregion Jump


#pragma region Registers
int NES::CLC() { int cycleTime = 2; return cycleTime; } //Clear Carry Flag											--Implied
int NES::CLD() { int cycleTime = 2; return cycleTime; } //Clear Decimal Mode										--Implied
int NES::CLI() { int cycleTime = 2; return cycleTime; } //Clear interrupt Disable Bit								--Implied
int NES::CLV() { int cycleTime = 2; return cycleTime; } //Clear Overflow Flag										--Implied

int NES::CMP(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //Compare M AND A
int NES::CMPI() { return CMP(Inmediate); }
int NES::CMPZP() { return CMP(ZeroPage); }
int NES::CMPZPX() { return CMP(ZeroPageX); }
int NES::CMPA() { return CMP(Absolute); }
int NES::CMPAX() { return CMP(AbsoluteX); }
int NES::CMPAY() { return CMP(AbsoluteY); }
int NES::CMPIX() { return CMP(IndexedIndirectX); }
int NES::CMPIY() { return CMP(IndirectIndexedY); }

int NES::CPX(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //Compare M and X
int NES::CPXI() { return CPX(Inmediate); }
int NES::CPXZP() { return CPX(ZeroPage); }
int NES::CPXA() { return CPX(Absolute); }

int NES::CPY(const AddressingMode &mode) { int cycleTime = 2; return cycleTime; } //Compare M and Y
int NES::CPYI() { return CPY(Inmediate); }
int NES::CPYZP() { return CPY(ZeroPage); }
int NES::CPYA() { return CPY(Absolute); }

int NES::SEC() { int cycleTime = 2; return cycleTime; } //Set Carry Flag											--Implied
int NES::SED() { int cycleTime = 2; return cycleTime; } //Set Decimal Mode											--Implied
int NES::SEI() { int cycleTime = 2; return cycleTime; } //Set Interrupt Disable Status								--Implied
#pragma endregion Registers


#pragma region Stack
int NES::PHA() { int cycleTime = 2; return cycleTime; } //Push A on Stack											--Implied
int NES::PHP() { int cycleTime = 2; return cycleTime; } //Push Processor Status on Stack							--Implied
int NES::PLA() { int cycleTime = 2; return cycleTime; } //Pull A from Stack											--Implied
int NES::PLP() { int cycleTime = 2; return cycleTime; } //Pull Processor Status from Stack							--Implied
#pragma endregion Stack


#pragma region Interrupts
int NES::NOP() { return 2; }		//No Operation											--Implied
int NES::NMI() { return 0; }		//Non-Maskable Intteruption
int NES::Reset() { return 0; }	//Reset
int NES::IRQ() { return 0; }		//Interrupt Request
int NES::BRK() { return 7; }		//Break													--Implied
#pragma endregion
#pragma endregion OPCODE Methods
