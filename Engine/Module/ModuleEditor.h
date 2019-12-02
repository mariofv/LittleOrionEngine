#ifndef _MODULEEDITOR_H_
#define _MODULEEDITOR_H_

#include "Module.h"
#include "Globals.h"

struct ImFont;

class ModuleEditor : public Module
{
public:
	ModuleEditor() = default;
	~ModuleEditor() = default;

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	void Render();
	bool CleanUp();

	ImFont* GetFont(const Fonts font) const;

private:
	void LoadFonts();
};

#endif //_MODULEEDITOR_H_