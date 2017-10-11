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
	int pc = cpuRegistry.pc;
	opcode = fetch();

	printf(NESOpCodeTableNames[opcode]);//TODO: remove this

	int cycleTime = decodeOPCode(opcode);

	if(
		opcode != 0x28 &&
		opcode != 0x00 &&
		opcode != 0x4C &&
		opcode != 0x20
	) //PLP, //BRK, //JMP, //JSR (opcode will be changed within the methods)
	cpuRegistry.pc = pc + cycleTime;

}

void NES::RESET()
{
	NES::InitilizeOpCodeTable();

	cpuRegistry.sp = 0x01FF;
}

int NES::powerUpSequence()
{
	int validCartridge = 1;

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
			validCartridge = 0;
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
	for (int n = 4; n < 7; n++)
	{
	//Four lower bits of ROM Mapper Type.
	}

	for (int n = 1; n < 7; n++)
	{
		if(n >= 1 && n <= 3) //RESERVED, MUST BE 0
		{
			if (n == 2) //NEW ADDITION
			{
				NESv2Format = ((flag7 >> n) & 1) + ((flag7 >> n + 1) & 1);
			}
		}
		else if (n >= 4 && n <= 7) //Four higher bits of ROM Mapper Type.
		{

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
				validCartridge = 0;
			}
		}
	}

	for (int i = 10; i < 15; i++) //RESERVED, MUST BE 0
	{
		if (cartridge[i] != 0 && NESv2Format == 0)
		{
			validCartridge = 0;
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

	return validCartridge;
}

bool NES::loadApplication(const char * filename)
{
	RESET();
	//open file
	FILE* appFile;
	errno_t err;
	if (err = fopen_s(&appFile, filename, "rb")) //binary mode
	{
		try
		{
			char buf[4096]; //might requirefix
			strerror_s(buf, sizeof buf, err);
			fprintf_s(stderr, "cannot open file '%s': %s\n",
				filename, buf);
		}
		catch (const std::exception& ex)
		{
			//error on errormessage
		}

		return false;
	}

	if (appFile == NULL)
	{
		fputs("Failed to open file", stderr);
		return false;
	}

	//check file size
	fseek(appFile, 0, SEEK_END);	//load full file
	long lSize = ftell(appFile);	//save loaded size
	rewind(appFile);				//reset file to position 0

									// Allocate memory to contain the whole file
	char * buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL)
	{
		fputs("Memory error", stderr);
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread(buffer, 1, lSize, appFile);
	if (result != lSize)
	{
		fputs("Reading error", stderr);
		return false;
	}

	/*
	// Copy buffer to Gameboy memory
	if ((__MAX_RAM) <= lSize)
	{
	for (int i = 0; i < lSize; ++i)
	{
	memory[i + __SLOT_0] = buffer[i];
	}
	}
	else
	{
	printf("Error: ROM too big for memory");
	}*/

	if ((sizeof(cartridge)) >= lSize)
	{
		for (int i = 0; i < lSize; ++i)
		{
			cartridge[i] = buffer[i];
		}

		if (powerUpSequence() == 1)
		{
			for (int i = 0; i < sizeof(cartridge); i++)
			{
				memory[/*0x4000 **/ i] = cartridge[i];
			}

			//set pc to 0x8000 (expected)
			cpuRegistry.pc = cpuRegistry.systemBehaviour.prgLocation;
			//exec jmp first time for allocating the correct start pos (?)
			JMPI();
		}
	}
	else
	{
		printf("Error: ROM is not a cartridge");
	}

	printf("Success: ROM has loaded");

	// Close file, free buffer
	fclose(appFile);
	free(buffer);

	return true;
}

void NES::debugDisplay()
{
}

uint8_t NES::fetch()
{
	uint8_t data = memory[cpuRegistry.pc];
	cpuRegistry.pc++; //next process step
	return data;
}


uint16_t NES::fetch16()
{

	uint8_t slot0 = memory[cpuRegistry.pc];
	uint8_t slot1 = memory[cpuRegistry.pc+1];

	uint16_t data = 0x0000;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	data |= slot0;
	data |= (slot1 << 8);
#else
	data |= slot1;
	data |= (slot0 << 8);
#endif
	cpuRegistry.pc += 2; //next process step
	return data;
}

#pragma region Obsolete fetch
/*
uint16_t NES::fetch16()
{
uint16_t data = memory[cpuRegistery.pc];
data |= (memory[cpuRegistery.pc] << 8);
cpuRegistery.pc+=2; //next process step
return data;
}*/
//uint16_t NES::fetch16IX()
//{
//
//	uint8_t slot0 = memory[cpuRegistery.pc];
//	uint8_t slot1 = memory[cpuRegistery.pc + 1];
//
//	uint16_t data = 0x0000;
//
//#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
//	data |= slot0;
//	data |= (slot1 << 8);
//#else
//	data |= slot1;
//	data |= (slot0 << 8);
//#endif
//	cpuRegistery.pc += 2; //next process step
//	return data;
//}
//uint16_t NES::fetch16IY()
//{
//
//	uint8_t slot0 = memory[cpuRegistery.pc];
//	uint8_t slot1 = memory[cpuRegistery.pc + 1];
//
//	uint16_t data = 0x0000;
//
//#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
//	data |= slot0;
//	data |= (slot1 << 8);
//#else
//	data |= slot1;
//	data |= (slot0 << 8);
//#endif
//	cpuRegistery.pc += 2; //next process step
//	return data;
//}
//
//uint16_t NES::fetch16ZX()
//{
//
//	uint8_t slot0 = memory[cpuRegistery.pc];
//	uint8_t slot1 = memory[cpuRegistery.pc + 1];
//
//	uint16_t data = 0x0000;
//
//#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
//	data |= slot0;
//	data |= (slot1 << 8);
//#else
//	data |= slot1;
//	data |= (slot0 << 8);
//#endif
//	cpuRegistery.pc += 2; //next process step
//	return data;
//}
//uint16_t NES::fetch16ZY()
//{
//
//	uint8_t slot0 = memory[cpuRegistery.pc];
//	uint8_t slot1 = memory[cpuRegistery.pc + 1];
//
//	uint16_t data = 0x0000;
//
//#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
//	data |= slot0;
//	data |= (slot1 << 8);
//#else
//	data |= slot1;
//	data |= (slot0 << 8);
//#endif
//	cpuRegistery.pc += 2; //next process step
//	return data;
//}
//uint16_t NES::fetch16AX()
//{
//
//	uint8_t slot0 = memory[cpuRegistery.pc];
//	uint8_t slot1 = memory[cpuRegistery.pc + 1];
//
//	uint16_t data = 0x0000;
//
//#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
//	data |= slot0;
//	data |= (slot1 << 8);
//#else
//	data |= slot1;
//	data |= (slot0 << 8);
//#endif
//	cpuRegistery.pc += 2; //next process step
//	return data;
//}
#pragma endregion Obsolete fetch


void NES::SET_ZN(const char &value)
{
	this->SET_ZERO(value);
	this->SET_NEGATIVE(value);
}
void NES::SET_NEGATIVE(const char &value)
{
	if (value & 0x80 != 0)
	{
		this->cpuRegistry.p.N = 1;
	}
	else
	{
		this->cpuRegistry.p.N = 0;
	}
}
void NES::SET_NEGATIVE()
{
	this->cpuRegistry.p.N = 1;
}
void NES::UNSET_NEGATIVE()
{
	this->cpuRegistry.p.N = 1;
}
void NES::SET_ZERO(const char &value) 
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
//Transform custom P to byte.
uint8_t NES::P_READ()							
{
	uint8_t SR = 0x00;
	SR |= cpuRegistry.p.C;//carry					0
	SR |= cpuRegistry.p.Z << 1;//zero					1
	SR |= cpuRegistry.p.I << 2; //interupt disable		2
	SR |= cpuRegistry.p.D << 3;//decimal mode			3
	SR |= cpuRegistry.p.B << 4;	//break command			4
	SR |= cpuRegistry.p.U << 5;//unused||empty state	5
	SR |= cpuRegistry.p.V << 6;//overflow				6
	SR |= cpuRegistry.p.N << 7;//negative				7
	return SR;
}
//Transform byte to custom P.
void NES::P_WRITE(const uint8_t &M)		
{
	cpuRegistry.p.C = (M) & 1;//carry					0
	cpuRegistry.p.Z = (M >> 1) & 1;//zero					1
	cpuRegistry.p.I = (M >> 2) & 1; //interupt disable		2
	cpuRegistry.p.D = (M >> 3) & 1;//decimal mode			3
	cpuRegistry.p.B = (M >> 4) & 1;	//break command			4
	cpuRegistry.p.U = (M >> 5) & 1;//unused||empty state	5
	cpuRegistry.p.V = (M >> 6) & 1;//overflow				6
	cpuRegistry.p.N = (M >> 7) & 1;//negative				7
}

uint8_t NES::getImmeditate_u8(){ return fetch(); }
uint8_t NES::getZeroPage_u8(){
	uint8_t zpd = fetch();
	return memory[zpd];
}
uint8_t NES::getZeroPage_u8(char registry) {
	uint8_t reg;

	switch (registry)
	{
	case 'X':
		reg = cpuRegistry.registry.X;
		break;
	case 'Y':
		reg = cpuRegistry.registry.Y;
		break;
	}
	uint8_t zpd = fetch() + reg; //check x
	return memory[zpd];
}
uint16_t NES::getAbsolute_u16(){
	uint16_t ad = fetch16();
	return memory[ad];
}
uint16_t NES::getAbsolute_u16(char registry){
	uint8_t reg;
	
	switch(registry)
	{
	case 'X':
		reg = cpuRegistry.registry.X;
		break;
	case 'Y':
		reg = cpuRegistry.registry.Y;
		break;
	}

	uint16_t ad = fetch16() + reg;
	return ad;
}
uint8_t NES::getIndirectX_u8(){
	uint8_t inzp = fetch() + cpuRegistry.registry.X;
	uint8_t idl = memory[inzp];
	uint8_t idh = memory[inzp + 1];
	//TODO: little/big endian comparer (currently in little endian format)
	
	uint16_t id;
	id |= idl;
	id |= (idh << 8);
	return memory[id];
}
uint8_t NES::getIndirectY_u8(){
	uint8_t inzp = fetch();
	uint8_t idl = memory[inzp];
	uint8_t idh = memory[inzp + 1];
	//TODO: little/big endian comparer (currently in little endian format)	
	uint16_t id;
	id |= idl;
	id |= (idh << 8);
	id = id + cpuRegistry.registry.Y;
	return id;
}

uint16_t NES::getDataFromMode_u16(const AddressingMode &mode) {

	//TODO: think of better format to select between fetching address space or memory.
	/*
	the data can both be used directly and as allocation of an address space.
	the best thing to do is to alter the fetch to return the address instead of the value.
	data will be returned as a 16 bit byte. this should then be used as memory=[&r16bit].

	or data should be treated per type.

	if(immediate) {direct} else {address}
	*/

	uint16_t data = NULL;

	switch (mode)
	{
	case ZeroPage:
		data = getZeroPage_u8();
		break;
	case ZeroPageX:
		data = getZeroPage_u8('X');
		break;
	case ZeroPageY:
		data = getZeroPage_u8('Y');
		break;
	case Absolute:
		data = getAbsolute_u16();
		break;
	case AbsoluteX:
		data = getAbsolute_u16('X');
		break;
	case AbsoluteY:
		data = getAbsolute_u16('Y');
		break;
	case IndexedIndirectX:
		data = getIndirectX_u8();
		break;
	case IndirectIndexedY:
		data = getIndirectY_u8();
		break;
	default:
		getImmeditate_u8();
		break;
	}
	return data; //16 bit data should shift the low bytes into a 8 bit value.
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
	uint8_t A = 0x00;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		A = memory[getZeroPage_u8()];
		//TODO: set pc? 
		break;
	case ZeroPageX:
		cycleTime = 4;
		A = memory[getZeroPage_u8('X')];
		break;
	case Absolute:
		cycleTime = 4;
		A = memory[getAbsolute_u16()];
		break;
	case AbsoluteX:
		cycleTime = 4;
		A = memory[getAbsolute_u16('X')];
		break;
	case AbsoluteY:
		cycleTime = 4;
		A = memory[getAbsolute_u16('Y')];
		break;
	case IndexedIndirectX:
		cycleTime = 6;
		A = memory[getIndirectX_u8()];
		break;
	case IndirectIndexedY:
		cycleTime = 5;
		A = memory[getIndirectY_u8()];
		break;
	default:
		A = getImmeditate_u8();
		break;
	}

	cpuRegistry.registry.A = A;
	SET_ZN(cpuRegistry.registry.A);


	return cycleTime; 
} 
int NES::LDAI() { return LDA(Immediate); }
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
	uint8_t X = 0x00;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		X = memory[getZeroPage_u8()];
		break;
	case ZeroPageY:
		cycleTime = 4;
		X = memory[getZeroPage_u8('Y')];
		break;
	case Absolute:
		cycleTime = 4;
		X = memory[getAbsolute_u16()];
		break;
	case AbsoluteY:
		cycleTime = 4;
		X = memory[getAbsolute_u16('Y')];
		break;
	default:
		X = fetch();
		break;
	}

	cpuRegistry.registry.X = X;
	SET_ZN(cpuRegistry.registry.X);

	return cycleTime; 
}
int NES::LDXI() { return LDX(Immediate); }
int NES::LDXZP() { return LDX(ZeroPage); }
int NES::LDXZPY() { return LDX(ZeroPageY); }
int NES::LDXA() { return LDX(Absolute); }
int NES::LDXAY() { return LDX(AbsoluteY); }

