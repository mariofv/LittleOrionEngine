#ifndef _PANEL_H_
#define _PANEL_H_

class Panel
{
public:
	Panel() = default;
	virtual ~Panel() = default;

	virtual void Render() {};

	bool IsHovered() const { return hovered; };
	bool IsEnabled() const { return enabled; };
	void SwitchEnable() { enabled = !enabled; };

protected:
	bool hovered = false;
	bool enabled = false;
};

#endif // _PANEL_H_