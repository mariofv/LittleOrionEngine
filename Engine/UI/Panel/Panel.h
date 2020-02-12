#ifndef _PANEL_H_
#define _PANEL_H_

class Panel
{
public:
	Panel() = default;
	virtual ~Panel() = default;

	virtual void Render(){}
	virtual bool IsHovered() const {return hovered;};

protected:
	bool hovered = false;
};

#endif // _PANEL_H_