//Load Y with M
int NES::LDY(const AddressingMode &mode) { 
	int cycleTime = 2;
	uint8_t Y = 0x00;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		Y = memory[getZeroPage_u8()];
		break;
	case ZeroPageX:
		cycleTime = 4;
		Y = memory[getZeroPage_u8('X')];
		break;
	case Absolute:
		cycleTime = 4;
		Y = memory[getAbsolute_u16()];
		break;
	case AbsoluteX:
		cycleTime = 4;
		Y = memory[getAbsolute_u16('X')];
		break;
	default:
		Y = fetch();
		break;
	}

	cpuRegistry.registry.Y = Y;
	SET_ZN(cpuRegistry.registry.Y);

	return cycleTime; 
} 
int NES::LDYI() { return LDY(Immediate); }
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
	uint8_t M = NULL;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		M = memory[getZeroPage_u8()];
		break;
	case ZeroPageX:
		cycleTime = 4;
		M = memory[getZeroPage_u8('X')];
		break;
	case Absolute:
		cycleTime = 4;
		M = memory[getAbsolute_u16()];
		break;
	case AbsoluteX:
		cycleTime = 4;
		M = memory[getAbsolute_u16('X')];
		break;
	case AbsoluteY:
		cycleTime = 4;
		M = memory[getAbsolute_u16('Y')];
		break;
	case IndexedIndirectX:
		cycleTime = 6;
		M = memory[getIndirectX_u8()];
		break;
	case IndirectIndexedY:
		cycleTime = 5;
		M = memory[getIndirectY_u8()];
		break;
	default:
		M = getImmeditate_u8();
		break;
	}

	cpuRegistry.registry.A = cpuRegistry.registry.A + M + cpuRegistry.p.C;
	//TODO: check how to set carry
	SET_ZN(cpuRegistry.registry.A);

	return cycleTime; 
} 
int NES::ADCI() { return ADC(Immediate); }
int NES::ADCZP() { return ADC(ZeroPage); }
int NES::ADCZPX() { return ADC(ZeroPageX); }
int NES::ADCA() { return ADC(Absolute); }
int NES::ADCAX() { return ADC(AbsoluteX); }
int NES::ADCAY() { return ADC(AbsoluteY); }
int NES::ADCIX() { return ADC(IndexedIndirectX); }
int NES::ADCIY() { return ADC(IndirectIndexedY); }

