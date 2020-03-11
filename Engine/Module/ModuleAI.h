#ifndef _MODULEAI_H_
#define _MODULEAI_H_

#include "Module/Module.h"
#include "AI/NavMesh.h"
#include <MathGeoLib/MathGeoLib.h>

class PanelNavMesh;
class ComponentCamera;

class ModuleAI : public Module
{
public:
	ModuleAI() = default;
	~ModuleAI() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	void RenderNavMesh(ComponentCamera& camera);

	bool FindPath(float3& start, float3& end, std::vector<float3>& path) const;

public:
	std::vector<float3> debug_path;
private:
	NavMesh nav_mesh = NavMesh();

	friend PanelNavMesh;
};

#endif //_MODULEAI_H_