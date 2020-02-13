#ifndef _PANELABOUT_H_
#define _PANELABOUT_H_

#include "Panel.h"

#include <string>

class PanelAbout : public Panel
{
public:
	PanelAbout();
	~PanelAbout() = default;

	void Render() override;

private:
	void MenuURL(const std::string& text, const std::string& link);
};

#endif //_PANELABOUT_H_