#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_

#include "Module/ModuleInput.h"
#include "Main/Globals.h"
#include <string>
#include <vector>

struct GameInput 
{
	std::string name;
	std::vector<KeyCode> keys;
	std::vector<MouseButton> mouse;
};

class InputManager
{
public:
	InputManager();
	~InputManager();
};



#endif // _INPUTMANAGER_H_