#ifndef _PANELGAME_H_
#define _PANELGAME_H_

#include "Panel.h"
#include <MathGeoLib.h>

class PanelConfiguration;

class PanelGame : public Panel
{
public:
	PanelGame();
	~PanelGame() = default;

	void Render() override;

private:
	void ShowEmptyGameWindow() const;

public:
	float game_window_content_area_width = 0;
	float game_window_content_area_height = 0;
	float previous_game_window_content_area_width = 0.f;
	float previous_game_window_content_area_height = 0.f;
	float2 game_window_content_area_pos = float2::zero;
	bool fullscreen = false;

};

#endif //_PANELGAME_H_
