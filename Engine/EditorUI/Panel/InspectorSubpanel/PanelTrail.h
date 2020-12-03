#ifndef __PANELTRAIL_H__
#define __PANELTRAIL_H__

#include "EditorUI/Panel/Panel.h"

class ComponentTrail;
class PanelTrail : public Panel
{

public:
	PanelTrail();
	~PanelTrail() = default;

	void Render(ComponentTrail* particle_system);

private:
	bool modified_by_user = false;
};

#endif //__PANELTRAIL_H__
