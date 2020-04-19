#ifndef _BUILDOPTIONS_H_
#define _BUILDOPTIONS_H_

#include "Helper/Config.h"

#include <string>
#include <vector>

struct Options
{
	Options() : uuid(0), library_path(""), name("") {}
	Options(uint32_t id, const std::string& path, const std::string& name) : uuid(id), library_path(path), name(name) {}

	uint32_t uuid;
	std::string library_path;
	std::string name;

	void Save(Config& config) const
	{
		config.AddUInt(uuid, "uuid");
		config.AddString(library_path, "library_path");
		config.AddString(name, "name");
	}

	void Load(Config& config)
	{
		uuid = config.GetUInt("uuid", 0);
		config.GetString("library_path", library_path, "Error");
		config.GetString("name", name, "Error");
	}
};

class BuildOptions
{
public:
	BuildOptions() = default;
	~BuildOptions() = default;

	void AddScene(uint32_t scene_uuid, const std::string& path,const std::string& name);
	void RemoveScene(unsigned position);

	bool AnySceneLoadable() const;
	void SetBuildModeActive();

	bool LoadOptions();
	bool SaveOptions() const;

	uint32_t GetSceneUUID(unsigned position) const;
	std::string GetScenePath(unsigned position) const;

private:
	std::vector<Options> build_scenes;
	bool build_mode_activated = true;

	friend class PanelBuildOptions;
};

#endif //_BUILDOPTIONS_H_