//Decrement M by One
int NES::DEC(const AddressingMode &mode) { 
	int cycleTime = 2;
	uint16_t M = NULL;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 5;
		M = getZeroPage_u8();
		break;
	case ZeroPageX:
		cycleTime = 6;
		M = getZeroPage_u8('X');
		break;
	case Absolute:
		cycleTime = 6;
		M = getAbsolute_u16();
		break;
	case AbsoluteX:
		cycleTime = 7;
		M = getAbsolute_u16('X');
		break;
	}

	subtract(M, 1);

	return cycleTime; 
} 
int NES::DECZP() { return DEC(ZeroPage); }
int NES::DECZPX() { return DEC(ZeroPageX); }
int NES::DECA() { return DEC(Absolute); }
int NES::DECAX() { return DEC(AbsoluteX); }

//Decrement X by One											--Implied
int NES::DEX() { int cycleTime = 2; cpuRegistry.registry.X -= 1; return cycleTime; }

//Decrement Y by One											--Implied
int NES::DEY() { int cycleTime = 2; cpuRegistry.registry.Y -= 1; return cycleTime; }

//Increment M by One
int NES::INC(const AddressingMode &mode) { 
	int cycleTime = 2;
	uint16_t M = NULL;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 5;
		M = getZeroPage_u8();
		break;
	case ZeroPageX:
		cycleTime = 6;
		M = getZeroPage_u8('X');
		break;
	case Absolute:
		cycleTime = 6;
		M = getAbsolute_u16();
		break;
	case AbsoluteX:
		cycleTime = 7;
		M = getAbsolute_u16('X');
		break;
	}

	add(M, 1);

	return cycleTime; 
} 
int NES::INCZP() { return INC(ZeroPage); }
int NES::INCZPX() { return INC(ZeroPageX); }
int NES::INCA() { return INC(Absolute); }
int NES::INCAX() { return INC(AbsoluteX); }

