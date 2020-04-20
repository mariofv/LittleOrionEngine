#include "Component.h"

void Component::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddString(GetComponentTypeName(type), "ComponentType");
	config.AddBool(active, "Active");

	SpecializedSave(config);
}

void Component::Load(const Config &config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
}

Component::ComponentType Component::GetComponentType(const std::string& component_type_name)
{
	if (component_type_name == "AABB")
	{
		return Component::ComponentType::AABB;
	}
	if (component_type_name == "Animation")
	{
		return Component::ComponentType::ANIMATION;
	}
	if (component_type_name == "Camera")
	{
		return Component::ComponentType::CAMERA;
	}
	if (component_type_name == "Canvas")
	{
		return Component::ComponentType::CANVAS;
	}
	if (component_type_name == "Light")
	{
		return Component::ComponentType::LIGHT;
	}
	if (component_type_name == "Mesh Renderer")
	{
		return Component::ComponentType::MESH_RENDERER;
	}
	if (component_type_name == "Transform")
	{
		return Component::ComponentType::TRANSFORM;
	}
	if (component_type_name == "Transform 2D")
	{
		return Component::ComponentType::TRANSFORM2D;
	}
	if (component_type_name == "Script")
	{
		return Component::ComponentType::SCRIPT;
	}
	if (component_type_name == "UI Button")
	{
		return Component::ComponentType::UI_BUTTON;
	}
	if (component_type_name == "UI Image")
	{
		return Component::ComponentType::UI_IMAGE;
	}
	if (component_type_name == "UI Progress Bar")
	{
		return Component::ComponentType::UI_PROGRESS_BAR;
	}
	if (component_type_name == "UI Text")
	{
		return Component::ComponentType::UI_TEXT;
	}
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

	case Component::ComponentType::UI_PROGRESS_BAR:
		return "UI Progress Bar";

	case Component::ComponentType::UI_TEXT:
		return "UI Text";
	}
}