#ifndef _PANELGAME_H_
#define _PANELGAME_H_

#include "Panel.h"

#include <MathGeoLib.h>

class PanelGame : public Panel
{
public:
	PanelGame();
	~PanelGame() = default;

	void Render() override;

public:
	float game_window_content_area_width = 0;
	float game_window_content_area_height = 0;

	float2 game_window_content_area_pos;

private:
	void ShowEmptyGameWindow() const;
};

#endif //_PANELGAME_H_