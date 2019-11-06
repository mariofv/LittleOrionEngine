#pragma once
#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Paths -------
# define DEFAULT_VERTEX_SHADER_PATH "./default.vs"
# define DEFAULT_FRAGMENT_SHADER_PATH "./default.fs"
# define TEXTURE_FRAGMENT_SHADER_PATH "./texture.fs"

# define WOBBLE_VERTEX_SHADER_PATH "./wobble.vs"
# define WOBBLE_FRAGMENT_SHADER_PATH "./wobble.fs"

# define LENA_TEXTURE_PATH "./lena.png"

// Configuration -----------
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define FULLSCREEN false
#define RESIZABLE true
#define BORDERED true

#define VSYNC true
#define TITLE "Orion Engine"