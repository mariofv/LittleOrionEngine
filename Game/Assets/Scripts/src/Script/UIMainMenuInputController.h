#ifndef  __UIMAINMENUINPUTCONTROLLER_H__
#define  __UIMAINMENUINPUTCONTROLLER_H__

class ModuleInput;

class UIMainMenuInputController
{
public:
	UIMainMenuInputController() = default;
	~UIMainMenuInputController() = default;

	static bool ConfirmMovedUp( ModuleInput& input);
	static bool ConfirmMovedDown( ModuleInput& input);
	static bool ConfirmMovedRight( ModuleInput& input);
	static bool ConfirmMovedLeft( ModuleInput& input);
	static bool ComfirmButtonPressed( ModuleInput& input);
	static bool CancelButtonPressed( ModuleInput& input);

}; 
#endif
