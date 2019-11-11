#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	virtual ~ModuleWindow();

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
	void setHeight(const int height);
	void setBrightness(const float brightness) const;

	void WindowResized(const unsigned width, const unsigned height);

	void ShowWindowOptions();

public:
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screen_surface = NULL;

	// OpenGL context
	SDL_GLContext glcontext;

public:
	int screen_width;
	int screen_height;

private:
	int height;
	int width;

	int fullscreen;
	bool bordered;
	bool resizable;

	float brightness;
};

#endif // __ModuleWindow_H__