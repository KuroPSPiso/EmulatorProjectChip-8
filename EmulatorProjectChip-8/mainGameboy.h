#pragma once
#ifndef MAINGAMEBOY_H_

class mainGameboy
{
public:
	mainGameboy();
	mainGameboy(int argc, char **argv);
	int mainGameboy::run();
	void setupGraphics();
	void inputUpdate();
	void drawGraphics();
	void loadApplication();
};

#endif // !MAINGAMEBOY_H_
#pragma once

