#ifndef _PANELTOOLBAR_H_
#define _PANELTOOLBAR_H_

#include "Panel.h"

#include <string>

class PanelToolBar : public Panel
{
public:
	PanelToolBar();
	~PanelToolBar() = default;

	void Render() override;

private:
	void ShowGizmoControls();
	void ShowTimeControls();
};

#endif //_PANELTOOLBAR_H_