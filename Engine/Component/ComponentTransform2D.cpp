#include "ComponentTransform2D.h"

#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Helper/Utils.h"
#include <Brofiler/Brofiler.h>

ComponentTransform2D::ComponentTransform2D(GameObject * owner) : Component(owner, ComponentType::TRANSFORM2D) {

	OnTransformChange();
}

ComponentTransform2D::ComponentTransform2D() : Component(ComponentType::TRANSFORM2D)
{
}

ComponentTransform2D::ComponentTransform2D(GameObject * owner, const float2 translation, const float rotation) :
	Component(owner, ComponentType::TRANSFORM2D),
	rotation_degrees(rotation)
{
	rect.x = translation.x;
	rect.y = translation.y;
	rect.w = 0.2f;
	rect.h = 0.2f;
	OnTransformChange();
}

void ComponentTransform2D::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddBool(active, "Active");
	config.AddString("2D", "TransformType");
	config.AddRect(rect, "Rect");
	config.AddFloat(rotation_degrees, "Rotation");
}

void ComponentTransform2D::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	config.GetRect("Rect", rect, SDL_Rect());
	config.GetFloat("Rotation", rotation_degrees);
	rotation_radians = math::DegToRad(rotation_degrees);

	OnTransformChange();
}

float2 ComponentTransform2D::GetGlobalTranslation() const
{
	return float2(global_rect.x, global_rect.y);
}

float2 ComponentTransform2D::GetTranslation() const
{
	return float2(rect.x, rect.y);;
}

void ComponentTransform2D::SetTranslation(const float2 &translation)
{
	rect.x = translation.x;
	rect.y = translation.y;
	OnTransformChange();
}

void ComponentTransform2D::Translate(const float2 &translation)
{
	rect.x += translation.x;
	rect.y += translation.y;
	OnTransformChange();
}

float ComponentTransform2D::GetRotationRadiants() const
{
	return rotation_radians;
}

float ComponentTransform2D::GetRotation() const
{
	return rotation_degrees;
}

void ComponentTransform2D::SetRotation(const float &rotation)
{
	rotation_degrees = rotation;
	rotation_radians = math::DegToRad(rotation_degrees);

	OnTransformChange();
}

void ComponentTransform2D::Rotate(const float &rotation)
{
	rotation_degrees += rotation;
	rotation_radians = math::DegToRad(rotation_degrees);

	OnTransformChange();
}

float2 ComponentTransform2D::ComponentTransform2D::GetScale() const
{
	return float2(rect.w, rect.h);
}


void ComponentTransform2D::SetScale(const float2 &scale)
{
	rect.w = scale.x;
	rect.h = scale.y;
	OnTransformChange();
}

void ComponentTransform2D::OnTransformChange()
{
	//If I have a parent
	ComponentTransform2D* parent = nullptr;
	if (owner != nullptr && (parent = ((ComponentTransform2D*)owner->GetComponent(ComponentType::TRANSFORM2D))) != nullptr)
	{
		global_rect.x = rect.x + parent->global_rect.x;
		global_rect.x = rect.y + parent->global_rect.y;
		global_rect.x = rect.w + parent->global_rect.w;
		global_rect.x = rect.h + parent->global_rect.h;

		global_rotation_degrees = rotation_degrees + parent->global_rotation_degrees;
		global_rotation_radians = math::DegToRad(global_rotation_degrees);
	}
	else // I am the parent
	{
		global_rect = rect;
		global_rotation_degrees = rotation_degrees;
		global_rotation_radians = rotation_radians;
	}

	for (auto & child : owner->children)
	{
		((ComponentTransform2D*)child->GetComponent(ComponentType::TRANSFORM2D))->OnTransformChange();
	}
}