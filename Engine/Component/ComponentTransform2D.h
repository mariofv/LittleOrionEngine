#ifndef _COMPONENTTRANSFORM2D_H_
#define _COMPONENTTRANSFORM2D_H_

#include "Component.h"
#include "UI/Panel/InspectorSubpanel/PanelComponent.h"
#include "SDL/SDL.h"
#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentTransform2D : public Component
{
public:
	ComponentTransform2D(GameObject * owner);
	ComponentTransform2D();
	ComponentTransform2D(GameObject * owner, const float2 translation, const float rotation);

	~ComponentTransform2D() = default;

	void Delete() override {};

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	float2 ComponentTransform2D::GetGlobalTranslation() const;
	float2 GetTranslation() const;
	void SetTranslation(const float2 &translation);
	void Translate(const float2 &translation);

	float GetRotationRadiants() const;
	float GetRotation() const;
	void SetRotation(const float &angle);
	void Rotate(const float &rotation);

	float2 ComponentTransform2D::GetScale() const;
	void SetScale(const float2 &scale);

private:
	void OnTransformChange();

private:
	SDL_Rect global_rect = SDL_Rect();
	SDL_Rect rect = SDL_Rect();
	float rotation_degrees = 0;
	float rotation_radians = 0;	
	
	float global_rotation_degrees = 0;
	float global_rotation_radians = 0;

	friend class PanelComponent;
};

#endif //_COMPONENTTRANSFORM2D_H_