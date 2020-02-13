#ifndef _PANEL_H_
#define _PANEL_H_

#include <string>

class Panel
{
public:
	Panel() = default;
	virtual ~Panel() = default;

	virtual void Render() {};

	std::string GetWindowName() const { return window_name; };
	bool IsHovered() const { return hovered; };
	bool IsEnabled() const { return enabled; };
	void SwitchEnable() { enabled = !enabled; };

protected:
	std::string window_name = "";
	bool hovered = false;
	bool enabled = false;
};

#endif // _PANEL_H_