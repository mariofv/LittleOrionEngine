#include "ModulePhysics.h"

#include "Component/ComponentBoxCollider.h"
#include "Component/ComponentCapsuleCollider.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentCylinderCollider.h"
#include "Component/ComponentMeshCollider.h"
#include "Component/ComponentSphereCollider.h"
#include "EditorUI/DebugDraw.h"
#include "Event/EventManager.h"
#include "Helper/Utils.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "ModuleTime.h"
#include "ModuleDebugDraw.h"
#include "Event/EventManager.h"

#include <GL/glew.h>

#include <Brofiler/Brofiler.h>

ModulePhysics::ModulePhysics()
{
	physics_timer = new Timer();	
}

bool ModulePhysics::Init()
{
	physics_timer->Start();
	
	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
	debug_draw = new DebugDrawer();
	debug_draw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	world->setDebugDrawer(debug_draw);

	return true;
}


update_status ModulePhysics::Update()
{

	BROFILER_CATEGORY("Module Physics Update", Profiler::Color::PaleTurquoise);
	ms = physics_timer->Read();
		
	//update the world
	world->stepSimulation(App->time->delta_time, 2);
		
	for (auto collider : colliders)
	{
		
		if (App->time->isGameRunning() && collider->active_physics && collider->IsEnabled())
		{
			if (collider->collider_type != ComponentCollider::ColliderType::MESH)
			{
				collider->MoveBody();
			}
		}
		else
		{			
			collider->UpdateDimensions();			
		}

		if(collider->collider_type != ComponentCollider::ColliderType::MESH)
		{
			world->synchronizeSingleMotionState(collider->body);
		}
	}

	for(auto collider_a : colliders)
	{
		std::vector<CollisionInformation> collisions;
		for (auto collider_b : colliders)
		{
			if (collider_a != collider_b)
			{
				CollisionInformation collision_info = collider_a->DetectCollisionWith(collider_b);
				if (collision_info.collider)
				{
					collisions.push_back(collision_info);
				}
			}
		}
		App->event_manager->Publish(new CollisionEvent(collider_a->owner, collisions));
	}
	
	float ms2;
	ms2 = physics_timer->Read();

	if (ms_info.size() < 100)
	{
		ms_info.push_back(ms2 - ms);
	}
	else
	{
		rotate(ms_info.begin(), ms_info.begin() + 1, ms_info.end());
		ms_info[99] = ms2 - ms;
	}
	

	return update_status::UPDATE_CONTINUE;
}

void ModulePhysics::UpdateAllDimensions()
{
	for (auto collider : colliders)
	{
		collider->UpdateDimensions();
		

		if (collider->collider_type != ComponentCollider::ColliderType::MESH)
		{
			world->synchronizeSingleMotionState(collider->body);
		}
	}
}


void ModulePhysics::SetGravity(float3& newGravity)
{
	world->setGravity(btVector3(newGravity.x, newGravity.y, newGravity.z));
}

float3 ModulePhysics::GetGravity() const
{
	return gravity;
}

ComponentCollider* ModulePhysics::CreateComponentCollider(ComponentCollider::ColliderType collider_type, GameObject* owner)
{
	ComponentCollider* new_collider = nullptr;
	switch (collider_type)
	{
		case ComponentCollider::ColliderType::BOX:
			new_collider = new ComponentBoxCollider(owner);
			break;
		case ComponentCollider::ColliderType::CAPSULE:
			new_collider = new ComponentCapsuleCollider(owner);
			break;
		case ComponentCollider::ColliderType::SPHERE:
			new_collider = new ComponentSphereCollider(owner);
			break;
		case ComponentCollider::ColliderType::CYLINDER:
			new_collider = new ComponentCylinderCollider(owner);
			break;
		case ComponentCollider::ColliderType::MESH:
			new_collider = new ComponentMeshCollider(owner);
			break;
	}
	if (new_collider)
	{
		colliders.push_back(new_collider);
	}
	return new_collider;
}

void ModulePhysics::RemoveComponentCollider(ComponentCollider* collider_to_remove)
{
	auto it = std::find(colliders.begin(), colliders.end(), collider_to_remove);
	if (it != colliders.end())
	{
		delete *it;
		colliders.erase(it);
	}
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	
	dd::line(float3(from.getX(), from.getY(), from.getZ()), float3(to.getX(), to.getY(), to.getZ()), float3(1.0f, 0.0f, 0.0f));
	
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = btIDebugDraw::DebugDrawModes::DBG_DrawWireframe;
}

int DebugDrawer::getDebugMode() const
{
	return btIDebugDraw::DBG_DrawWireframe;
}

ComponentCollider* ModulePhysics::FindColliderByWorldId(int id)
{
	for (auto collider : colliders)
	{
		if (collider->body->getWorldArrayIndex() == id)
		{
			return collider;
		}
	}
	return nullptr;
}
