#ifndef _MODULEWINDOW_H_
#define _MODULEWINDOW_H_

#include "Module.h"

#include <SDL/SDL.h>

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow() = default;
	virtual ~ModuleWindow() = default;

	bool Init();
	bool CleanUp();

	void SetResizable(const bool resizable) const;
	void SetBordered(const bool bordered) const;

	void SetWindowed() const;
	void SetFullScreenDesktop() const;
	void SetFullScreen() const;

	void SetWidth(const int width);
	float GetWidth() const;
	void SetHeight(const int height);
	float GetHeight() const;

	void SetBrightness(const float brightness) const;

	void WindowResized(const unsigned width, const unsigned height);

	void ShowWindowOptions();

private:
	void InitOpenGLAttributes() const;
public:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

	// OpenGL context
	SDL_GLContext glcontext;

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