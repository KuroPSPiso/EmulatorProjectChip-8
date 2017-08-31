#include "gameboy.h"
#include "string.h"

#define __MAX_RAM 0x7FFF
#define __SLOT_0 0x0000
#define __SLOT_1 0x4000


void Gameboy::write(const uint16_t &address, const uint8_t value)
{
	memory[address] = value;
}
void Gameboy::add(const uint16_t &address, const uint8_t value)
{
	memory[address] += value;
}
void Gameboy::subtract(const uint16_t &address, const uint8_t value)
{
	memory[address] -= value;
}

void Gameboy::SetFlag(uint8_t flag)
{
	cpuRegistery.registery.F |= flag;
}
void Gameboy::ResetFlag(uint8_t flag)
{
	cpuRegistery.registery.F &= ~flag;
}

void Gameboy::ADD_SetFlags(uint8_t result)
{
	//TODO: check if solution works.
	if (result = 0x00) { SetFlag(flag_Z); }
	if (result = 0x03) { SetFlag(flag_H); }
	if (result = 0x07) { SetFlag(flag_C); }
	ResetFlag(flag_N);
}
void Gameboy::SUB_SetFlags(uint8_t result)
{
	//TODO: check if solution works.
	if (result < 0) { SetFlag(flag_C); }
	if (result = 0x00) { SetFlag(flag_Z); }
	if (result != 0x04) { SetFlag(flag_H); }
	SetFlag(flag_N);
}
void Gameboy::AND_SetFlags(uint8_t result)
{
	//TODO: check if solution works.
	if (result = 0x00) { SetFlag(flag_Z); }
	SetFlag(flag_H);
	ResetFlag(flag_N);
	ResetFlag(flag_C);
}

const uint8_t& Gameboy::read(const uint16_t &address)
{
	return memory[address];
}

void Gameboy::emulateCycle()
{
	opcode = fetch();
	decodeOPCode(opcode);
}

void Gameboy::RESET()
{
	Gameboy::InitilizeOpCodeTable();
}

int Gameboy::powerUpSequence()
{
	int validCartridge = 1;

	unsigned char comparerGraphic[] = {
		0xCE, 0xED , 0x66 , 0x66 , 0xCC , 0x0D , 0x00 , 0x0B , 0x03 , 0x73 , 0x00 , 0x83 , 0x00 , 0x0C , 0x00 , 0x0D,
		0x00 , 0x08 , 0x11 , 0x1F , 0x88 , 0x89 , 0x00 , 0x0E , 0xDC , 0xCC , 0x6E , 0xE6 , 0xDD , 0xDD , 0xD9 , 0x99,
		0xBB , 0xBB , 0x67 , 0x63 , 0x6E , 0x0E , 0xEC , 0xCC , 0xDD , 0xDC , 0x99 , 0x9F , 0xBB , 0xB9 , 0x33 , 0x3E
	};

	printf("GRAPHIC-CHECK::");

	for (int i = 0; i <= 0x133 - 0x104; i++)
	{
		std::string data_str;
		char val[8];
		snprintf(val, sizeof(val), "0x%X-", cartridge[i + 0x104]);
		data_str.append(val);
		const char* data = data_str.c_str();
		printf(data);

		if (comparerGraphic[i] != cartridge[i + 0x104])
		{
			validCartridge = 0;
		}
	}

	printf("|NAME::");
	for (int i = 0; i <= 0x142 - 0x134; i++)
	{
		std::string data_str = "";
		data_str.append(1, cartridge[i + 0x134]);
		const char* data = data_str.c_str();
		printf(data);
	}

	return validCartridge;
}

bool Gameboy::loadApplication(const char * filename)
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
				memory[i] = cartridge[i];
			}
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

void Gameboy::debugDisplay()
{
}

uint8_t Gameboy::fetch()
{
	uint8_t data = memory[cpuRegistery.pc];
	cpuRegistery.pc++; //next process step
	return data;
}

#pragma region 8-bit Loads
/*0x00*/
int Gameboy::NOP()
{
	//No Operation
	printf("NOP::");
	return 4;
}

/*0x7F*/
int Gameboy::LD_A_A()
{
	printf("LD_A_A::");
	cpuRegistery.registery.A = cpuRegistery.registery.A;
	return 4;
}
/*0X78*/
int Gameboy::LD_A_B()
{
	printf("LD_A_B::");
	cpuRegistery.registery.A = cpuRegistery.registery.B;
	return 4;
}
/*0X79*/
int Gameboy::LD_A_C()
{
	printf("LD_A_C::");
	cpuRegistery.registery.A = cpuRegistery.registery.C;
	return 4;
}
/*0X7A*/
int Gameboy::LD_A_D()
{
	printf("LD_A_D::");
	cpuRegistery.registery.A = cpuRegistery.registery.D;
	return 4;
}
/*0X7B*/
int Gameboy::LD_A_E()
{
	printf("LD_A_E::");
	cpuRegistery.registery.A = cpuRegistery.registery.E;
	return 4;
}
/*0X7C*/
int Gameboy::LD_A_H()
{
	printf("LD_A_H::");
	cpuRegistery.registery.A = cpuRegistery.registery.H;
	return 4;
}
/*0X7D*/
int Gameboy::LD_A_L()
{
	printf("LD_A_L::");
	cpuRegistery.registery.A = cpuRegistery.registery.L;
	return 4;
}

/*0x47*/
int Gameboy::LD_B_A()
{
	printf("LD_B_A::");
	cpuRegistery.registery.B = cpuRegistery.registery.A;
	return 4;
}
/*0x40*/
int Gameboy::LD_B_B()
{
	printf("LD_B_B::");
	cpuRegistery.registery.B = cpuRegistery.registery.B;
	return 4;
}
/*0x41*/
int Gameboy::LD_B_C()
{
	printf("LD_B_C::");
	cpuRegistery.registery.B = cpuRegistery.registery.C;
	return 4;
}
/*0x42*/
int Gameboy::LD_B_D()
{
	printf("LD_B_D::");
	cpuRegistery.registery.B = cpuRegistery.registery.D;
	return 4;
}
/*0x43*/
int Gameboy::LD_B_E()
{
	printf("LD_B_E::");
	cpuRegistery.registery.B = cpuRegistery.registery.E;
	return 4;
}
/*0x44*/
int Gameboy::LD_B_H()
{
	printf("LD_B_H::");
	cpuRegistery.registery.B = cpuRegistery.registery.H;
	return 4;
}
/*0x45*/
int Gameboy::LD_B_L()
{
	printf("LD_B_L::");
	cpuRegistery.registery.B = cpuRegistery.registery.L;
	return 4;
}

