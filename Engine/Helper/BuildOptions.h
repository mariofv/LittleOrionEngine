#ifndef _BUILDOPTIONS_H_
#define _BUILDOPTIONS_H_

#include "Helper/Config.h"

#include <string>
#include <vector>


struct Options
{
	Options() = default;
	Options(uint32_t scene_uuid, const std::string& imported_assets_path, const std::string& exported_library_path) 
		: scene_uuid(scene_uuid), imported_assets_path(imported_assets_path), exported_library_path(exported_library_path) 
	{
		name = imported_assets_path.substr(imported_assets_path.find_last_of('/') + 1, -1);
	}

	uint32_t scene_uuid;
	std::string imported_assets_path;
	std::string exported_library_path;
	std::string name;

	void Save(Config& config) const
	{
		config.AddUInt(scene_uuid, "uuid");
		config.AddString(imported_assets_path, "assets_path");
		config.AddString(exported_library_path, "library_path");
		config.AddString(name, "name");
	}

	void Load(Config& config)
	{
		scene_uuid = static_cast<uint32_t>(config.GetUInt("uuid", 0));
		config.GetString("assets_path", imported_assets_path, "Error");
		config.GetString("library_path", exported_library_path, "Error");
		config.GetString("name", name, "Error");
	}
};

class BuildOptions
{
public:
	BuildOptions() = default;
	~BuildOptions() = default;

	void AddScene(uint32_t scene_uuid, const std::string& imported_assets_path, const std::string& exported_library_path);
	void RemoveScene(unsigned position);

	bool AnySceneLoadable() const;
	void SetBuildModeActive();

	bool LoadOptions();
	bool SaveOptions() const;

	uint32_t GetSceneUUID(unsigned position) const;
	std::string GetScenePath(unsigned position) const;
	int GetPositionFromPath(const std::string& assets_path) const;

public:
	bool is_imported = false;

private:
	std::vector<Options> build_scenes;
	bool build_mode_activated = true;

	friend class PanelBuildOptions;
};

#endif //_BUILDOPTIONS_H_