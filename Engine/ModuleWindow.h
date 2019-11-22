#ifndef _MODULEWINDOW_H_
#define _MODULEWINDOW_H_

#include "Module.h"

#include "SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow() = default;
	virtual ~ModuleWindow() = default;

	// Called before quitting
	bool Init();
	update_status Update();
	// Called before quitting
	bool CleanUp();

	void setResizable(const bool resizable) const;
	void setBordered(const bool bordered) const;

	void setWindowed() const;
	void setFullScreenDesktop() const;
	void setFullScreen() const;

	void setWidth(const int width);
	float getWidth() const;
	void setHeight(const int height);
	float getHeight() const;

	void setBrightness(const float brightness) const;

	void WindowResized(const unsigned width, const unsigned height);

	void ShowWindowOptions();

public:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

	// OpenGL context
	SDL_GLContext glcontext;

public:
	int screen_width = 0;
	int screen_height = 0;

private:
	int height = 0;
	int width = 0;

	int fullscreen = -1;
	bool bordered = false;
	bool resizable = false;

	float brightness = 1.f;
};

#endif // _MODULEWINDOW_H_