/*0x4F*/
int Gameboy::LD_C_A()
{
	printf("LD_C_A::");
	cpuRegistery.registery.C = cpuRegistery.registery.A;
	return 4;
}
/*0X48*/
int Gameboy::LD_C_B()
{
	printf("LD_C_B::");
	cpuRegistery.registery.C = cpuRegistery.registery.B;
	return 4;
}
/*0X49*/
int Gameboy::LD_C_C()
{
	printf("LD_C_C::");
	cpuRegistery.registery.C = cpuRegistery.registery.C;
	return 4;
}
/*0X4A*/
int Gameboy::LD_C_D()
{
	printf("LD_C_D::");
	cpuRegistery.registery.C = cpuRegistery.registery.D;
	return 4;
}
/*0X4B*/
int Gameboy::LD_C_E()
{
	printf("LD_C_E::");
	cpuRegistery.registery.C = cpuRegistery.registery.E;
	return 4;
}
/*0X4C*/
int Gameboy::LD_C_H()
{
	printf("LD_C_H::");
	cpuRegistery.registery.C = cpuRegistery.registery.H;
	return 4;
}
/*0X4D*/
int Gameboy::LD_C_L()
{
	printf("LD_C_L::");
	cpuRegistery.registery.C = cpuRegistery.registery.L;
	return 4;
}

/*0x57*/
int Gameboy::LD_D_A()
{
	printf("LD_D_A::");
	cpuRegistery.registery.D = cpuRegistery.registery.A;
	return 4;
}
/*0x50*/
int Gameboy::LD_D_B()
{
	printf("LD_D_B::");
	cpuRegistery.registery.D = cpuRegistery.registery.B;
	return 4;
}
/*0x51*/
int Gameboy::LD_D_C()
{
	printf("LD_D_C::");
	cpuRegistery.registery.D = cpuRegistery.registery.C;
	return 4;
}
/*0x52*/
int Gameboy::LD_D_D()
{
	printf("LD_D_D::");
	cpuRegistery.registery.D = cpuRegistery.registery.D;
	return 4;
}
/*0x53*/
int Gameboy::LD_D_E()
{
	printf("LD_D_E::");
	cpuRegistery.registery.D = cpuRegistery.registery.E;
	return 4;
}
/*0x54*/
int Gameboy::LD_D_H()
{
	printf("LD_D_H::");
	cpuRegistery.registery.D = cpuRegistery.registery.H;
	return 4;
}
/*0x55*/
int Gameboy::LD_D_L()
{
	printf("LD_D_L::");
	cpuRegistery.registery.D = cpuRegistery.registery.L;
	return 4;
}

/*0x5F*/
int Gameboy::LD_E_A()
{
	printf("LD_E_A::");
	cpuRegistery.registery.E = cpuRegistery.registery.A;
	return 4;
}
/*0X58*/
int Gameboy::LD_E_B()
{
	printf("LD_E_B::");
	cpuRegistery.registery.E = cpuRegistery.registery.B;
	return 4;
}
/*0X59*/
int Gameboy::LD_E_C()
{
	printf("LD_E_C::");
	cpuRegistery.registery.E = cpuRegistery.registery.C;
	return 4;
}
/*0X5A*/
int Gameboy::LD_E_D()
{
	printf("LD_E_D::");
	cpuRegistery.registery.E = cpuRegistery.registery.D;
	return 4;
}
/*0X5B*/
int Gameboy::LD_E_E()
{
	printf("LD_E_E::");
	cpuRegistery.registery.E = cpuRegistery.registery.E;
	return 4;
}
/*0X5C*/
int Gameboy::LD_E_H()
{
	printf("LD_E_H::");
	cpuRegistery.registery.E = cpuRegistery.registery.H;
	return 4;
}
/*0X5D*/
int Gameboy::LD_E_L()
{
	printf("LD_E_L::");
	cpuRegistery.registery.E = cpuRegistery.registery.L;
	return 4;
}

/*0x67*/
int Gameboy::LD_H_A()
{
	printf("LD_H_A::");
	cpuRegistery.registery.H = cpuRegistery.registery.A;
	return 4;
}
/*0x60*/
int Gameboy::LD_H_B()
{
	printf("LD_H_B::");
	cpuRegistery.registery.H = cpuRegistery.registery.B;
	return 4;
}
/*0x61*/
int Gameboy::LD_H_C()
{
	printf("LD_H_C::");
	cpuRegistery.registery.H = cpuRegistery.registery.C;
	return 4;
}
/*0x62*/
int Gameboy::LD_H_D()
{
	printf("LD_H_D::");
	cpuRegistery.registery.H = cpuRegistery.registery.D;
	return 4;
}
/*0x63*/
int Gameboy::LD_H_E()
{
	printf("LD_H_E::");
	cpuRegistery.registery.H = cpuRegistery.registery.E;
	return 4;
}
/*0x64*/
int Gameboy::LD_H_H()
{
	printf("LD_H_H::");
	cpuRegistery.registery.H = cpuRegistery.registery.H;
	return 4;
}
/*0x65*/
int Gameboy::LD_H_L()
{
	printf("LD_H_L::");
	cpuRegistery.registery.H = cpuRegistery.registery.L;
	return 4;
}

