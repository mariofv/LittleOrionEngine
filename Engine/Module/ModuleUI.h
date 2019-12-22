#ifndef _MODULEUI_H_
#define _MODULEUI_H_

#include "Module.h"
#include "Globals.h"

class EngineUI;
struct ImFont;

class ModuleUI : public Module
{
public:
	ModuleUI() = default;
	~ModuleUI() = default;

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	void Render();
	bool CleanUp();

	ImFont* GetFont(const Fonts font) const;

private:
	void LoadFonts();

public:
	EngineUI* editor_ui = nullptr;

};

#endif //_MODULEUI_H_