#include "main.h"
#include <io.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <direct.h>
#include "mainChip8.h"
#include "mainGameboy.h"

mainChip8 chip8Launcher;
mainGameboy gameboyLauncher;

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

int main(int argc, char **argv)
{
	int launcherIndex = 0;
	while (launcherIndex == 0)
	{
		char buffer[256];
		printf("Select a machine to boot:\n\t1)\tChip-8\n");
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
	default:
		break;
	}

}