/*0x6F*/
int Gameboy::LD_L_A()
{
	printf("LD_L_A::");
	cpuRegistery.registery.L = cpuRegistery.registery.A;
	return 4;
}
/*0X68*/
int Gameboy::LD_L_B()
{
	printf("LD_L_B::");
	cpuRegistery.registery.L = cpuRegistery.registery.B;
	return 4;
}
/*0X69*/
int Gameboy::LD_L_C()
{
	printf("LD_L_C::");
	cpuRegistery.registery.L = cpuRegistery.registery.C;
	return 4;
}
/*0X6A*/
int Gameboy::LD_L_D()
{
	printf("LD_L_D::");
	cpuRegistery.registery.L = cpuRegistery.registery.D;
	return 4;
}
/*0X6B*/
int Gameboy::LD_L_E()
{
	printf("LD_L_E::");
	cpuRegistery.registery.L = cpuRegistery.registery.E;
	return 4;
}
/*0X6C*/
int Gameboy::LD_L_H()
{
	printf("LD_L_H::");
	cpuRegistery.registery.L = cpuRegistery.registery.H;
	return 4;
}
/*0X6D*/
int Gameboy::LD_L_L()
{
	printf("LD_L_L::");
	cpuRegistery.registery.L = cpuRegistery.registery.L;
	return 4;
}
/*0x3E*/
int Gameboy::LD_A_n()
{
	printf("LD_A_n::");
	uint8_t n = fetch();
	cpuRegistery.registery.A = n;
	return 8;
}
/*0x06*/
int Gameboy::LD_B_n()
{
	printf("LD_B_n::");
	uint8_t n = fetch();
	cpuRegistery.registery.B = n;
	return 8;
}
/*0x0E*/
int Gameboy::LD_C_n()
{
	printf("LD_C_n::");
	uint8_t n = fetch();
	cpuRegistery.registery.C = n;
	return 8;
}
/*0x16*/
int Gameboy::LD_D_n()
{
	printf("LD_D_n::");
	uint8_t n = fetch();
	cpuRegistery.registery.D = n;
	return 8;
}
/*0x1E*/
int Gameboy::LD_E_n()
{
	printf("LD_E_n::");
	uint8_t n = fetch();
	cpuRegistery.registery.E = n;
	return 8;
}
/*0x26*/
int Gameboy::LD_H_n()
{
	printf("LD_H_n::");
	uint8_t n = fetch();
	cpuRegistery.registery.H = n;
	return 8;
}
/*0x2E*/
int Gameboy::LD_L_n()
{
	printf("LD_L_n::");
	uint8_t n = fetch();
	cpuRegistery.registery.L = n;
	return 8;
}
/*0x7E*/
int Gameboy::LD_A_HL()
{
	printf("LD_A_HL::");
	uint8_t readVal = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.A = readVal;
	return 8;
}
/*0x46*/
int Gameboy::LD_B_HL()
{
	printf("LD_B_HL::");
	uint8_t readVal = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.B = readVal;
	return 8;
}
/*0x4E*/
int Gameboy::LD_C_HL()
{
	printf("LD_C_HL::");
	uint8_t readVal = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.C = readVal;
	return 8;
}
/*0x56*/
int Gameboy::LD_D_HL()
{
	printf("LD_D_HL::");
	uint8_t readVal = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.D = readVal;
	return 8;
}
/*0x5E*/
int Gameboy::LD_E_HL()
{
	printf("LD_E_HL::");
	uint8_t readVal = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.E = readVal;
	return 8;
}
/*0x66*/
int Gameboy::LD_H_HL()
{
	printf("LD_H_HL::");
	uint8_t readVal = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.H = readVal;
	return 8;
}
/*0x6E*/
int Gameboy::LD_L_HL()
{
	printf("LD_L_HL::");
	uint8_t readVal = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.L = readVal;
	return 8;
}
/*0x77*/
int Gameboy::LD_HL_A()
{
	printf("LD_HL_A::");
	write(cpuRegistery.registery.HL, cpuRegistery.registery.A);
	return 8;
}
/*0x70*/
int Gameboy::LD_HL_B()
{
	printf("LD_HL_B::");
	write(cpuRegistery.registery.HL, cpuRegistery.registery.B);
	return 8;
}
/*0x71*/
int Gameboy::LD_HL_C()
{
	printf("LD_HL_C::");
	write(cpuRegistery.registery.HL, cpuRegistery.registery.C);
	return 8;
}
/*0x72*/
int Gameboy::LD_HL_D()
{
	printf("LD_HL_D::");
	write(cpuRegistery.registery.HL, cpuRegistery.registery.D);
	return 8;
}
/*0x73*/
int Gameboy::LD_HL_E()
{
	printf("LD_HL_E::");
	write(cpuRegistery.registery.HL, cpuRegistery.registery.E);
	return 8;
}
/*0x74*/
int Gameboy::LD_HL_H()
{
	printf("LD_HL_H::");
	write(cpuRegistery.registery.HL, cpuRegistery.registery.H);
	return 8;
}
/*0x75*/
int Gameboy::LD_HL_L()
{
	printf("LD_HL_L::");
	write(cpuRegistery.registery.HL, cpuRegistery.registery.L);
	return 8;
}
/*0x36*/
int Gameboy::LD_HL_n()
{
	printf("LD_HL_n::");
	uint8_t n = fetch();
	write(cpuRegistery.registery.HL, n);
	return 12;
}
/*0xEA*/
int Gameboy::LD_nn_A()
{
	printf("LD_nn_A::");
	//TODO: check result of n;

	uint8_t n = read(fetch());
	write(n, cpuRegistery.registery.A);
	return 16;
}
/*0x02*/
int Gameboy::LD_BC_A()
{
	printf("LD_BC_A::");
	cpuRegistery.registery.BC = cpuRegistery.registery.A;
	return 8;
}
/*0x12*/
int Gameboy::LD_DE_A()
{
	printf("LD_DE_A::");
	cpuRegistery.registery.DE = cpuRegistery.registery.A;
	return 8;
}
/*0x0A*/
int Gameboy::LD_A_BC()
{
	printf("LD_A_BC::");
	uint8_t readVal = read(cpuRegistery.registery.BC);
	cpuRegistery.registery.E = readVal;
	return 8;
}
/*0x1A*/
int Gameboy::LD_A_DE()
{
	printf("LD_A_DE::");
	uint8_t readVal = read(cpuRegistery.registery.DE);
	cpuRegistery.registery.H = readVal;
	return 8;
}
/*0xFA*/
int Gameboy::LD_A_nn()
{
	printf("LD_A_nn::");
	uint8_t n = read(fetch());
	cpuRegistery.registery.A = n;
	return 16;
}
/*0xF9*/
int Gameboy::LD_SP_HL()
{
	printf("LD_SP_HL::");
	cpuRegistery.sp = cpuRegistery.registery.HL;
	return 8;
}