//Increment X by One											--Implied
int NES::INX() { int cycleTime = 2; cpuRegistry.registry.X += 1; return cycleTime; }

//Increment Y by One											--Implied
int NES::INY() { int cycleTime = 2; cpuRegistry.registry.Y += 1; return cycleTime; }

//Subtract M from A with Borrow
int NES::SBC(const AddressingMode &mode) { 
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
int NES::SBCI() { return SBC(Immediate); }
int NES::SBCZP() { return SBC(ZeroPage); }
int NES::SBCZPX() { return SBC(ZeroPageX); }
int NES::SBCA() { return SBC(Absolute); }
int NES::SBCAX() { return SBC(AbsoluteX); }
int NES::SBCAY() { return SBC(AbsoluteY); }
int NES::SBCIX() { return SBC(IndexedIndirectX); }
int NES::SBCIY() { return SBC(IndirectIndexedY); }

#pragma endregion Math


#pragma region Bitwise

int NES::AND(const AddressingMode &mode) { 
	int cycleTime = 2;
	uint8_t M = NULL;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		M = getZeroPage_u8();
		break;
	case ZeroPageX:
		cycleTime = 4;
		M = getZeroPage_u8('X');
		break;
	case Absolute:
		cycleTime = 4;
		M = getAbsolute_u16();
		break;
	case AbsoluteX:
		cycleTime = 4;
		M = getAbsolute_u16('X');
		break;
	case AbsoluteY:
		cycleTime = 4;
		M = getAbsolute_u16('Y');
		break;
	case IndexedIndirectX:
		cycleTime = 6;
		M = getIndirectX_u8();
		break;
	case IndirectIndexedY:
		cycleTime = 5;
		M = getIndirectY_u8();
		break;
	default:
		M = getImmeditate_u8();
		break;
	}
	cpuRegistry.registry.A = cpuRegistry.registry.A && M;
	SET_ZN(cpuRegistry.registry.A);

	return cycleTime; 
} //"AND" M with A
int NES::ANDI() { return AND(Immediate); }
int NES::ANDZP() { return AND(ZeroPage); }
int NES::ANDZPX() { return AND(ZeroPageX); }
int NES::ANDA() { return AND(Absolute); }
int NES::ANDAX() { return AND(AbsoluteX); }
int NES::ANDAY() { return AND(AbsoluteY); }
int NES::ANDIX() { return AND(IndexedIndirectX); }
int NES::ANDIY() { return AND(IndirectIndexedY); }


