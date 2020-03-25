#ifndef _METAFILEMANAGER_H_
#define _METAFILEMANAGER_H_

#include <string>

class Metafile;
class Path;
enum class ResourceType;

class MetafileManager
{
public:
	MetafileManager() = default;
	~MetafileManager() = default;

	static void GetMetafile(const Path& metafile_path, Metafile& metafile);
	static std::string GetMetafilePath(const Path& file_path);
	static std::string GetMetafilePath(const std::string& file_path_string);
	static std::string GetMetafileExportedFolder(const Metafile& metafile);
	static std::string GetMetafileExportedFile(const Metafile& metafile);

	static void TouchMetafileTimestamp(Metafile& metafile);

	/*
		A metafile is consistent when both imported path and exported path exist
	*/
	static bool IsMetafileConsistent(const Path& metafile_path);
	static bool IsMetafileConsistent(const Metafile& metafile);

	/*
		Cleaning inconsistencies will delete if exist imported and exported path, along the metafile
	*/
	static void DeleteMetafileInconsistencies(const Path& metafile_path);
	static void DeleteMetafileInconsistencies(const Metafile& metafile);

	static void CreateMetafile(const std::string& asset_file_path_string, ResourceType resource_type, Metafile& result_metafile);
	static void CreateMetafile(Path& asset_file_path, ResourceType resource_type, Metafile& result_metafile);
};


#endif // _METAFILEMANAGER_H_

