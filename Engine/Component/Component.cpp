#include "Component.h"

void Component::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddUInt((uint64_t)type, "ComponentType");
	config.AddBool(active, "Active");

	SpecializedSave(config);
}

void Component::Load(const Config &config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);

	SpecializedLoad(config);
}

void Component::CloneBase(Component* component) const
{
	*component = *this;
}

bool Component::Is2DComponent() const
{
	return
		type == ComponentType::CANVAS
		|| type == ComponentType::UI_BUTTON
		|| type == ComponentType::UI_IMAGE
		|| type == ComponentType::UI_TEXT;
}

std::string Component::GetComponentTypeName(Component::ComponentType component_type)
{
	switch (component_type)
	{
	case Component::ComponentType::AABB:
		return "AABB";

	case Component::ComponentType::ANIMATION:
		return "Animation";

	case Component::ComponentType::CAMERA:
		return "Camera";

	case Component::ComponentType::CANVAS:
		return "Canvas";

	case Component::ComponentType::EVENT_SYSTEM:
		return "Event System";

	case Component::ComponentType::LIGHT:
		return "Light";

	case Component::ComponentType::MESH_RENDERER:
		return "Mesh Renderer";

	case Component::ComponentType::TRANSFORM:
		return "Transform";

	case Component::ComponentType::TRANSFORM2D:
		return "Transform 2D";

	case Component::ComponentType::SCRIPT:
		return "Script";

	case Component::ComponentType::UI_BUTTON:
		return "UI Button";

	case Component::ComponentType::UI_IMAGE:
		return "UI Image";

	case Component::ComponentType::UI_TEXT:
		return "UI Text";
	default:
		return "Unknown";
	}
}