int NES::ASL(const AddressingMode &mode) { 
	int cycleTime = 2;
	uint16_t memoryLocation = NULL;
	if (mode == Accumulator)
	{
		cpuRegistry.p.C = ((cpuRegistry.registry.A >> 7) & 0x01);
		cpuRegistry.registry.A = (cpuRegistry.registry.A << 1);
		SET_ZN(cpuRegistry.registry.A);
	}
	else
	{
		switch (mode)
		{
		case ZeroPage:
			cycleTime = 5;
			memoryLocation = getZeroPage_u8();
			break;
		case ZeroPageX:
			cycleTime = 6;
			memoryLocation = getZeroPage_u8('X');
			break;
		case Absolute:
			cycleTime = 6;
			memoryLocation = getAbsolute_u16();
			break;
		case AbsoluteX:
			memoryLocation = getAbsolute_u16('X');
			cycleTime = 7;
			break;
		}
		 //required to be in a u16 slot
		cpuRegistry.p.C = ((memory[memoryLocation] >> 7) & 0x01);
		memory[memoryLocation] = (memory[memoryLocation] << 1);
		SET_ZN(memory[memoryLocation]);
	}

	return cycleTime; 
} //Shift Left One Bit(M or A)
int NES::ASLACC() { return ASL(Accumulator); }
int NES::ASLZP() { return ASL(ZeroPage); }
int NES::ASLZPX() { return ASL(ZeroPageX); }
int NES::ASLA() { return ASL(Absolute); }
int NES::ASLAX() { return ASL(AbsoluteX); }


