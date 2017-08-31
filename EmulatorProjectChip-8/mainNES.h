#pragma once
#ifndef MAINNES_H_

class mainNES
{
public:
	mainNES();
	mainNES(int argc, char **argv);
	int mainNES::run();
	void setupGraphics();
	void inputUpdate();
	void drawGraphics();
	void loadApplication();
};

#endif // !MAINNES_H_
#pragma once

