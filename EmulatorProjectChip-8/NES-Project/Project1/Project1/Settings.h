#pragma once
#ifndef NES_SETTINGS_H_

#include <iostream>
#include "type.h"

//Keyboard Inputs
#define __KEY_A			0x5A //windowsKey Z
#define __KEY_B			0x58 //windowsKey X
#define __KEY_START		0x0D //windowsKey ENTER
#define __KEY_SELECT	0x10 //windowsKey (RIGHT-)SHIFT
#define __KEY_LEFT		0x25 //windowsKey LEFT-ARROW
#define __KEY_UP		0x26 //windowsKey UP-ARROW
#define __KEY_RIGHT		0x27 //windowsKey RIGHT-ARROW
#define __KEY_DOWN		0x28 //windowsKey DOWN-ARROW
#define __KEY_EXIT		0x1B //windowsKey ESC

#define configFileName "config.cfg"

class Settings
{
private:
	BOOL LoadConfig();
	void CreateConfig();

public:
	Settings();

	int vk_exit,
		vk_start,
		vk_select,
		vk_a,
		vk_b,
		vk_up,
		vk_down,
		vk_left,
		vk_right;
};

#endif // !NES_SETTINGS_H_