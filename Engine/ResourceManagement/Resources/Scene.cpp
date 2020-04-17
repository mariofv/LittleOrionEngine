#include "Scene.h"

void Scene::Load(const Config& config)
{
	scene_config = std::make_unique<Config>(config);
}
