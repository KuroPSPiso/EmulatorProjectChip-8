#include <io.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <direct.h>

#include "NES.h" // Your cpu core implementation
#include "mainNES.h"

#define __KEY_A 0x31
#define __KEY_B 0x32
#define __KEY_START 0x33
#define __KEY_SELECT 0x34
#define __KEY_LEFT 0x51
#define __KEY_UP 0x57
#define __KEY_RIGHT 0x45
#define __KEY_DOWN 0x52
#define __KEY_EXIT 0x1B

NES __NES;

void mainNES::setupGraphics()
{
	int waitInSeconds = 2;
	printf("\n"); //prepared new line
	for (int i = 0; i < waitInSeconds; i++)
	{
		Sleep(1000);
		printf("\rStarting application in '%d' seconds.", (waitInSeconds - i));
	}
}

void mainNES::inputUpdate()
{
	if (GetAsyncKeyState(__KEY_EXIT) > 0)//(GetAsyncKeyState(VK_ESCAPE) > 0)
	{
		exit(0);
	}

	//if (GetAsyncKeyState(__KEY_A) > 0)	__chip8.key[0xA] = 1;
	//else if (GetAsyncKeyState(__KEY_B) > 0)	__chip8.key[0xB] = 1;

}

void mainNES::drawGraphics()
{
	__NES.debugDisplay();
}

void mainNES::loadApplication()
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
	__NES.loadApplication(s.c_str()); //chip8 compatible code project

	free(workingpath);
}

mainNES::mainNES()
{
	//empty
}

mainNES::mainNES(int argc, char **argv)
{
	//init
}

int mainNES::run()
{
	__NES.RESET();
	loadApplication();
	setupGraphics();
	system("cls");
	//__NES.RESET();

	// System emulation
	for (;;)
	{
		__NES.emulateCycle(); //execute one cycle
		
		if (__NES.drawFlag) //on draw flag update screen
		{
			drawGraphics();
		}

		inputUpdate();
	}
	return 0;
}