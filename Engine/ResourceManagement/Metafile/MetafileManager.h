#ifndef _METAFILEMANAGER_H_
#define _METAFILEMANAGER_H_

#include <string>
#include <unordered_map>

class Metafile;
class Path;
enum class ResourceType;

class MetafileManager
{
public:
	MetafileManager() = default;
	~MetafileManager();

	Metafile* CreateMetafile(const std::string& asset_file_path_string, ResourceType resource_type);
	Metafile* CreateMetafile(Path& asset_file_path, ResourceType resource_type, uint32_t uuid = 0);

	void SaveMetafile(Metafile* created_metafile, Path& asset_file_path) const;
	
	Metafile* GetMetafile(const Path& metafile_path);

	std::string GetMetafilePath(const Path& file_path) const;
	std::string GetMetafilePath(const std::string& file_path_string) const;

	static std::string GetMetafileExportedFolder(const Metafile& metafile);
	static std::string GetUUIDExportedFolder(uint32_t uuid);

	static std::string GetMetafileExportedFile(const Metafile& metafile);
	static std::string GetUUIDExportedFile(uint32_t uuid);

	static void TouchMetafileTimestamp(Metafile& metafile);

	/*
		A metafile is consistent when both imported path and exported path exist
	*/
	bool IsMetafileConsistent(const Path& metafile_path);
	bool IsMetafileConsistent(const Metafile& metafile);

	bool IsMetafileMoved(const Path& metafile_path);


	void RefreshMetafile(const Path& metafile_path);

private:
	std::unordered_map<std::string, Metafile*> metafiles;
	Metafile* CreateSpecializedMetafile(ResourceType resource_type) const;
};


#endif // _METAFILEMANAGER_H_

