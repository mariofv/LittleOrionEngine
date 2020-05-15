#ifndef  __HELPMENULOGIC_H__
#define  __HELPMENULOGIC_H__

#include "Script.h"
class HelpMenuLogic : public Script
{

public:
	HelpMenuLogic();
	~HelpMenuLogic() = default;

	void Update() override;

	void OnInspector(ImGuiContext * context) override;

	void InitPublicGameObjects() override;
	bool ConfirmMovedRight();
	bool ConfirmMovedLeft();
private:
	GameObject* help_controller = nullptr;
	GameObject* help_keyboard = nullptr;
};
extern "C" SCRIPT_API HelpMenuLogic* HelpMenuLogicDLL(); //This is how we are going to load the script
#endif
