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

};

#endif //_PANELNAVMESH_H_