#ifndef _PANELINSPECTOR_H_
#define _PANELINSPECTOR_H_

#include "Panel.h"
#include "InspectorSubpanel/PanelGameObject.h"

class PanelInspector : public Panel
{
public:
	PanelInspector();
	~PanelInspector() = default;

	void Render() override;

public:
	PanelGameObject gameobject_panel;

};

#endif //_PANELINSPECTOR_H_