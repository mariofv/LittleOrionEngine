#ifndef _COMPONENTTRAILRENDERER_H_
#define _COMPONENTTRAILRENDERER_H_

#include "Component.h"
#include "Component/ComponentAABB.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"
#include "EditorUI/Panel/PanelScene.h"

#include "MathGeoLib.h"
#include <GL/glew.h>

class GameObject;

class ComponentTrailRenderer : public Component
{
public:

	ComponentTrailRenderer();
	ComponentTrailRenderer(GameObject * owner);
	~ComponentTrailRenderer();

	//Copy and move
	ComponentTrailRenderer(const ComponentTrailRenderer& component_to_copy) = default;
	ComponentTrailRenderer(ComponentTrailRenderer&& component_to_move) = default;

	ComponentTrailRenderer & operator=(const ComponentTrailRenderer & component_to_copy) = default;
	ComponentTrailRenderer & operator=(ComponentTrailRenderer && component_to_move) = default;
	void Delete() override;

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void InitData();

	void Render(std::vector<float>& to_render);
	void SwitchFrame();

	void ChangeTexture(uint32_t texture_uuid);

public:
	float vertices[12] = {
			0.5f,  0.5f, 0.0f,		// outline_left second point -- top right
			0.5f, -0.5f, 0.0f,		 // outline_right second point -- bottom right
		   -0.5f, -0.5f, 0.0f,		 // outline_right second point -- bottom left
		   -0.5f,  0.5f, 0.0f,	// outline_left first points -- top left
	};
	float* trail_renderer_vertices = nullptr;
	uint32_t* indexArray = nullptr;
	unsigned int rendered_vertices = 0, erase_vertices = 0;

private:
	uint32_t texture_uuid = 0;
	std::shared_ptr<Texture> trail_texture = nullptr;

	//Spritesheet params
	int x_tiles = 1;
	int y_tiles = 1;

	//color
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//Current sprite position
	float time_since_start = 0.f;
	float current_sprite_x = 0, current_sprite_y = 0;
	float sheet_speed = 1;
	bool oriented_to_camera;	

	bool is_spritesheet = false;
	float width = 5.f;
	float height = 5.f;

	unsigned int vbo, vao, ebo, trail_vao, trail_vbo;

	//Determines when the sprite is changed
	int innerCount = 0;

	friend class PanelComponent;
	friend class ComponentParticleSystem;
	friend class ComponentTrail;
};

#endif //_COMPONENTTRAILRENDERER_H_