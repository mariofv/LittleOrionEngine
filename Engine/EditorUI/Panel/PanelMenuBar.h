#ifndef _PANELMENUBAR_H_
#define _PANELMENUBAR_H_

#include "Component/ComponentUI.h"
#include "Panel.h"
#include <string>

class GameObject;

class PanelMenuBar : public Panel
{
public:
	PanelMenuBar();
	~PanelMenuBar() = default;

	void Render() override;

private:
	void ShowFileMenu();
	void ShowGameObjectMenu();
	void ShowWindowMenu();
	void ShowHelpMenu();
	GameObject* CreateUIGameObject(const char* name, ComponentUI::UIType ui_type) const;
};

#endif //_PANELMENUBAR_H_