int NES::BIT(const AddressingMode &mode) { 
	int cycleTime = 2;
	uint8_t M = NULL;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		break;
	case Absolute:
		cycleTime = 4;
		break;
	}

	uint8_t checksum = cpuRegistry.registry.A && M;
	(checksum >> 7) & 1 ? SET_NEGATIVE() : UNSET_NEGATIVE(); //override standard negative check.
	(checksum >> 6) & 1 ? SEV() : CLV(); //--unofficial set /w official clear.

	return cycleTime; 
} //Test Bits in M with A
int NES::BITZP() { return BIT(ZeroPage); }
int NES::BITA() { return BIT(Absolute); }


int NES::EOR(const AddressingMode &mode) { 
	int cycleTime = 2;
	uint8_t M = NULL;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		M = getZeroPage_u8();
		break;
	case ZeroPageX:
		cycleTime = 4;
		M = getZeroPage_u8('X');
		break;
	case Absolute:
		cycleTime = 4;
		M = getAbsolute_u16();
		break;
	case AbsoluteX:
		cycleTime = 4;
		M = getAbsolute_u16('X');
		break;
	case AbsoluteY:
		cycleTime = 4;
		M = getAbsolute_u16('Y');
		break;
	case IndexedIndirectX:
		cycleTime = 6;
		M = getIndirectX_u8();
		break;
	case IndirectIndexedY:
		cycleTime = 5;
		M = getIndirectY_u8();
		break;
	default:
		M = getImmeditate_u8();
		break;
	}

	cpuRegistry.registry.A ^= M;

	return cycleTime; 

} //"Exclusive-Or" M with A
int NES::EORI() { return EOR(Immediate); }
int NES::EORZP() { return EOR(ZeroPage); }
int NES::EORZPX() { return EOR(ZeroPageX); }
int NES::EORA() { return EOR(Absolute); }
int NES::EORAX() { return EOR(AbsoluteX); }
int NES::EORAY() { return EOR(AbsoluteY); }
int NES::EORIX() { return EOR(IndexedIndirectX); }
int NES::EORIY() { return EOR(IndirectIndexedY); }

int NES::LSR(const AddressingMode &mode) { 
	int cycleTime = 2; 

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 5;
		break;
	case ZeroPageX:
		cycleTime = 6;
		break;
	case Absolute:
		cycleTime = 6;
		break;
	case AbsoluteX:
		cycleTime = 7;
		break;
	case Accumulator:
		break;
	}

	return cycleTime; 
} //Shift Right One Bit(M or A)
int NES::LSRACC() { return LSR(Accumulator); }
int NES::LSRZP() { return LSR(ZeroPage); }
int NES::LSRZPX() { return LSR(ZeroPageX); }
int NES::LSRA() { return LSR(Absolute); }
int NES::LSRAX() { return LSR(AbsoluteX); }


