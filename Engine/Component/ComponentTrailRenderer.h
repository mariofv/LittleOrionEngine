#ifndef _COMPONENTTRAILRENDERER_H_
#define _COMPONENTTRAILRENDERER_H_

#include "Component.h"
#include "Component/ComponentAABB.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"
#include "EditorUI/Panel/PanelScene.h"

#include "MathGeoLib.h"
#include <GL/glew.h>

class GameObject;
class Texture;

struct Vertex
{
	float3 position;
};

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

	void Render(std::vector<Vertex>& to_render);
	//void SwitchFrame();

	void ChangeTexture(uint32_t texture_uuid);

public:
	float* trail_renderer_vertices = nullptr;
	float* trail_renderer_uvs = nullptr;
	unsigned int rendered_vertices = 0;

private:
	uint32_t texture_uuid = 0;
	std::shared_ptr<Texture> trail_texture = nullptr;
	float2 tex_uv = {0.0f, 0.0f};
	//color
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	unsigned int trail_vao, trail_vbo;

	friend class PanelComponent;
	friend class ComponentParticleSystem;
	friend class ComponentTrail;
};

#endif //_COMPONENTTRAILRENDERER_H_