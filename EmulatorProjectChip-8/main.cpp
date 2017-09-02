#include "main.h"
#include <io.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <direct.h>
#include "mainChip8.h"
#include "mainGameboy.h"
#include "mainNES.h"

mainChip8 chip8Launcher;
mainGameboy gameboyLauncher;
mainNES nesLauncher;

using namespace std;

void launchChip8(int argc, char **argv)
{
	chip8Launcher = mainChip8(argc, argv);
	chip8Launcher.run();
}

void launchGameboy(int argc, char **argv)
{
	gameboyLauncher = mainGameboy(argc, argv);
	gameboyLauncher.run();
}

void launchNES(int argc, char **argv)
{
	nesLauncher = mainNES(argc, argv);
	nesLauncher.run();
}

int main(int argc, char **argv)
{
	int launcherIndex = 0;
	while (launcherIndex == 0)
	{
		char buffer[256];
		printf("Select a machine to boot:\n\t1)\tChip-8\n\t2)\tGameBoy (W.I.P.)\n\t2)\tNES (W.I.P.)");
		printf("Enter a number: ");
		fgets(buffer, 256, stdin);

		launcherIndex = atoi(buffer);
	}

	switch (launcherIndex)
	{
	case 1:
		launchChip8(argc, argv);
		break;
	case 2:
		launchGameboy(argc, argv);
		break;
	case 3:
		launchNES(argc, argv);
	default:
		break;
	}

}