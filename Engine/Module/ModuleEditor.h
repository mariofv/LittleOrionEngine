#ifndef _MODULEEDITOR_H_
#define _MODULEEDITOR_H_

#include "Module.h"
#include "Globals.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor() = default;
	~ModuleEditor() = default;

	bool Init() override;
	bool CleanUp() override;

	void OpenScene() const;
	void SaveScene() const;

	void OpenDefaultScene() const;

	void OpenTmpScene() const;
	void SaveTmpScene() const;
};

#endif //_MODULEEDITOR_H_