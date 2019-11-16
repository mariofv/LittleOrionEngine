#pragma once
#include "Module.h"
#include "Globals.h"

struct ImFont;

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	ImFont* GetFont(Fonts font);

private:
	void LoadFonts();
};
