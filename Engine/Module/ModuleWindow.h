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

	bool Init() override;
	bool CleanUp() override;

	bool IsFocused() const;

	void SetResizable(bool resizable) const;
	void SetBordered(bool bordered) const;

	void SetWindowed() const;
	void SetFullScreenDesktop() const;
	void SetFullScreen() const;

	void SetWidth(int width);
	float GetWidth() const;
	void SetHeight(int height);
	float GetHeight() const;

	void SetBrightness(float brightness) const;

	void WindowResized(unsigned width, unsigned height);

	ENGINE_API void CloseWindow() const;

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

	friend class PanelConfiguration;
};

#endif // _MODULEWINDOW_H_