#include "ComponentCamera.h"
#include "GameObject.h"

#include "imgui.h"
#include <FontAwesome5/IconsFontAwesome5.h>

ComponentCamera::ComponentCamera(GameObject * owner) : Component(owner, ComponentType::CAMERA)
{

}

void ComponentCamera::Enable()
{

}

void ComponentCamera::Disable()
{

}

void ComponentCamera::Update()
{

}

void ComponentCamera::ShowComponentWindow()
{
	if (ImGui::CollapsingHeader(ICON_FA_VIDEO " Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
}