#pragma region Custom OpCodes
/*0xF2*/
int Gameboy::LD_A_FF00_PLUS_C()
{
	printf("LD_A_FF00_PLUS_C::");
	cpuRegistery.registery.A = read(0xFF00 + cpuRegistery.registery.C); //read due to uint16_t to uint8_t
	return 8;
}
/*0xE2*/
int Gameboy::LD_FF00_PLUS_C_A()
{
	printf("LD_FF00_PLUS_C_A::");
	write(0xFF00 + cpuRegistery.registery.C, cpuRegistery.registery.A);
	return 8;
}

/*0x3A*/
int Gameboy::LDD_A_HL()
{
	printf("LDD_A_HL::");
	cpuRegistery.registery.A = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.HL -= 0x01;
	return 8;
}
/*0x32*/
int Gameboy::LDD_HL_A()
{
	printf("LDD_HL_A::");
	write(cpuRegistery.registery.HL, cpuRegistery.registery.A);
	cpuRegistery.registery.HL -= 0x01;
	return 8;
}
/*0x2A*/
int Gameboy::LDI_A_HL()
{
	printf("LDI_A_HL::");
	cpuRegistery.registery.A = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.HL += 0x01;
	return 8;
}
/*0x22*/
int Gameboy::LDI_HL_A()
{
	printf("LDI_HL_A::");
	write(cpuRegistery.registery.HL, cpuRegistery.registery.A);
	cpuRegistery.registery.HL += 0x01;
	return 8;
}
/*0xE0*/
int Gameboy::LDH_FF00_PLUS_n_A()
{
	printf("LDH_FF00_PLUS_n_A::");
	uint8_t n = fetch();
	write(0xFF00 + n, cpuRegistery.registery.A);
	return 12;
}
/*0xF0*/
int Gameboy::LDH_A_FF00_PLUS_n()
{
	printf("LDH_A_FF00_PLUS_n::");
	uint8_t n = fetch();
	cpuRegistery.registery.A = read(0xFF00 + n);
	return 12;
}
#pragma endregion Custom OpCodes

#pragma region deleted OpCodes
//INDEX
/*0xED*/
int Gameboy::LD_I()
{
	//Index is not implemented in Gameboys Z80 like cpu.
	return 0; //default 20
}
/*0xDD*/
int Gameboy::LD_r_IX_PLUS_d()
{
	uint8_t subOpCode = fetch();
	return decodeOPCodeI__d(subOpCode, cpuRegistery.index.IX);
}
/*0xFD*/
int Gameboy::LD_r_IY_PLUS_d()
{
	uint8_t subOpCode = fetch();
	return decodeOPCodeI__d(subOpCode, cpuRegistery.index.IY);
}
#pragma endregion deleted OpCodes

#pragma endregion 8-bit Loads

#pragma region 16-bit Loads
/*0x01*/
int Gameboy::LD_BC_nn()
{
	//due to littleendian first
	//cpuRegistery.registery.C = fetch();
	//cpuRegistery.registery.B = fetch();
	uint8_t n = fetch();
	cpuRegistery.registery.C = n;
	n = fetch();
	cpuRegistery.registery.B = n;
	return 12;
}
/*0x11*/
int Gameboy::LD_DE_nn()
{
	uint8_t n = fetch();
	cpuRegistery.registery.E = n;
	n = fetch();
	cpuRegistery.registery.D = n;
	return 12;
}
/*0x21*/
int Gameboy::LD_HL_nn()
{
	//uint8_t n = read(fetch());
	//cpuRegistery.registery.H = n + 1;
	//cpuRegistery.registery.L = n;
	uint8_t n = fetch();
	cpuRegistery.registery.L = n;
	n = fetch();
	cpuRegistery.registery.H = n;
	return 12;
}
/*0x31*/
int Gameboy::LD_SP_nn()
{
	uint8_t n = fetch();
	uint8_t n1 = fetch();
	cpuRegistery.sp = (n >> 8) + n1; //TODO: check sp result
	return 12;
}
/*0xF8*/
int Gameboy::LD_HL_SP_PLUS_n()
{
	uint8_t n = fetch();
	add(cpuRegistery.sp, n);
	cpuRegistery.registery.HL = cpuRegistery.sp;
	subtract(cpuRegistery.sp, n);
	return 12;
}
/*0x08*/
int Gameboy::LD_nn_SP()
{
	uint8_t n = fetch();
	n = read(cpuRegistery.sp);
	n = fetch();
	n = read(cpuRegistery.sp);
	return 20;
}
/*0xF5*/
int Gameboy::PUSH_AF()
{
	cpuRegistery.sp = cpuRegistery.registery.AF;
	cpuRegistery.sp -= 2;
	return 16;
}
/*0xC5*/
int Gameboy::PUSH_BC()
{
	cpuRegistery.sp = cpuRegistery.registery.BC;
	cpuRegistery.sp -= 2;
	return 16;
}
/*0xD5*/
int Gameboy::PUSH_DE()
{
	cpuRegistery.sp = cpuRegistery.registery.DE;
	cpuRegistery.sp -= 2;
	return 16;
}
/*0xE5*/
int Gameboy::PUSH_HL()
{
	cpuRegistery.sp = cpuRegistery.registery.HL;
	cpuRegistery.sp -= 2;
	return 16;
}
/*0xF1*/
int Gameboy::POP_AF()
{
	uint8_t n = fetch();
	n = cpuRegistery.registery.F;
	n = fetch();
	n = cpuRegistery.registery.A;
	cpuRegistery.sp += 2;
	return 12;
}
/*0xC1*/
int Gameboy::POP_BC()
{
	uint8_t n = fetch();
	n = cpuRegistery.registery.C;
	n = fetch();
	n = cpuRegistery.registery.B;
	cpuRegistery.sp += 2;
	return 12;
}
/*0xD1*/
int Gameboy::POP_DE()
{
	uint8_t n = fetch();
	n = cpuRegistery.registery.E;
	n = fetch();
	n = cpuRegistery.registery.D;
	cpuRegistery.sp += 2;
	return 12;
}
/*0xE1*/
int Gameboy::POP_HL()
{
	uint8_t n = fetch();
	n = cpuRegistery.registery.L;
	n = fetch();
	n = cpuRegistery.registery.H;
	cpuRegistery.sp += 2;
	return 12;
}
#pragma endregion 16-bit Loads

