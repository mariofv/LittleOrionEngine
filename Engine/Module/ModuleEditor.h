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

	void OpenScene(const std::string &path) const;
	void SaveScene(const std::string &path) const;
};

#endif //_MODULEEDITOR_H_