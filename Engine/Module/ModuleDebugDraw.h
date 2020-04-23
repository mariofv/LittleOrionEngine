#ifndef _MODULEDEBUGDRAW_H_
#define _MODULEDEBUGDRAW_H_

#include "Module.h"

#include <GL/glew.h>
#include <MathGeoLib.h>

class Billboard;
class ComponentCamera;
class GameObject;
class Grid;
class IDebugDrawOpenGLImplementation;

class ModuleDebugDraw : public Module
{
public:
	ModuleDebugDraw() = default;
	~ModuleDebugDraw() = default;

	bool Init() override;
	bool CleanUp() override;

	void Render();
	void RenderDebugDraws(const ComponentCamera& camera);

private:
	void RenderOutline() const;

	void RenderTangentsAndBitangents() const;

	void RenderCameraFrustum() const;
	void RenderLightGizmo() const;
	void RenderBones() const;
	void RenderBone(const GameObject* current_bone, const GameObject* last_bone, const float3& color) const;

	void RenderBoundingBoxes() const;
	void RenderGlobalBoundingBoxes() const;
	void RenderBillboards() const;
	void RenderPathfinding() const;


private:
	static IDebugDrawOpenGLImplementation* dd_interface_implementation;

	const float MIN_MAGNITUDE_ORDER_GRID = 0;
	const float MAX_MAGNITUDE_ORDER_GRID = 20;

	Billboard* camera_billboard = nullptr;
	Billboard* light_billboard = nullptr;
	Billboard* spritesheet_billboard = nullptr;

	Grid* grid = nullptr;
};

#endif //_MODULEDEBUGDRAW_H_