#pragma region 8 bit ALU
/*0x87*/
int Gameboy::ADD_A_A()
{
	uint8_t s = cpuRegistery.registery.A;
	cpuRegistery.registery.A += s;
	ADD_SetFlags(s);
	return 4;
}
/*0x80*/
int Gameboy::ADD_A_B()
{
	uint8_t s = cpuRegistery.registery.B;
	cpuRegistery.registery.A += s;
	ADD_SetFlags(cpuRegistery.registery.A);
	return 4;
}
/*0x81*/
int Gameboy::ADD_A_C()
{
	uint8_t s = cpuRegistery.registery.C;
	cpuRegistery.registery.A += s;
	ADD_SetFlags(cpuRegistery.registery.A);
	return 4;
}
/*0x82*/
int Gameboy::ADD_A_D()
{
	uint8_t s = cpuRegistery.registery.D;
	cpuRegistery.registery.A += s;
	ADD_SetFlags(cpuRegistery.registery.A);
	return 4;
}
/*0x83*/
int Gameboy::ADD_A_E()
{
	uint8_t s = cpuRegistery.registery.E;
	cpuRegistery.registery.A += s;
	ADD_SetFlags(cpuRegistery.registery.A);
	return 4;
}
/*0x84*/
int Gameboy::ADD_A_H()
{
	uint8_t s = cpuRegistery.registery.H;
	cpuRegistery.registery.A += s;
	ADD_SetFlags(cpuRegistery.registery.A);
	return 4;
}
/*0x85*/
int Gameboy::ADD_A_L()
{
	uint8_t s = cpuRegistery.registery.L;
	cpuRegistery.registery.A += s;
	ADD_SetFlags(cpuRegistery.registery.A);
	return 4;
}
/*0x86*/
int Gameboy::ADD_A_HL()
{
	uint8_t s = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.A += s;
	ADD_SetFlags(cpuRegistery.registery.A);
	return 8;
}
/*0xC6*/
int Gameboy::ADD_A_n()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A += n;
	ADD_SetFlags(n);
	return 8;
}
/*0x8F*/
int Gameboy::ADC_A_A()
{
	uint8_t s = cpuRegistery.registery.A;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	ADD_SetFlags(s);
	return 4;
}
/*0x88*/
int Gameboy::ADC_A_B()
{
	uint8_t s = cpuRegistery.registery.B;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	ADD_SetFlags(s);
	return 4;
}
/*0x89*/
int Gameboy::ADC_A_C()
{
	uint8_t s = cpuRegistery.registery.C;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	ADD_SetFlags(s);
	return 4;
}
/*0x8A*/
int Gameboy::ADC_A_D()
{
	uint8_t s = cpuRegistery.registery.D;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	ADD_SetFlags(s);
	return 4;
}
/*0x8B*/
int Gameboy::ADC_A_E()
{
	uint8_t s = cpuRegistery.registery.E;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	ADD_SetFlags(s);
	return 4;
}
/*0x8C*/
int Gameboy::ADC_A_H()
{
	uint8_t s = cpuRegistery.registery.H;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	ADD_SetFlags(s);
	return 4;
}
/*0x8D*/
int Gameboy::ADC_A_L()
{
	uint8_t s = cpuRegistery.registery.L;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	ADD_SetFlags(s);
	return 4;
}
/*0x8E*/
int Gameboy::ADC_A_HL()
{
	uint8_t s = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	ADD_SetFlags(s);
	return 8;
}
/*0xCE*/
int Gameboy::ADC_A_n()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A += n + cpuRegistery.registery.F;
	ADD_SetFlags(n);
	return 8;
}
/*0x97*/
int Gameboy::SUB_A_A()
{
	uint8_t s = cpuRegistery.registery.A;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x90*/
int Gameboy::SUB_A_B()
{
	uint8_t s = cpuRegistery.registery.B;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x91*/
int Gameboy::SUB_A_C()
{
	uint8_t s = cpuRegistery.registery.C;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x92*/
int Gameboy::SUB_A_D()
{
	uint8_t s = cpuRegistery.registery.D;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x93*/
int Gameboy::SUB_A_E()
{
	uint8_t s = cpuRegistery.registery.E;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x94*/
int Gameboy::SUB_A_H()
{
	uint8_t s = cpuRegistery.registery.H;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x95*/
int Gameboy::SUB_A_L()
{
	uint8_t s = cpuRegistery.registery.L;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x96*/
int Gameboy::SUB_A_HL()
{
	uint8_t s = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 8;
}
/*0xD6*/
int Gameboy::SUB_A_n()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A += n + cpuRegistery.registery.F;
	SUB_SetFlags(n);
	return 8;
}
/*0x9F*/
int Gameboy::SBC_A_A()
{
	uint8_t s = cpuRegistery.registery.A;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x98*/
int Gameboy::SBC_A_B()
{
	uint8_t s = cpuRegistery.registery.B;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x99*/
int Gameboy::SBC_A_C()
{
	uint8_t s = cpuRegistery.registery.C;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x9A*/
int Gameboy::SBC_A_D()
{
	uint8_t s = cpuRegistery.registery.D;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x9B*/
int Gameboy::SBC_A_E()
{
	uint8_t s = cpuRegistery.registery.E;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x9C*/
int Gameboy::SBC_A_H()
{
	uint8_t s = cpuRegistery.registery.H;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x9D*/
int Gameboy::SCB_A_L()
{
	uint8_t s = cpuRegistery.registery.L;
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 4;
}
/*0x9E*/
int Gameboy::SCB_A_HL()
{
	uint8_t s = read(cpuRegistery.registery.HL);
	cpuRegistery.registery.A += s + cpuRegistery.registery.F;
	SUB_SetFlags(s);
	return 8;
}
/*0xDE*/
int Gameboy::SCB_A_n()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A += n + cpuRegistery.registery.F;
	SUB_SetFlags(n);
	return 8;
}

int Gameboy::AND_A_A()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A = n & cpuRegistery.registery.A;
	AND_SetFlags(cpuRegistery.registery.A);
	return 4;
}

int Gameboy::AND_A_B()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A = n & cpuRegistery.registery.A;
	AND_SetFlags(cpuRegistery.registery.A);
	return 4;
}

int Gameboy::AND_A_C()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A = n & cpuRegistery.registery.A;
	AND_SetFlags(cpuRegistery.registery.A);
	return 4;
}

int Gameboy::AND_A_D()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A = n & cpuRegistery.registery.A;
	AND_SetFlags(cpuRegistery.registery.A);
	return 4;
}

int Gameboy::AND_A_E()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A = n & cpuRegistery.registery.A;
	AND_SetFlags(cpuRegistery.registery.A);
	return 4;
}

int Gameboy::AND_A_H()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A = n & cpuRegistery.registery.A;
	AND_SetFlags(cpuRegistery.registery.A);
	return 4;
}

int Gameboy::AND_A_L()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A = n & cpuRegistery.registery.A;
	AND_SetFlags(cpuRegistery.registery.A);
	return 4;
}

int Gameboy::AND_A_HL()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A = n & cpuRegistery.registery.A;
	AND_SetFlags(cpuRegistery.registery.A);
	return 4;
}

int Gameboy::AND_A_n()
{
	uint8_t n = fetch();
	cpuRegistery.registery.A = n;
	AND_SetFlags(cpuRegistery.registery.A);
	return 4;
}

#pragma endregion 8 bit ALU

uint8_t Gameboy::decodeOPCode(const uint8_t &opcode)
{
	int tickStates = 0; 
	uint8_t n, readVal, subOpCode = NULL;
	
	try
	{
		tickStates = (this->*(gbOpCodeTable[opcode]))();
	}
	catch (std::exception &e)
	{
		printf(e.what());
	}

	return tickStates;
}

uint8_t Gameboy::decodeOPCodeI__d(const uint8_t &opcode, uint16_t index)
{
	//NO NEED FOR OPTIMILIZATION, does not / should not be used. (TODO: remove if interrupting with performance)

	uint8_t tickStates = 0;
	uint8_t n, d, readVal = NULL;

	switch (opcode)
	{
	//READ
	case 0x7E: //LD A, (IX+d) OR LD A, (IY+d) 
		d = fetch();
		readVal = read(index + d);
		cpuRegistery.registery.A = readVal;
		tickStates = 19;
		break;
	case 0x46: //LD B, (IX+d) OR LD B, (IY+d) 
		d = fetch();
		readVal = read(index + d);
		cpuRegistery.registery.A = readVal;
		tickStates = 19;
		break;
	case 0x4E: //LD C, (IX+d) OR LD C, (IY+d) 
		d = fetch();
		readVal = read(index + d);
		cpuRegistery.registery.A = readVal;
		tickStates = 19;
		break;
	case 0x56: //LD D, (IX+d) OR LD D, (IY+d) 
		d = fetch();
		readVal = read(index + d);
		cpuRegistery.registery.A = readVal;
		tickStates = 19;
		break;
	case 0x5E: //LD E, (IX+d) OR LD E, (IY+d) 
		d = fetch();
		readVal = read(index + d);
		cpuRegistery.registery.A = readVal;
		tickStates = 19;
		break;
	case 0x66: //LD H, (IX+d) OR LD H, (IY+d) 
		d = fetch();
		readVal = read(index + d);
		cpuRegistery.registery.A = readVal;
		tickStates = 19;
		break;
	case 0x6E: //LD L, (IX+d) OR LD L, (IY+d) 
		d = fetch();
		readVal = read(index + d);
		cpuRegistery.registery.A = readVal;
		tickStates = 19;
		break;
		
	//WRITE
	case 0x77: //LD (IX+d), A  OR LD (IY+d), A
		d = fetch();
		write(index + d, cpuRegistery.registery.A);
		tickStates = 19;
		break;
	case 0x70: //LD (IX+d), B  OR LD (IY+d), B
		d = fetch();
		write(index + d, cpuRegistery.registery.A);
		tickStates = 19;
		break;
	case 0x71: //LD (IX+d), C  OR LD (IY+d), C
		d = fetch();
		write(index + d, cpuRegistery.registery.A);
		tickStates = 19;
		break;
	case 0x72: //LD (IX+d), D  OR LD (IY+d), D
		d = fetch();
		write(index + d, cpuRegistery.registery.A);
		tickStates = 19;
		break;
	case 0x73: //LD (IX+d), E  OR LD (IY+d), E
		d = fetch();
		write(index + d, cpuRegistery.registery.A);
		tickStates = 19;
		break;
	case 0x74: //LD (IX+d), H  OR LD (IY+d), H
		d = fetch();
		write(index + d, cpuRegistery.registery.A);
		tickStates = 19;
		break;
	case 0x75: //LD (IX+d), L  OR LD (IY+d), L
		d = fetch();
		write(index + d, cpuRegistery.registery.A);
		tickStates = 19;
		break;
	case 0x36: //#LD(HL)R_(HL)n
		n = fetch();
		write(cpuRegistery.registery.HL, n);
		tickStates = 19;
		break;

	//Operation LD SP, IX/IY
	case 0xF9:
		cpuRegistery.sp = read(index);
		tickStates = 10;
		break;
	default:
		break;
	}


	return tickStates;
}

void Gameboy::InitilizeOpCodeTable()
{
	//everycommand is either no operation or returns combining opcode.
	for (int i = 0x00; i <= 0xFF; i++)
	{
		gbOpCodeTable[i] = &Gameboy::NOP;
	}

	gbOpCodeTable[0x7F] = &Gameboy::LD_A_A; //+LD AA
	gbOpCodeTable[0x78] = &Gameboy::LD_A_B; //+LD AB
	gbOpCodeTable[0x79] = &Gameboy::LD_A_C; //+LD AC
	gbOpCodeTable[0x7A] = &Gameboy::LD_A_D; //+LD AD
	gbOpCodeTable[0x7B] = &Gameboy::LD_A_E; //+LD AE
	gbOpCodeTable[0X7C] = &Gameboy::LD_A_H; //+LD AH
	gbOpCodeTable[0X7D] = &Gameboy::LD_A_L; //+LD AL
	gbOpCodeTable[0x77] = &Gameboy::LD_B_A; //+LD BA
	gbOpCodeTable[0x40] = &Gameboy::LD_B_B; //+LD BB
	gbOpCodeTable[0x41] = &Gameboy::LD_B_C; //+LD BC
	gbOpCodeTable[0x42] = &Gameboy::LD_B_D; //+LD BD
	gbOpCodeTable[0x43] = &Gameboy::LD_B_E; //+LD BE
	gbOpCodeTable[0x44] = &Gameboy::LD_B_H; //+LD BH
	gbOpCodeTable[0x45] = &Gameboy::LD_B_L; //+LD BL
	gbOpCodeTable[0x4F] = &Gameboy::LD_C_A; //+LD CA
	gbOpCodeTable[0x48] = &Gameboy::LD_C_B; //+LD CB
	gbOpCodeTable[0x49] = &Gameboy::LD_C_C; //+LD CC
	gbOpCodeTable[0x4A] = &Gameboy::LD_C_D; //+LD CD
	gbOpCodeTable[0x4B] = &Gameboy::LD_C_E; //+LD CE
	gbOpCodeTable[0X4C] = &Gameboy::LD_C_H; //+LD CH
	gbOpCodeTable[0X4D] = &Gameboy::LD_C_L; //+LD CL
	gbOpCodeTable[0x57] = &Gameboy::LD_D_A; //+LD DA
	gbOpCodeTable[0x50] = &Gameboy::LD_D_B; //+LD DB
	gbOpCodeTable[0x51] = &Gameboy::LD_D_C; //+LD DC
	gbOpCodeTable[0x52] = &Gameboy::LD_D_D; //+LD DD
	gbOpCodeTable[0x53] = &Gameboy::LD_D_E; //+LD DE
	gbOpCodeTable[0x54] = &Gameboy::LD_D_H; //+LD DH
	gbOpCodeTable[0x55] = &Gameboy::LD_D_L; //+LD DL
	gbOpCodeTable[0x5F] = &Gameboy::LD_E_A; //+LD EA
	gbOpCodeTable[0x58] = &Gameboy::LD_E_B; //+LD EB
	gbOpCodeTable[0x59] = &Gameboy::LD_E_C; //+LD EC
	gbOpCodeTable[0x5A] = &Gameboy::LD_E_D; //+LD ED
	gbOpCodeTable[0x5B] = &Gameboy::LD_E_E; //+LD EE
	gbOpCodeTable[0x5C] = &Gameboy::LD_E_H; //+LD EH
	gbOpCodeTable[0x5D] = &Gameboy::LD_E_L; //+LD EL
	gbOpCodeTable[0x67] = &Gameboy::LD_H_A; //+LD HA
	gbOpCodeTable[0x60] = &Gameboy::LD_H_B; //+LD HB
	gbOpCodeTable[0x61] = &Gameboy::LD_H_C; //+LD HC
	gbOpCodeTable[0x62] = &Gameboy::LD_H_D; //+LD HD
	gbOpCodeTable[0x63] = &Gameboy::LD_H_E; //+LD HE
	gbOpCodeTable[0x64] = &Gameboy::LD_H_H; //+LD HH
	gbOpCodeTable[0x65] = &Gameboy::LD_H_L; //+LD HL
	gbOpCodeTable[0x6F] = &Gameboy::LD_L_A; //+LD LA
	gbOpCodeTable[0x68] = &Gameboy::LD_L_B; //+LD LB
	gbOpCodeTable[0x69] = &Gameboy::LD_L_C; //+LD LC
	gbOpCodeTable[0x6A] = &Gameboy::LD_L_D; //+LD LD
	gbOpCodeTable[0x6B] = &Gameboy::LD_L_E; //+LD LE
	gbOpCodeTable[0x6C] = &Gameboy::LD_L_H; //+LD LH
	gbOpCodeTable[0x6D] = &Gameboy::LD_L_L; //+LD LL
	gbOpCodeTable[0x3E] = &Gameboy::LD_A_nn; //+LDAnA(#)
	gbOpCodeTable[0x06] = &Gameboy::LD_B_n; //#LD(nn)n_Bn
	gbOpCodeTable[0x0E] = &Gameboy::LD_C_n; //#LD(nn)n_Cn
	gbOpCodeTable[0x16] = &Gameboy::LD_D_n; //#LD(nn)n_Dn
	gbOpCodeTable[0x1E] = &Gameboy::LD_E_n; //#LD(nn)n_En
	gbOpCodeTable[0x26] = &Gameboy::LD_H_n; //#LD(nn)n_Hn
	gbOpCodeTable[0x2E] = &Gameboy::LD_L_n; //#LD(nn)n_Ln

		//Operation LD r, (HL)
	gbOpCodeTable[0x7E] = &Gameboy::LD_A_HL; //+LDAnA(HL)
	gbOpCodeTable[0x46] = &Gameboy::LD_B_HL; //+LDR(HL)_B(HL)
	gbOpCodeTable[0x4E] = &Gameboy::LD_C_HL; //+LDR(HL)_C(HL)
	gbOpCodeTable[0x56] = &Gameboy::LD_D_HL; //+LDR(HL)_D(HL)
	gbOpCodeTable[0x5E] = &Gameboy::LD_E_HL; //+LDR(HL)_E(HL)
	gbOpCodeTable[0x66] = &Gameboy::LD_H_HL; //+LDR(HL)_H(HL)
	gbOpCodeTable[0x6E] = &Gameboy::LD_L_HL; //+LDR(HL)_L(HL)

		//Operation LD (HL), r
	gbOpCodeTable[0x77] = &Gameboy::LD_HL_A; //+LDnA(HL)A
	gbOpCodeTable[0x70] = &Gameboy::LD_HL_B; //+LD(HL)R_(HL)B
	gbOpCodeTable[0x71] = &Gameboy::LD_HL_C; //+LD(HL)R_(HL)C
	gbOpCodeTable[0x72] = &Gameboy::LD_HL_D; //+LD(HL)R_(HL)D
	gbOpCodeTable[0x73] = &Gameboy::LD_HL_E; //+LD(HL)R_(HL)E
	gbOpCodeTable[0x74] = &Gameboy::LD_HL_H; //+LD(HL)R_(HL)H
	gbOpCodeTable[0x75] = &Gameboy::LD_HL_L; //+LD(HL)R_(HL)L
	gbOpCodeTable[0x36] = &Gameboy::LD_HL_n; //#LD(HL)R_(HL)n

		
	gbOpCodeTable[0x02] = &Gameboy::LD_BC_A; //+LDnA(BC)A
	gbOpCodeTable[0x12] = &Gameboy::LD_DE_A; //+LDnA(DE)A
	gbOpCodeTable[0xEA] = &Gameboy::LD_nn_A; //+LDnA(nn)A
	gbOpCodeTable[0x0A] = &Gameboy::LD_A_BC; //+LDAnA(BC)
	gbOpCodeTable[0x1A] = &Gameboy::LD_A_DE; //+LDAnA(DE)
	gbOpCodeTable[0xFA] = &Gameboy::LD_A_nn; //+LDAnA(nn)
	gbOpCodeTable[0x2A] = &Gameboy::LDI_A_HL; //+LDHL(nn)
	gbOpCodeTable[0xF9] = &Gameboy::LD_SP_HL; //Operation LD SP,HL
#pragma region deleted OpCodes

	gbOpCodeTable[0xED] = &Gameboy::LD_I; //+LDI
	gbOpCodeTable[0xDD] = &Gameboy::LD_r_IX_PLUS_d; //Operation LD r, (IX+d)
	gbOpCodeTable[0xFD] = &Gameboy::LD_r_IY_PLUS_d; //Operation LD r, (IY+d)
#pragma endregion deleted OpCodes

#pragma region custom OpCodes
	gbOpCodeTable[0xF2] = &Gameboy::LD_A_FF00_PLUS_C;
	gbOpCodeTable[0xE2] = &Gameboy::LD_FF00_PLUS_C_A;
	gbOpCodeTable[0x3A] = &Gameboy::LDD_A_HL; 
	gbOpCodeTable[0x32] = &Gameboy::LDD_HL_A; 
	gbOpCodeTable[0x22] = &Gameboy::LDI_HL_A;
#pragma endregion custom OpCodes

#pragma region 16 bit OpCodes
	gbOpCodeTable[0x01] = &Gameboy::LD_A_FF00_PLUS_C;
	gbOpCodeTable[0x11] = &Gameboy::LD_A_FF00_PLUS_C;
	gbOpCodeTable[0x21] = &Gameboy::LD_HL_nn;
	gbOpCodeTable[0x31] = &Gameboy::LD_SP_nn;
	gbOpCodeTable[0xF9] = &Gameboy::LD_SP_HL;
	gbOpCodeTable[0xF8] = &Gameboy::LD_HL_SP_PLUS_n;
	gbOpCodeTable[0x08] = &Gameboy::LD_nn_SP;
	gbOpCodeTable[0xF5] = &Gameboy::PUSH_AF;
	gbOpCodeTable[0xC5] = &Gameboy::PUSH_BC;
	gbOpCodeTable[0xD5] = &Gameboy::PUSH_DE;
	gbOpCodeTable[0xE5] = &Gameboy::PUSH_HL;
	gbOpCodeTable[0xF1] = &Gameboy::POP_AF;
	gbOpCodeTable[0xC1] = &Gameboy::POP_BC;
	gbOpCodeTable[0xD1] = &Gameboy::POP_DE;
	gbOpCodeTable[0xE1] = &Gameboy::POP_HL;

#pragma endregion 16 bit OpCodes

#pragma region 8 bit ALU
	gbOpCodeTable[0x87] = &Gameboy::ADD_A_A;
	gbOpCodeTable[0x80] = &Gameboy::ADD_A_B;
	gbOpCodeTable[0x81] = &Gameboy::ADD_A_C;
	gbOpCodeTable[0x82] = &Gameboy::ADD_A_D;
	gbOpCodeTable[0x83] = &Gameboy::ADD_A_E;
	gbOpCodeTable[0x84] = &Gameboy::ADD_A_H;
	gbOpCodeTable[0x85] = &Gameboy::ADD_A_L;
	gbOpCodeTable[0x86] = &Gameboy::ADD_A_HL;
	gbOpCodeTable[0xC6] = &Gameboy::ADD_A_n;
	gbOpCodeTable[0x8F] = &Gameboy::ADC_A_A;
	gbOpCodeTable[0x88] = &Gameboy::ADC_A_B;
	gbOpCodeTable[0x89] = &Gameboy::ADC_A_C;
	gbOpCodeTable[0x8A] = &Gameboy::ADC_A_D;
	gbOpCodeTable[0x8B] = &Gameboy::ADC_A_E;
	gbOpCodeTable[0x8C] = &Gameboy::ADC_A_H;
	gbOpCodeTable[0x8D] = &Gameboy::ADC_A_L;
	gbOpCodeTable[0x8E] = &Gameboy::ADC_A_HL;
	gbOpCodeTable[0xCE] = &Gameboy::ADC_A_n;
	gbOpCodeTable[0x97] = &Gameboy::SUB_A_A;
	gbOpCodeTable[0x90] = &Gameboy::SUB_A_B;
	gbOpCodeTable[0x91] = &Gameboy::SUB_A_C;
	gbOpCodeTable[0x92] = &Gameboy::SUB_A_D;
	gbOpCodeTable[0x93] = &Gameboy::SUB_A_E;
	gbOpCodeTable[0x94] = &Gameboy::SUB_A_H;
	gbOpCodeTable[0x95] = &Gameboy::SUB_A_L;
	gbOpCodeTable[0x96] = &Gameboy::SUB_A_HL;
	gbOpCodeTable[0xD6] = &Gameboy::SUB_A_n;
	gbOpCodeTable[0xA7] = &Gameboy::AND_A_A;
	gbOpCodeTable[0xA0] = &Gameboy::AND_A_B;
	gbOpCodeTable[0xA1] = &Gameboy::AND_A_C;
	gbOpCodeTable[0xA2] = &Gameboy::AND_A_D;
	gbOpCodeTable[0xA3] = &Gameboy::AND_A_E;
	gbOpCodeTable[0xA4] = &Gameboy::AND_A_H;
	gbOpCodeTable[0xA5] = &Gameboy::AND_A_L;
	gbOpCodeTable[0xA6] = &Gameboy::AND_A_HL;
	gbOpCodeTable[0xE6] = &Gameboy::AND_A_n;
#pragma endregion 8 bit ALU

}