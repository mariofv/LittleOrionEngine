#ifndef _MODULEUI_H_
#define _MODULEUI_H_
#define ENGINE_EXPORTS

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

#define MAX_NUM_LAYERS 8

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

	ComponentUI* CreateComponentUI(const ComponentUI::UIType ui_type, GameObject* owner);
	void RemoveComponentUI(ComponentUI* ui_to_remove);

	//Glyph init 
	void InitGlyph();
	ENGINE_API void SortComponentsUI();

private:
	void RenderUIGameObject(GameObject*, float4x4*);

public:
	std::map<GLchar, Character> Characters;
	float window_width, window_height;
	bool glyphInit = false;
	ComponentCanvas* main_canvas = nullptr;

private:
	FT_Library ft;
	FT_Face face;
	unsigned int text_texture;
	std::vector<ComponentUI*> ui_elements;
	std::vector<ComponentUI*> ordered_ui;

};

#endif //_MODULEUI_H_