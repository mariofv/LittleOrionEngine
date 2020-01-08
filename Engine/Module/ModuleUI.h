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

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	void Render();
	bool CleanUp() override;

	ImFont* GetFont(const Fonts & font) const;

private:
	void LoadFonts();

public:
	EngineUI* editor_ui = nullptr;

};

#endif //_MODULEUI_H_