int NES::ORA(const AddressingMode &mode) { 
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
} //"OR" M with A
int NES::ORAI() { return ORA(Immediate); }
int NES::ORAZP() { return ORA(ZeroPage); }
int NES::ORAZPX() { return ORA(ZeroPageX); }
int NES::ORAA() { return ORA(Absolute); }
int NES::ORAAX() { return ORA(AbsoluteX); }
int NES::ORAAY() { return ORA(AbsoluteY); }
int NES::ORAIX() { return ORA(IndexedIndirectX); }
int NES::ORAIY() { return ORA(IndirectIndexedY); }

int NES::ROL(const AddressingMode &mode) { 
	int cycleTime = 2; 

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 5;
		break;
	case ZeroPageX:
		cycleTime = 6;
		break;
	case Absolute:
		cycleTime = 6;
		break;
	case AbsoluteX:
		cycleTime = 7;
		break;
	case Accumulator:
		break;
	}

	return cycleTime; 
} //Rotate One Bit Left(M or A)
int NES::ROLACC() { return ROL(Accumulator); }
int NES::ROLZP() { return ROL(ZeroPage); }
int NES::ROLZPX() { return ROL(ZeroPageX); }
int NES::ROLA() { return ROL(Absolute); }
int NES::ROLAX() { return ROL(AbsoluteX); }


