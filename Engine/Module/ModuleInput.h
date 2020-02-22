#ifndef _MODULEINPUT_H_
#define _MODULEINPUT_H_

#include "Module.h"
#include "Main/Globals.h"

typedef unsigned __int8 Uint8;
struct SDL_Cursor;

class ModuleInput : public Module
{
public:
	ModuleInput() = default;
	~ModuleInput() = default;

	bool Init() override;
	update_status PreUpdate() override;
	bool CleanUp() override;
	bool testing = false;
private:
	bool controlKeyDown = false;
	const Uint8 *keyboard = nullptr;
};

#endif //_MODULEINPUT_H_