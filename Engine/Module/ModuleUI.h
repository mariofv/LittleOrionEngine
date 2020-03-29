#ifndef _MODULEUI_H_
#define _MODULEUI_H_

#include "Component/ComponentUI.h"
#include "Module.h"
#include "Main/Globals.h"

// Std. Includes
#include <iostream>
#include <map>
// GLEW
#include <GL/glew.h>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H


class ComponentCanvas;
class ComponentCamera;
class ComponentText;
class ComponentButton;
class Gameobject;

struct SDL_Renderer;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	float2 Size;    // Size of glyph
	float2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};
class ModuleUI : public Module
{
public:
	ModuleUI() = default;
	~ModuleUI() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	void Render(const ComponentCamera* camera);

	ComponentUI* CreateComponentUI(const ComponentUI::UIType, GameObject*);
	void RemoveComponentUI(ComponentUI*);

	//Glyph init 
	void InitGlyph();
	bool glyphInit = false;
	FT_Library ft;
	FT_Face face;
	std::map<GLchar, Character> Characters;

	unsigned int text_texture;

	std::vector<ComponentCanvas*> canvases;
	std::vector<ComponentUI*> ui_elements;
	std::vector<ComponentText*> ui_texts;
	std::vector<ComponentButton*> ui_buttons;
	float window_width, window_height;

};

#endif //_MODULEUI_H_