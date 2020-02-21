#ifndef _MODULEINPUT_H_
#define _MODULEINPUT_H_

#include "Module.h"
#include "Main/Globals.h"

typedef unsigned __int8 Uint8;
struct SDL_Cursor;

const int MAX_KEYS = 300;
const int NUM_MOUSE_BUTTONS = 5;


enum class KeyState {
	IDLE, DOWN, REPEAT, UP
};


class ModuleInput : public Module
{
public:
	ModuleInput() = default;
	~ModuleInput() = default;

	bool Init() override;
	update_status PreUpdate() override;
	bool CleanUp() override;

	bool GetKey(int id) const;
	bool GetMouseButtonDown(int id) const;
	bool GetKeyDown(int id) const;
	bool GetKeyUp(int id) const;

private:
	KeyState* keyboard_state;
	KeyState mouseButtons[NUM_MOUSE_BUTTONS];
	bool controlKeyDown = false;
	const Uint8 *keyboard = nullptr;
};

#endif _MODULEINPUT_H_