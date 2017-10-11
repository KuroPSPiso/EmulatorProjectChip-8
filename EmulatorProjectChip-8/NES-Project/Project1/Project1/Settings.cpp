#include "Settings.h"

BOOL Settings::LoadConfig()
{
	return FALSE;
}

void Settings::CreateConfig()
{
	//Set defaults
	vk_a		= __KEY_A;
	vk_b		= __KEY_B;
	vk_start	= __KEY_START;
	vk_select	= __KEY_SELECT;
	vk_left		= __KEY_LEFT;
	vk_up		= __KEY_UP;
	vk_right	= __KEY_RIGHT;
	vk_down		= __KEY_DOWN;
	vk_exit		= __KEY_EXIT;
}

Settings::Settings()
{
	if (!LoadConfig())
	{
		CreateConfig();
	}
}