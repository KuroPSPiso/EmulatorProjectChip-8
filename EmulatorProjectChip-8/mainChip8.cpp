#include <io.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <direct.h>
//#include   // OpenGL graphics and input
#include "chip8.h" // Your cpu core implementation
#include "mainChip8.h"

#define __KEY_1 0x31	//1
#define __KEY_2 0x32	//2
#define __KEY_3 0x33	//3
#define __KEY_C 0x34	//4
#define __KEY_4 0x51	//Q
#define __KEY_5 0x57	//W
#define __KEY_6 0x45	//E
#define __KEY_D 0x52	//R
#define __KEY_7 0x41	//A
#define __KEY_8 0x53	//S
#define __KEY_9 0x44	//D
#define __KEY_E 0x46	//F
#define __KEY_A 0x5A	//Z
#define __KEY_0 0x58	//X
#define __KEY_B 0x43	//C
#define __KEY_F 0x56	//V
#define __KEY_EXIT 0x1B	//ESC

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
	if (GetKeyState(__KEY_EXIT) < 0)
	{
		exit(0);
	}

	for (int i = 0; i < sizeof(__chip8.key); i++)
	{
		__chip8.key[i] = 0; //default state = off
	}

	if (GetKeyState(__KEY_1) < 0)		__chip8.key[0] = 1;
	if (GetKeyState(__KEY_2) < 0)		__chip8.key[1] = 1;
	if (GetKeyState(__KEY_3) < 0)		__chip8.key[2] = 1;
	if (GetKeyState(__KEY_C) < 0)		__chip8.key[3] = 1;

	if (GetKeyState(__KEY_4) < 0)		__chip8.key[4] = 1;
	if (GetKeyState(__KEY_5) < 0)		__chip8.key[5] = 1;
	if (GetKeyState(__KEY_6) < 0)		__chip8.key[6] = 1;
	if (GetKeyState(__KEY_D) < 0)		__chip8.key[7] = 1;

	if (GetKeyState(__KEY_7) < 0)		__chip8.key[8] = 1;
	if (GetKeyState(__KEY_8) < 0)		__chip8.key[9] = 1;
	if (GetKeyState(__KEY_9) < 0)		__chip8.key[10] = 1;
	if (GetKeyState(__KEY_E) < 0)		__chip8.key[11] = 1;

	if (GetAsyncKeyState(__KEY_A) < 0)	__chip8.key[12] = 1;
	if (GetAsyncKeyState(__KEY_0) < 0)	__chip8.key[13] = 1;
	if (GetAsyncKeyState(__KEY_B) < 0)	__chip8.key[14] = 1;
	if (GetAsyncKeyState(__KEY_F) < 0)	__chip8.key[15] = 1;

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

		//DRAWTIME IN MS
		clock_t timespend_total = clock();

		__chip8.emulateCycle(); //execute one cycle
		inputUpdate();

		clock_t timespend_cpu = clock() - timespend_total;

		printf("%.f (CYCLE & INPUT TIME) ", timespend_cpu);

		//on draw flag update screen
		if (__chip8.drawFlag)
		{
			drawGraphics();
		}

		timespend_total = clock() - timespend_total;

		float total_frames = ((float)timespend_total) / 1000;
		float fps = 1 / total_frames;

		printf("%.f FPS (FULL TIME) ", fps);
	}
	return 0;
}