#ifndef  __MENUCONTROLLER_H__
#define  __MENUCONTROLLER_H__
class ModuleInput;
class MenuController
{
public:
	MenuController() = default;
	~MenuController() = default;

	static bool ConfirmMovedUp( ModuleInput& input);
	static bool ConfirmMovedDown( ModuleInput& input);
	static bool ComfirmButtonPressed( ModuleInput& input);
	static bool ConfirmMovedRight( ModuleInput& input);
	static bool ConfirmMovedLeft( ModuleInput& input);

}; 
#endif
