#ifndef _COMPONENTBILLBOARD_H_
#define _COMPONENTBILLBOARD_H_

#define ENGINE_EXPORTS

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
	
	enum AnimationType {
		CONSTANT,
		RANDOM_BETWEEN_VALUES
	};
	ComponentBillboard();
	ComponentBillboard(GameObject* owner);
	~ComponentBillboard();

	//Copy and move
	ComponentBillboard(const ComponentBillboard& component_to_copy) = default;
	ComponentBillboard(ComponentBillboard&& component_to_move) = default;

	ComponentBillboard& operator=(const ComponentBillboard & component_to_copy) = default;
	ComponentBillboard& operator=(ComponentBillboard && component_to_move) = default;
	void Delete() override;

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void InitData();

	void Render(const float3& position);
	void SwitchFrame();

	void ChangeTexture(uint32_t texture_uuid);
	void ChangeBillboardType(ComponentBillboard::AlignmentType alignment_type);

	ENGINE_API void EmitOnce();

public:
	float width = 5.f;
	float height = 5.f;
	float transparency = 1.f;
	bool play_once = false;
	float current_sprite_x = 0, current_sprite_y = 0;
private:
	AlignmentType alignment_type = ComponentBillboard::AlignmentType::VIEW_POINT;

    uint32_t texture_uuid = 0;
	std::shared_ptr<Texture> billboard_texture = nullptr;
	
	//Spritesheet params
	int x_tiles = 1;
	int y_tiles = 1;

	//color
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//Current sprite position
	float time_since_start = 0.f;

	float sheet_speed = 1;
	bool oriented_to_camera;
	AnimationType animation_type = AnimationType::CONSTANT;
	int num_of_tiles = 0;
	bool is_spritesheet = false;

	int total_frame = 0;
	int current_frame = 0;
	bool play = true;
	unsigned int vbo, vao, ebo;

	//Determines when the sprite is changed
	int innerCount = 0;

	friend class PanelComponent;
	friend class ComponentParticleSystem;
};

#endif //_COMPONENTBILLBOARD_H_