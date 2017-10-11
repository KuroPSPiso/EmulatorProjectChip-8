#include <io.h>
#include <iostream>
#include <string>

#include <windows.h>

//Includes for program
#include "type.h"
#include "Settings.h"
#include "CartridgeLDR.h"
#include "CPU.h"

using namespace std;

Settings* settings;
CartridgeLDR* cldr;
CPU* cpu;
BOOL runningState;
uint8_t joy1Data; // a, b , select, start, up, down, left, right
uint8_t joy2Data; // a, b , select, start, up, down, left, right

void Input()
{
	int keyVals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 }; //joy input (9th = exit)
	int keyVals2[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 }; //joy input (9th = exit)

	//WinOnly
#define winKeyPress(con, vk_key, val) if(GetAsyncKeyState(vk_key) != 0){ (con == 2) ? keyVals2[val] = 1 : keyVals[val] = 1; }
	winKeyPress(1, settings->vk_a, 0);
	winKeyPress(1, settings->vk_b, 1);
	winKeyPress(1, settings->vk_select, 2);
	winKeyPress(1, settings->vk_start, 3);
	winKeyPress(1, settings->vk_up, 4);
	winKeyPress(1, settings->vk_down, 5);
	winKeyPress(1, settings->vk_left, 6);
	winKeyPress(1, settings->vk_right, 7);
	winKeyPress(1, settings->vk_exit, 8);

	//All purpose
	joy1Data = 0;
	joy2Data = 0;
#define joydata(con, bit) (con == 2) ? joy2Data |= keyVals2[bit] << bit : joy1Data |= keyVals[bit] << bit;
	joydata(1, 0);
	joydata(1, 1);
	joydata(1, 2);
	joydata(1, 3);
	joydata(1, 4);
	joydata(1, 5);
	joydata(1, 6);
	joydata(1, 7);

	if (keyVals[8] == 1) runningState = FALSE;
}

void Run()
{
	runningState = TRUE;

	cpu = new CPU(cldr);
	if (cpu->operable == FALSE) return;

	for (;;)
	{
		Input();

		cpu->EmulateCycle();

		if (runningState == FALSE || cpu->operable == FALSE) break;
	}
}

int main(int argc, char **argv)
{
	char buffer[256];
	char* path;
	settings = new Settings();
	cldr = new CartridgeLDR();

	if (argc > 1) //arg 1 = execution
	{
		path = argv[1];
	}
	else
	{
		path = "E:\\Github\\EmulatorProjectChip-8\\ROMS\\BRIX";
	}

	printf("ROM Path: %s\n", path);
	printf("Booting Cartridge Slot, press enter to continue...\n");
	fgets(buffer, 256, stdin);
	
	BOOL r = cldr->LoadCartridge(path);

	printf("ROM Load Status: %s\n", (r == TRUE)?"Succeeded":"Failed");
	printf("Booting NES, press enter to continue...\n");
	fgets(buffer, 256, stdin);

	//run the program if it has loaded the rom, else exit.
	if (r == TRUE)
	{
		Run();
	}

	return 0;
}