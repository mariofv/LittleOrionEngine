#ifndef _PANELINSPECTOR_H_
#define _PANELINSPECTOR_H_

#include "Panel.h"
#include "InspectorSubpanel/PanelGameObject.h"
#include "InspectorSubpanel/PanelMaterial.h"
#include "InspectorSubpanel/PanelMetaFile.h"


class PanelInspector : public Panel
{
public:
	PanelInspector();
	~PanelInspector() = default;

	void Render() override;

public:
	PanelGameObject gameobject_panel;
	PanelMaterial material_panel;
	PanelMetaFile metafile_panel;

};

#endif //_PANELINSPECTOR_H_