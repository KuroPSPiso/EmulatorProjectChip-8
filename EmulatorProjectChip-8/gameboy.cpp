#include "gameboy.h"

void Gameboy::write(const uint16_t &address, const uint8_t value)
{
	memory[address] = value;
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
}

bool Gameboy::loadApplication(const char * filename)
{
	return false;
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

uint8_t Gameboy::decodeOPCode(const uint8_t & opcode)
{

	uint8_t tickStates = 0; 
	uint8_t n, readVal, subOpCode = NULL;

	switch (opcode)
	{
	case 0x00: //NOP
		break;

//Operation 8-Bit Load Group
	//Operation LD r, r'
	case 0x7F: //+LD AA
		cpuRegistery.registery.A = cpuRegistery.registery.A;
		tickStates = 4;
		break;
	case 0x78: //+LD AB
		cpuRegistery.registery.A = cpuRegistery.registery.B;
		tickStates = 4;
		break;
	case 0x79: //+LD AC
		cpuRegistery.registery.A = cpuRegistery.registery.C;
		tickStates = 4;
		break;
	case 0x7A: //+LD AD
		cpuRegistery.registery.A = cpuRegistery.registery.D;
		tickStates = 4;
		break;
	case 0x7B: //+LD AE
		cpuRegistery.registery.A = cpuRegistery.registery.E;
		tickStates = 4;
		break;
	case 0x7C: //+LD AH
		cpuRegistery.registery.A = cpuRegistery.registery.H;
		tickStates = 4;
		break;
	case 0x7D: //+LD AL
		cpuRegistery.registery.A = cpuRegistery.registery.L;
		tickStates = 4;
		break;

	case 0x47: //+LD BA
		cpuRegistery.registery.B = cpuRegistery.registery.A;
		tickStates = 4;
		break;
	case 0x40: //+LD BB
		cpuRegistery.registery.B = cpuRegistery.registery.B;
		tickStates = 4;
		break;
	case 0x41: //+LD BC
		cpuRegistery.registery.B = cpuRegistery.registery.C;
		tickStates = 4;
		break;
	case 0x42: //+LD BD
		cpuRegistery.registery.B = cpuRegistery.registery.D;
		tickStates = 4;
		break;
	case 0x43: //+LD BE
		cpuRegistery.registery.B = cpuRegistery.registery.E;
		tickStates = 4;
		break;
	case 0x44: //+LD BH
		cpuRegistery.registery.B = cpuRegistery.registery.H;
		tickStates = 4;
		break;
	case 0x45: //+LD BL
		cpuRegistery.registery.B = cpuRegistery.registery.L;
		tickStates = 4;
		break;

	case 0x4F: //+LD CA
		cpuRegistery.registery.C = cpuRegistery.registery.A;
		tickStates = 4;
		break;
	case 0x48: //+LD CB
		cpuRegistery.registery.C = cpuRegistery.registery.B;
		tickStates = 4;
		break;
	case 0x49: //+LD CC
		cpuRegistery.registery.C = cpuRegistery.registery.C;
		tickStates = 4;
		break;
	case 0x4A: //+LD CD
		cpuRegistery.registery.C = cpuRegistery.registery.D;
		tickStates = 4;
		break;
	case 0x4B: //+LD CE
		cpuRegistery.registery.C = cpuRegistery.registery.E;
		tickStates = 4;
		break;
	case 0X4C: //+LD CH
		cpuRegistery.registery.C = cpuRegistery.registery.H;
		tickStates = 4;
		break;
	case 0X4D: //+LD CL
		cpuRegistery.registery.C = cpuRegistery.registery.L;
		tickStates = 4;
		break;

	case 0x57: //+LD DA
		cpuRegistery.registery.D = cpuRegistery.registery.A;
		tickStates = 4;
		break;
	case 0x50: //+LD DB
		cpuRegistery.registery.D = cpuRegistery.registery.B;
		tickStates = 4;
		break;
	case 0x51: //+LD DC
		cpuRegistery.registery.D = cpuRegistery.registery.C;
		tickStates = 4;
		break;
	case 0x52: //+LD DD
		cpuRegistery.registery.D = cpuRegistery.registery.D;
		tickStates = 4;
		break;
	case 0x53: //+LD DE
		cpuRegistery.registery.D = cpuRegistery.registery.E;
		tickStates = 4;
		break;
	case 0x54: //+LD DH
		cpuRegistery.registery.D = cpuRegistery.registery.H;
		tickStates = 4;
		break;
	case 0x55: //+LD DL
		cpuRegistery.registery.D = cpuRegistery.registery.L;
		tickStates = 4;
		break;

	case 0x5F: //+LD EA
		cpuRegistery.registery.E = cpuRegistery.registery.A;
		tickStates = 4;
		break;
	case 0x58: //+LD EB
		cpuRegistery.registery.E = cpuRegistery.registery.B;
		tickStates = 4;
		break;
	case 0x59: //+LD EC
		cpuRegistery.registery.E = cpuRegistery.registery.C;
		tickStates = 4;
		break;
	case 0x5A: //+LD ED
		cpuRegistery.registery.E = cpuRegistery.registery.D;
		tickStates = 4;
		break;
	case 0x5B: //+LD EE
		cpuRegistery.registery.E = cpuRegistery.registery.E;
		tickStates = 4;
		break;
	case 0x5C: //+LD EH
		cpuRegistery.registery.E = cpuRegistery.registery.H;
		tickStates = 4;
		break;
	case 0x5D: //+LD EL
		cpuRegistery.registery.E = cpuRegistery.registery.L;
		tickStates = 4;
		break;

	case 0x67: //+LD HA
		cpuRegistery.registery.H = cpuRegistery.registery.A;
		tickStates = 4;
		break;
	case 0x60: //+LD HB
		cpuRegistery.registery.H = cpuRegistery.registery.B;
		tickStates = 4;
		break;
	case 0x61: //+LD HC
		cpuRegistery.registery.H = cpuRegistery.registery.C;
		tickStates = 4;
		break;
	case 0x62: //+LD HD
		cpuRegistery.registery.H = cpuRegistery.registery.D;
		tickStates = 4;
		break;
	case 0x63: //+LD HE
		cpuRegistery.registery.H = cpuRegistery.registery.E;
		tickStates = 4;
		break;
	case 0x64: //+LD HH
		cpuRegistery.registery.H = cpuRegistery.registery.H;
		tickStates = 4;
		break;
	case 0x65: //+LD HL
		cpuRegistery.registery.H = cpuRegistery.registery.L;
		tickStates = 4;
		break;

	case 0x6F: //+LD LA
		cpuRegistery.registery.L = cpuRegistery.registery.A;
		tickStates = 4;
		break;
	case 0x68: //+LD LB
		cpuRegistery.registery.L = cpuRegistery.registery.B;
		tickStates = 4;
		break;
	case 0x69: //+LD LC
		cpuRegistery.registery.L = cpuRegistery.registery.C;
		tickStates = 4;
		break;
	case 0x6A: //+LD LD
		cpuRegistery.registery.L = cpuRegistery.registery.D;
		tickStates = 4;
		break;
	case 0x6B: //+LD LE
		cpuRegistery.registery.L = cpuRegistery.registery.E;
		tickStates = 4;
		break;
	case 0x6C: //+LD LH
		cpuRegistery.registery.L = cpuRegistery.registery.H;
		tickStates = 4;
		break;
	case 0x6D: //+LD LL
		cpuRegistery.registery.L = cpuRegistery.registery.L;
		tickStates = 4;
		break;

	//Operation LD r, n
	case 0x3E: //+LDAnA(#)
		n = fetch();
		cpuRegistery.registery.A = n;
		tickStates = 7;
		break;
	case 0x06: //#LD(nn)n_Bn
		n = fetch();
		cpuRegistery.registery.B = n;
		tickStates = 8;
		break;
	case 0x0E: //#LD(nn)n_Cn
		n = fetch();
		cpuRegistery.registery.C = n;
		tickStates = 8;
		break;
	case 0x16: //#LD(nn)n_Dn
		n = fetch();
		cpuRegistery.registery.D = n;
		tickStates = 8;
		break;
	case 0x1E: //#LD(nn)n_En
		n = fetch();
		cpuRegistery.registery.E = n;
		tickStates = 8;
		break;
	case 0x26: //#LD(nn)n_Hn
		n = fetch();
		cpuRegistery.registery.H = n;
		tickStates = 8;
		break;
	case 0x2E: //#LD(nn)n_Ln
		n = fetch();
		cpuRegistery.registery.L = n;
		tickStates = 8;
		break;

	//Operation LD r, (HL)
	case 0x7E: //+LDAnA(HL)
		readVal = read(cpuRegistery.registery.HL);
		cpuRegistery.registery.A = readVal;
		tickStates = 7;
		break;
	case 0x46: //+LDR(HL)_B(HL)
		readVal = read(cpuRegistery.registery.HL);
		cpuRegistery.registery.B = readVal;
		tickStates = 7;
		break;
	case 0x4E: //+LDR(HL)_C(HL)
		readVal = read(cpuRegistery.registery.HL);
		cpuRegistery.registery.C = readVal;
		tickStates = 7;
		break;
	case 0x56: //+LDR(HL)_D(HL)
		readVal = read(cpuRegistery.registery.HL);
		cpuRegistery.registery.D = readVal;
		tickStates = 7;
		break;
	case 0x5E: //+LDR(HL)_E(HL)
		readVal = read(cpuRegistery.registery.HL);
		cpuRegistery.registery.E = readVal;
		tickStates = 7;
		break;
	case 0x66: //+LDR(HL)_H(HL)
		readVal = read(cpuRegistery.registery.HL);
		cpuRegistery.registery.H = readVal;
		tickStates = 7;
		break;
	case 0x6E: //+LDR(HL)_L(HL)
		readVal = read(cpuRegistery.registery.HL);
		cpuRegistery.registery.L = readVal;
		tickStates = 7;
		break;

	//Operation LD (HL), r
	case 0x77: //+LDnA(HL)A
		write(cpuRegistery.registery.HL, cpuRegistery.registery.A);
		tickStates = 7;
		break;
	case 0x70: //+LD(HL)R_(HL)B
		write(cpuRegistery.registery.HL, cpuRegistery.registery.B);
		tickStates = 7;
		break;
	case 0x71: //+LD(HL)R_(HL)C
		write(cpuRegistery.registery.HL, cpuRegistery.registery.C);
		tickStates = 7;
		break;
	case 0x72: //+LD(HL)R_(HL)D
		write(cpuRegistery.registery.HL, cpuRegistery.registery.D);
		tickStates = 7;
		break;
	case 0x73: //+LD(HL)R_(HL)E
		write(cpuRegistery.registery.HL, cpuRegistery.registery.E);
		tickStates = 7;
		break;
	case 0x74: //+LD(HL)R_(HL)H
		write(cpuRegistery.registery.HL, cpuRegistery.registery.H);
		tickStates = 7;
		break;
	case 0x75: //+LD(HL)R_(HL)L
		write(cpuRegistery.registery.HL, cpuRegistery.registery.L);
		tickStates = 7;
		break;
	case 0x36: //#LD(HL)R_(HL)n
		n = fetch();
		write(cpuRegistery.registery.HL, n);
		tickStates = 10;
		break;

	//Operation LD A, (nn)
	case 0x3A:
		n = read(fetch());
		cpuRegistery.registery.A = n;
		tickStates = 4;
		break;
	case 0x02: //+LDnA(BC)A
		cpuRegistery.registery.BC = cpuRegistery.registery.A;
		tickStates = 8;
		break;
	case 0x12: //+LDnA(DE)A
		cpuRegistery.registery.DE = cpuRegistery.registery.A;
		tickStates = 8;
		break;
	case 0xEA: //+LDnA(nn)A
		tickStates = 16;
		break;
	case 0x0A: //+LDAnA(BC)
		readVal = read(cpuRegistery.registery.BC);
		cpuRegistery.registery.A = readVal;
		tickStates = 7;
		break;
	case 0x1A: //+LDAnA(DE)
		readVal = read(cpuRegistery.registery.DE);
		cpuRegistery.registery.A = readVal;
		tickStates = 7;
		break;
	case 0xFA: //+LDAnA(nn)
		n = read(fetch());
		cpuRegistery.registery.A = n;
		tickStates = 13;
		break;
	case 0x2A: //+LDHL(nn)
		n = read(fetch());
		cpuRegistery.registery.H = n + 1;
		cpuRegistery.registery.L = n;
		tickStates = 16;
		break;
	case 0xED:

		tickStates = 20;
		break;
	//Operation LD SP,HL
	case 0xF9:
		cpuRegistery.sp = read(cpuRegistery.registery.HL);
		tickStates = 6;
		break;
#pragma region deleted OpCodes
	//Operation LD r, (IX+d)
	case 0xDD:
		subOpCode = fetch();
		tickStates = decodeOPCodeI__d(subOpCode, cpuRegistery.index.IX);
		break;
	case 0xFD:
		subOpCode = fetch();
		tickStates = decodeOPCodeI__d(subOpCode, cpuRegistery.index.IY);
		break;
	//Operation LD IX, nn
	case 0xDD & 0x21:
		tickStates = 14;
		break;
	//Operation LD IY, nn
	case 0xFD & 0x21:
		tickStates = 14;
		break;
	//Operation LD IX, nn
	case 0xDD & 0x2A:
		tickStates = 20;
		break;
	//Operation LD IY, nn
	case 0xFD & 0x2A:
		tickStates = 20;
		break;
#pragma endregion deleted OpCodes

	default:
		break;
	}

	return tickStates;
}

uint8_t Gameboy::decodeOPCodeI__d(const uint8_t &opcode, uint16_t index)
{
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

uint8_t Gameboy::PUSH(const uint16_t & qq)
{
	uint8_t tickStates = 0;

	cpuRegistery.sp = 0xC5 & read(qq);

	tickStates = 11;
	return tickStates;
}
