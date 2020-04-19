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
	
	Metafile* GetMetafile(const Path& metafile_path);

	std::string GetMetafilePath(const Path& file_path);
	std::string GetMetafilePath(const std::string& file_path_string);
	static std::string GetMetafileExportedFolder(const Metafile& metafile);
	static std::string GetMetafileExportedFile(const Metafile& metafile);

	static void TouchMetafileTimestamp(Metafile& metafile);

	/*
		A metafile is consistent when both imported path and exported path exist
	*/
	bool IsMetafileConsistent(const Path& metafile_path);
	bool IsMetafileConsistent(const Metafile& metafile);

	/*
		Cleaning inconsistencies will delete if exist imported and exported path, along the metafile
	*/
	void DeleteMetafileInconsistencies(const Path& metafile_path);
	void DeleteMetafileInconsistencies(const Metafile& metafile);

private:
	std::unordered_map<std::string, Metafile*> metafiles;
};


#endif // _METAFILEMANAGER_H_

