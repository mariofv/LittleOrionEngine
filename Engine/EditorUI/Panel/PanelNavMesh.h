#ifndef _PANELNAVMESH_H_
#define _PANELNAVMESH_H_

#include "Panel.h"
#include <MathGeoLib/MathGeoLib.h>
#include <string>
#include "AI/NavMesh.h"

class PanelNavMesh : public Panel
{
public:
	PanelNavMesh();
	~PanelNavMesh() = default;

	void Render() override;

	bool succes = false;
	bool first_time = true;
	bool baked = false;

	NavMesh new_nav_mesh;

};

#endif //_PANELNAVMESH_H_