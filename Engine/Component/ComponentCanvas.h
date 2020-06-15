#ifndef _COMPONENTCANVAS_H_
#define _COMPONENTCANVAS_H_

#include "Component/ComponentCamera.h"

#include <GL/glew.h>
#include <MathGeoLib.h>

class ComponentButton;
class ComponentCanvasRenderer;

class ComponentCanvas : public Component
{
public:
	ComponentCanvas();
	ComponentCanvas(GameObject * owner);
	~ComponentCanvas() = default;

	ComponentCanvas & operator=(const ComponentCanvas& component_to_copy);
	ComponentCanvas & operator=(ComponentCanvas&& component_to_move) = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;
	void Delete() override;

	void Render(bool scene_mode = false);
	void RenderGameObject(GameObject* owner, float4x4* projection) const;

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	float2 GetCanvasScreenPosition() const;
	ENGINE_API float2 GetCanvasScreenSize() const;

	bool IsFocused() const;
	ComponentButton* GetUIElementAtPosition(float2 mouse_position);

private:
	std::vector<ComponentCanvasRenderer*> GetComponentCanvasRendererToRender() const;

	float2 canvas_screen_size = float2::zero;
	float2 canvas_screen_position = float2::zero;

	bool focused = false;
};

#endif //_COMPONENTCANVAS_H_

