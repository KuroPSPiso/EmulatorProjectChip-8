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
		if (n >= 1 && n <= 3) //RESERVED, MUST BE 0
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
				operable = 0;
			}
		}
	}

	for (int i = 10; i < 15; i++) //RESERVED, MUST BE 0
	{
		if (cartridge[i] != 0 && NESv2Format == 0)
		{
			operable = 0;
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
	}
}

void CPU::RESET()
{
	operable = TRUE;
	cpuRegistry.sp = 0x01FF; //Default start position of stackpointer
	cpuRegistry.pc = 0x0000; //
	cpuRegistry.registry.A = NULL;
	cpuRegistry.registry.X = NULL;
	cpuRegistry.registry.Y = NULL;
	opcode = NULL;

	Boot();
}

void CPU::__MemADD(const uint16_t & address, const uint8_t value)
{
	cartridge[address] += value;
}

void CPU::__MemSUB(const uint16_t & address, const uint8_t value)
{
	cartridge[address] -= value;
}

void CPU::__MemWrite(const uint16_t & address, const uint8_t value)
{
	cartridge[address] = value;
}

const uint8_t CPU::__MemRead(const uint16_t & address)
{
	return cartridge[address];
}

const uint16_t CPU::__MemRead16(const uint16_t & address)
{
	uint8_t slot0 = cartridge[address];
	uint8_t slot1 = cartridge[address + 1];
	uint16_t data = 0x0000;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	data |= slot0;
	data |= (slot1 << 8);
#else	
	data |= slot1;
	data |= (slot0 << 8);
#endif

	return cartridge[address];
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
}

CPU::CPU(CartridgeLDR* cartridgeSlot)
{
	cartridgeSlot->LoadIntoCartridgeSlot(cartridge);
	RESET();
}