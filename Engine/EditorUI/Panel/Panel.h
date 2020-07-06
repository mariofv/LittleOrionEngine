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
	bool IsFocused() const { return focused; };
	bool IsEnabled() const { return enabled; };
	void SwitchEnable() { enabled = !enabled; };
	bool IsOpened() const { return opened; };
	void SwitchOpen() { opened = !opened; };
	void Open() { opened = true; };

protected:
	std::string window_name = "";
	bool hovered = false;
	bool focused = false;
	bool enabled = false;
	bool opened = false;
};

#endif // _PANEL_H_