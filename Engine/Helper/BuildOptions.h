#ifndef _BUILDOPTIONS_H_
#define _BUILDOPTIONS_H_

#include "Helper/Config.h"

#include <string>
#include <vector>

struct Options
{
	Options() : uuid(0), library_path("") {}
	Options(uint32_t id, const std::string& path) : uuid(id), library_path(path) {}

	uint32_t uuid;
	std::string library_path;

	void Save(Config config) const
	{
		config.AddUInt(uuid, "uuid");
		config.AddString(library_path, "library_path");
	}

	void Load(Config config)
	{
		uuid = config.GetUInt("uuid", 0);
		config.GetString("library_path", library_path, "Error");
	}
};

class BuildOptions
{
public:
	BuildOptions() = default;
	~BuildOptions() = default;

	void AddScene(uint32_t scene_uuid, const std::string& path);
	void RemoveScene(unsigned position);

	bool AnySceneLoadable() const;
	void SetBuildModeActive();

	bool LoadOptions();
	bool SaveOptions() const;

private:
	std::vector<Options> build_scenes;
	bool build_mode_activated = true;

};

#endif //_BUILDOPTIONS_H_