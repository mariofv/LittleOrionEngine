#ifndef _PANELGAME_H_
#define _PANELGAME_H_

#include "Panel.h"

class PanelGame : public Panel
{
public:
	PanelGame();
	~PanelGame() = default;

	void Render() override;

private:
	void ShowEmptyGameWindow() const;

};

#endif //_PANELGAME_H_