int NES::ROR(const AddressingMode &mode) { 
	int cycleTime = 2;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 5;
		break;
	case ZeroPageX:
		cycleTime = 6;
		break;
	case Absolute:
		cycleTime = 6;
		break;
	case AbsoluteX:
		cycleTime = 7;
		break;
	case Accumulator:
		break;
	}

	return cycleTime;
} //Rotate One Bit Right(M or A)
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
int NES::JMP(const AddressingMode &mode) { 
	int cycleTime = 2;
	uint16_t PC = cpuRegistry.pc;
	uint8_t PCl = fetch();
	uint8_t PCh = 0x00;
	uint16_t read = 0x0000;

	switch (mode)
	{
	case Absolute:
		cycleTime = 3;
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
		cycleTime = 7;
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
int NES::JMPA() { return JMP(Absolute); }
int NES::JMPI() { return JMP(Indirect);  }

int NES::JSR() {
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
int NES::RTI() { int cycleTime = 2; return cycleTime; } //Return from Interrupt										--Implied
int NES::RTS() { int cycleTime = 2; return cycleTime; } //Return from Subroutine									--Implied
#pragma endregion Jump


#pragma region Registers
int NES::CLC() { int cycleTime = 2; cpuRegistry.p.C = 0; return cycleTime; } //Clear Carry Flag											--Implied
int NES::CLD() { int cycleTime = 2; cpuRegistry.p.D = 0; return cycleTime; } //Clear Decimal Mode										--Implied
int NES::CLI() { int cycleTime = 2; cpuRegistry.p.I = 0; return cycleTime; } //Clear interrupt Disable Bit								--Implied
int NES::CLV() { int cycleTime = 2; cpuRegistry.p.V = 0; return cycleTime; } //Clear Overflow Flag										--Implied

int NES::CMP_ALL(const AddressingMode &mode, char registry)
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

	(reg >= M) ? SEC() : CLC();
	result = reg;
	result -= M; //if result = 0 then Z is 1. Z is the comparers result.
	SET_ZN(result);

	return cycleTime;
}

/*int NES::CMP(const AddressingMode &mode) { 
	int cycleTime = 2;
	uint8_t A = cpuRegistry.registry.A;
	uint8_t M = 0x00;

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



	SET_ZERO(A - M);
	SET_NEGATIVE((A == M));
	if (A >= M)
	{
		SEC();
	}
	else {
		CLC();
	}

	return cycleTime;
} //Compare M AND A*/
int NES::CMP(const AddressingMode &mode) { return CMP_ALL(mode, 'A'); }
int NES::CMPI() { return CMP(Immediate); }
int NES::CMPZP() { return CMP(ZeroPage); }
int NES::CMPZPX() { return CMP(ZeroPageX); }
int NES::CMPA() { return CMP(Absolute); }
int NES::CMPAX() { return CMP(AbsoluteX); }
int NES::CMPAY() { return CMP(AbsoluteY); }
int NES::CMPIX() { return CMP(IndexedIndirectX); }
int NES::CMPIY() { return CMP(IndirectIndexedY); }

/*int NES::CPX(const AddressingMode &mode) { 
	int cycleTime = 2;
	uint8_t X = cpuRegistry.registry.X;
	uint8_t M = 0x00;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		break;
	case Absolute:
		cycleTime = 4;
		break;
	default:
		break;
	}

	SET_ZERO(!(X == M)); //if A is equal to M set zero to 1, if(equals) true = 1 but zero == 1 = 0, this is why we need to invert the boolean state.
	SET_NEGATIVE((X == M));
	if (X >= M)
	{
		SEC();
	}
	else {
		CLC();
	}

	return cycleTime;
} //Compare M and X*/
int NES::CPX(const AddressingMode &mode) { return CMP_ALL(mode, 'X'); }
int NES::CPXI() { return CPX(Immediate); }
int NES::CPXZP() { return CPX(ZeroPage); }
int NES::CPXA() { return CPX(Absolute); }

/*int NES::CPY(const AddressingMode &mode) { 
	int cycleTime = 2;
	uint8_t Y = cpuRegistry.registry.Y;
	uint8_t M = 0x00;

	switch (mode)
	{
	case ZeroPage:
		cycleTime = 3;
		break;
	case Absolute:
		cycleTime = 4;
		break;
	default:
		break;
	}

	SET_ZERO(!(Y == M)); //if A is equal to M set zero to 1, if(equals) true = 1 but zero == 1 = 0, this is why we need to invert the boolean state.
	SET_NEGATIVE((Y == M));
	if (Y >= M)
	{
		SEC();
	}
	else {
		CLC();
	}

	return cycleTime;
} //Compare M and Y*/
int NES::CPY(const AddressingMode &mode) { return CMP_ALL(mode, 'Y'); }
int NES::CPYI() { return CPY(Immediate); }
int NES::CPYZP() { return CPY(ZeroPage); }
int NES::CPYA() { return CPY(Absolute); }

int NES::SEC() { int cycleTime = 2; cpuRegistry.p.C = 1; return cycleTime; } //Set Carry Flag											--Implied
int NES::SED() { int cycleTime = 2; cpuRegistry.p.D = 1; return cycleTime; } //Set Decimal Mode											--Implied
int NES::SEI() { int cycleTime = 2; cpuRegistry.p.I = 1; return cycleTime; } //Set Interrupt Disable Status								--Implied
int NES::SEV() { cpuRegistry.p.V = 1; return 0; } //Unofficial OP
#pragma endregion Registers


#pragma region Stack
int NES::PHA() { 
	int cycleTime = 3;

	cpuRegistry.sp = cpuRegistry.registry.A;

	return cycleTime; 
} //Push A on Stack											--Implied
int NES::PHP() { 
	int cycleTime = 3; 

	cpuRegistry.sp = cpuRegistry.pc;

	return cycleTime; 
} //Push Processor Status on Stack							--Implied
int NES::PLA() { 
	int cycleTime = 4; 

	cpuRegistry.registry.A = cpuRegistry.sp;

	return cycleTime; 
} //Pull A from Stack											--Implied
int NES::PLP() { 
	int cycleTime = 4; 

	cpuRegistry.pc = cpuRegistry.sp;

	return cycleTime; 
} //Pull Processor Status from Stack							--Implied
#pragma endregion Stack


#pragma region Interrupts
int NES::NOP() { return 2; }		//No Operation											--Implied
int NES::NMI() { return 0; }		//Non-Maskable Intteruption
int NES::Reset() { return 0; }	//Reset
int NES::IRQ() { return 0; }		//Interrupt Request
int NES::BRK() { return 7; }		//Break													--Implied
#pragma endregion
#pragma endregion OPCODE Methods
