#ifndef _PANELDEBUG_H_
#define _PANELDEBUG_H_

#include "Panel.h"

class PanelDebug : public Panel
{
public:
	PanelDebug();
	~PanelDebug() = default;

	void Render() override;

};

#endif //_PANELDEBUG_H_