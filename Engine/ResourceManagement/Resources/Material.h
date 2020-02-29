#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <vector>
#include "MathGeoLib.h"

#include <GL/glew.h>
#include "Resource.h"
#include <ResourceManagement/Loaders/MaterialLoader.h>

class Material : public Resource
{
public:



	void Save(Config& config) const override;
	void Load(const Config& config) override;

private:
	void LoadInMemory() override;

public:


private:

};

namespace Loader
{
	template<>
	static std::shared_ptr<Material> Load(const std::string& uid) {
		return MaterialLoader::Load(uid);
	}
}
#endif // !_MESH_H_

