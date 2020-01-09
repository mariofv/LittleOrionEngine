#ifndef _MODULEEDITOR_H_
#define _MODULEEDITOR_H_

#include "Module.h"
#include "Globals.h"
#include "Component/ComponentCamera.h"

#include <ImGuizmo.h>

class ModuleEditor : public Module
{
public:
	ModuleEditor() = default;
	~ModuleEditor() = default;

	bool Init() override;
	bool CleanUp() override;

	void RenderEditorTools();
	void RenderCameraFrustum() const;
	void RenderGizmo();

	void OpenScene(const std::string &path) const;
	void SaveScene(const std::string &path) const;

	void ShowGizmoControls();


	bool gizmo_hovered = false;
private:
	ImGuizmo::OPERATION gizmo_operation = ImGuizmo::TRANSLATE;

};

#endif //_MODULEEDITOR_H_