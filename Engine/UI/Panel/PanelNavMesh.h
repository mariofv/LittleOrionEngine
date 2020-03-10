#ifndef _PANELNAVMESH_H_
#define _PANELNAVMESH_H_

#include "Panel.h"
#include <string>

class PanelNavMesh : public Panel
{
public:
	PanelNavMesh();
	~PanelNavMesh() = default;

	void Render() override;

	bool succes = false;
	bool first_time = true;

	//PathFinding
	float3 start_position;
	float3 end_position;

};

#endif //_PANELNAVMESH_H_