#ifndef _PANELNAVMESH_H_
#define _PANELNAVMESH_H_

#include "Panel.h"
#include <MathGeoLib/MathGeoLib.h>
#include <string>

class PanelNavMesh : public Panel
{
public:
	PanelNavMesh();
	~PanelNavMesh() = default;

	void Render() override;

	bool succes = false;
	bool first_time = true;

};

#endif //_PANELNAVMESH_H_