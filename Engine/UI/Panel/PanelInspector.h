#ifndef _PANELINSPECTOR_H_
#define _PANELINSPECTOR_H_

#include "Panel.h"

class PanelInspector : public Panel
{
public:
	PanelInspector() { enabled = true; };
	~PanelInspector() = default;

	void Render() override;

private:
	void ShowAddNewComponentButton();

};

#endif //_PANELINSPECTOR_H_