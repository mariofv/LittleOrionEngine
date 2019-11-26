#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"


GameObject::GameObject(const std::string name, const GameObject &parent) :
	name(name)
{
	*this->parent = parent;
}


GameObject::~GameObject()
{
}

void GameObject::Update()
{

}

Component* GameObject::CreateComponent(const Component::ComponentType type)
{
	switch (type)
	{
	case Component::ComponentType::MATERIAL:
		return new ComponentMaterial();
		break;
	case Component::ComponentType::MESH:
		return new ComponentMesh();
		break;
	case Component::ComponentType::TRANSFORM:
		return new ComponentTransform();
		break;
	default:
		APP_LOG_ERROR("Error creating component. Incorrect component type.");
		return nullptr;
	}
}

