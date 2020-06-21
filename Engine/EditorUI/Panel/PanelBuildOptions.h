#ifndef _PANELBUILDOPTIONS_H_
#define _PANELBUILDOPTIONS_H_

#include "Panel.h"

class ModuleScene;

class PanelBuildOptions : public Panel
{
public:
	PanelBuildOptions();
	~PanelBuildOptions() = default;

	void Render() override;

private:
	bool build_options_modified = false;
};

#endif //_PANELBUILDOPTIONS_H_