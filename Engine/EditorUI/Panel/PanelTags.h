#ifndef _PANELTAGS_H_
#define _PANELTAGS_H_

#include "Panel.h"

class PanelTags : public Panel
{
public:
	PanelTags();
	~PanelTags() = default;

	void Render() override;
};

#endif //_PANELBUILDOPTIONS_H_