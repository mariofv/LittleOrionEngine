#ifndef _COMPONENTBILLBOARD_H_
#define _COMPONENTBILLBOARD_H_

#define ENGINE_EXPORTS

#include "Component.h"

#include <MathGeoLib.h>
#include <GL/glew.h>

class GameObject;
class Texture;

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
	void Update() override;

	void Render(const float3& global_position);
	void CommonUniforms(const GLuint &shader_program);

	void ChangeTexture(uint32_t texture_uuid);
	void ChangeTextureEmissive(uint32_t texture_uuid);
	void ChangeBillboardType(ComponentBillboard::AlignmentType alignment_type);


	void ComputeAnimationFrame(float progress);

	ENGINE_API void Play();
	ENGINE_API bool IsPlaying();
	ENGINE_API void SetOrientation(bool is_oriented);

private:
	unsigned int GetBillboardVariation();

public:
	float width = 5.f;
	float height = 5.f;
	float transparency = 1.f;
	bool oriented_to_camera;
	bool loop = false;
	bool pulse = false;
	int current_sprite_x = 0;
	int current_sprite_y = 0;
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float color_emissive[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	int emissive_intensity = 1;
private:
	GLuint shader_program;
	GLuint vbo, vao, ebo;

    uint32_t texture_uuid = 0;
	std::shared_ptr<Texture> billboard_texture = nullptr;
	uint32_t texture_emissive_uuid = 0;
	std::shared_ptr<Texture> billboard_texture_emissive = nullptr;
	
	//color

	AlignmentType alignment_type = ComponentBillboard::AlignmentType::WORLD;


	//Spritesheet params
	bool is_spritesheet = false;
	int num_sprisheet_rows = 1;
	int num_sprisheet_columns = 1;
	int num_sprites = 1;

	int time_since_start = 0;
	int animation_time = 1000;

	AnimationType animation_type = AnimationType::CONSTANT;
	bool playing = false;

	friend class PanelComponent;
	friend class PanelParticleSystem;
	friend class ComponentParticleSystem;
	
};

#endif //_COMPONENTBILLBOARD_H_