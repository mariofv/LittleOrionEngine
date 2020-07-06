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

	enum AlignmentType 
	{
		WORLD,
		VIEW_POINT,
		AXIAL			
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

	void Render(const float3& global_position);
	void SwitchFrame();

	void ChangeTexture(uint32_t texture_uuid);
	void ChangeBillboardType(ComponentBillboard::AlignmentType alignment_type);

	ENGINE_API void EmitOnce();
	ENGINE_API bool IsPlaying();

private:
	float4x4 ComponentBillboard::GetSpriteRotation(const float3& position) const;
	unsigned int GetBillboardVariation();

public:
	float width = 5.f;
	float height = 5.f;
	float transparency = 1.f;

	bool play_once = false;
	int current_sprite_x = 0;
	int current_sprite_y = 0;

private:
	GLuint shader_program;
	GLuint vbo, vao, ebo;

    uint32_t texture_uuid = 0;
	std::shared_ptr<Texture> billboard_texture = nullptr;
	
	//color
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	AlignmentType alignment_type = ComponentBillboard::AlignmentType::VIEW_POINT;

	//Spritesheet params
	bool is_spritesheet = false;
	int num_sprisheet_rows = 1;
	int num_sprisheet_columns = 1;
	int num_sprites = 1;

	int time_since_start = 0;
	int loop_time = 0;

	AnimationType animation_type = AnimationType::CONSTANT;
	bool play = true;

	friend class PanelComponent;
	friend class ComponentParticleSystem;
};

#endif //_COMPONENTBILLBOARD_H_