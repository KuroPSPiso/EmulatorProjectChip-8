#include <io.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <direct.h>
//#include   // OpenGL graphics and input
#include "chip8.h" // Your cpu core implementation
#include "mainChip8.h"

#define __KEY_1 0x31
#define __KEY_2 0x32
#define __KEY_3 0x33
#define __KEY_C 0x34
#define __KEY_4 0x51
#define __KEY_5 0x57
#define __KEY_6 0x45
#define __KEY_D 0x52
#define __KEY_7 0x41
#define __KEY_8 0x53
#define __KEY_9 0x44
#define __KEY_E 0x46
#define __KEY_A 0x5A
#define __KEY_0 0x58
#define __KEY_B 0x43
#define __KEY_F 0x56
#define __KEY_EXIT 0x1B

chip8 __chip8;

void mainChip8::setupGraphics()
{
	int waitInSeconds = 2;
	printf("\n"); //prepared new line
	for (int i = 0; i < waitInSeconds; i++)
	{
		Sleep(1000);
		printf("\rStarting application in '%d' seconds.", (waitInSeconds - i));
	}
}

void mainChip8::inputUpdate()
{
	if (GetAsyncKeyState(__KEY_EXIT) > 0)//(GetAsyncKeyState(VK_ESCAPE) > 0)
	{
		exit(0);
	}

	if (GetAsyncKeyState(__KEY_1) > 0)	__chip8.key[0x1] = 1;
	else if (GetAsyncKeyState(__KEY_2) > 0)	__chip8.key[0x2] = 1;
	else if (GetAsyncKeyState(__KEY_3) > 0)	__chip8.key[0x3] = 1;
	else if (GetAsyncKeyState(__KEY_C) > 0)	__chip8.key[0xC] = 1;

	else if (GetAsyncKeyState(__KEY_4) > 0)	__chip8.key[0x4] = 1;
	else if (GetAsyncKeyState(__KEY_5) > 0)	__chip8.key[0x5] = 1;
	else if (GetAsyncKeyState(__KEY_6) > 0)	__chip8.key[0x6] = 1;
	else if (GetAsyncKeyState(__KEY_D) > 0)	__chip8.key[0xD] = 1;

	else if (GetAsyncKeyState(__KEY_7) > 0)	__chip8.key[0x7] = 1;
	else if (GetAsyncKeyState(__KEY_8) > 0)	__chip8.key[0x8] = 1;
	else if (GetAsyncKeyState(__KEY_9) > 0)	__chip8.key[0x9] = 1;
	else if (GetAsyncKeyState(__KEY_E) > 0)	__chip8.key[0xE] = 1;

	else if (GetAsyncKeyState(__KEY_A) > 0)	__chip8.key[0xA] = 1;
	else if (GetAsyncKeyState(__KEY_0) > 0)	__chip8.key[0x0] = 1;
	else if (GetAsyncKeyState(__KEY_B) > 0)	__chip8.key[0xB] = 1;
	else if (GetAsyncKeyState(__KEY_F) > 0)	__chip8.key[0xF] = 1;

}

void mainChip8::drawGraphics()
{
	__chip8.debugDisplay();
}

void mainChip8::loadApplication()
{
	HANDLE hFind;
	WIN32_FIND_DATA data;

	char* workingpath;
	if ((workingpath = _getcwd(NULL, 0)) == NULL)
	{
		perror("_getcwd error");
	}

	std::string temp_path2 = workingpath;
	temp_path2 += "\\ROMS\\";

	const char* path2 = temp_path2.c_str();

	std::string temp_path = workingpath;
	temp_path += "\\ROMS\\*";

	const char* path = temp_path.c_str();

	hFind = FindFirstFile(path, &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			printf("%s\n", data.cFileName);
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

	std::cout << "\n\n";

	std::cout << "Enter game name:" << std::flush;
	std::string s;
	std::getline(std::cin, s);
	s = path2 + s;
	__chip8.loadApplication(s.c_str()); //chip8 compatible code project

	free(workingpath);
}

mainChip8::mainChip8()
{
	//empty
}

mainChip8::mainChip8(int argc, char **argv)
{
	//init
}

int mainChip8::run()
{
	__chip8.initilize();
	loadApplication();
	setupGraphics();
	system("cls");

	// System emulation
	for (;;)
	{
		__chip8.emulateCycle(); //execute one cycle

		//on draw flag update screen
		if (__chip8.drawFlag)
		{
			drawGraphics();
		}

		inputUpdate();
	}
	return 0;
}