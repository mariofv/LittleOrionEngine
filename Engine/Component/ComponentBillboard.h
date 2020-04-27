#ifndef _COMPONENTBILLBOARD_H_
#define _COMPONENTBILLBOARD_H_

#include "Component.h"
#include "Component/ComponentAABB.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"
#include "EditorUI/Panel/PanelScene.h"

#include "MathGeoLib.h"
#include <GL/glew.h>

class GameObject;

class ComponentBillboard : public Component
{
public:

	enum AlignmentType {
		VIEW_POINT,
		AXIAL,	
		SPRITESHEET,
		CROSSED			
	};
	
	ComponentBillboard();
	ComponentBillboard(const std::string& texture_path, float width, float height, AlignmentType a_type);
	ComponentBillboard(GameObject * owner);
	~ComponentBillboard();

	//Copy and move
	ComponentBillboard(const ComponentBillboard& component_to_copy) = default;
	ComponentBillboard(ComponentBillboard&& component_to_move) = default;

	ComponentBillboard & operator=(const ComponentBillboard & component_to_copy) = default;
	ComponentBillboard & operator=(ComponentBillboard && component_to_move) = default;
	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;
	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Render(const float3& position);
	void SwitchFrame();

	//TODO
	void ChangeTexture(std::string path);

	void ChangeBillboardType(ComponentBillboard::AlignmentType alignment_type);

	AlignmentType alignment_type;

	//Spritesheet params
	int x_tiles = 0;
	int y_tiles = 0;

	//Current sprite position
	float current_sprite_x = 0, current_sprite_y = 0;
	std::shared_ptr<Texture> billboard_texture = nullptr;
	float sheet_speed = 0;
	bool oriented_to_camera;

private:
	bool is_spritesheet = false;
	float width = 5.f;
	float height = 5.f;

	float vertices[20] = {
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f
	};
	unsigned int indices[6] = {
		0, 1, 3,
		1, 2, 3
	};

	unsigned int EBO;

	//Determines when the sprite is changed
	int innerCount = 0;

	friend class PanelComponent;
};

#endif //_COMPONENTBILLBOARD_H_