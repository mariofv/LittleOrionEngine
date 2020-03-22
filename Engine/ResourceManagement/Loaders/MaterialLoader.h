#ifndef _MATERIALLOADER_H
#define _MATERIALLOADER_H

#include <memory>
#include <string>

class Material;

class MaterialLoader
{
public:
	MaterialLoader() = default;
	~MaterialLoader() = default;
	static std::shared_ptr<Material> Load(const std::string& file_path);
};


#endif // !_MATERIALLOADER_H



