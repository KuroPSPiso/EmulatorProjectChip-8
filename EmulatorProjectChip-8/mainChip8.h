#pragma once
#ifndef MAINCHIP8_H_

class mainChip8
{
public:
	mainChip8();
	mainChip8(int argc, char **argv);
	int mainChip8::run();
	void setupGraphics();
	void inputUpdate();
	void drawGraphics();
	void loadApplication();
};

#endif // !MAINCHIP8_H_
