#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_

#include "Module/ModuleInput.h"
#include "Main/Globals.h"
#include <string>
#include <vector>
#include <map>

struct GameInput 
{
	std::string name;
	std::vector<KeyCode> keys;
	std::vector<MouseButton> mouse_buttons;
};

class InputManager
{
public:
	InputManager();
	~InputManager();

	void CreateGameInput();

	bool GetGameInput(std::string &name);
	bool GetGameInputDown(std::string &name);
	bool GetGameInputUp(std::string &name);

private:
	std::map<std::string, GameInput> game_inputs;

};



#endif // _INPUTMANAGER_H_