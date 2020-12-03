#include "File.h"

#include "Log/EngineLog.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"

#include <algorithm>
#include <cctype>
#include <SDL/SDL.h>

File::File(Path* path) : file_path(path)
{
	CalculateFileInfo();
}

File::~File()
{
}

FileData File::Load() const
{
	FileData loaded_data;
	PHYSFS_File* physfs_file_handle = PHYSFS_openRead(file_path->GetFullPath().c_str());
	if (physfs_file_handle == NULL)
	{
		APP_LOG_ERROR("Error loading file %s, %s", file_path->GetFullPath().c_str(), PHYSFS_getLastError())
		loaded_data.size = 0;
		loaded_data.buffer = NULL;
		return loaded_data;

	}

	size_t res_size = static_cast<size_t>(PHYSFS_fileLength(physfs_file_handle));
	char* res = new char[res_size + 1];

	int length_read = static_cast<int>(PHYSFS_read(physfs_file_handle, res, 1, res_size));
	PHYSFS_close(physfs_file_handle);

	if (length_read != res_size)
	{
		delete[] res;
		APP_LOG_ERROR("Error loading file %s", file_path->GetFullPath().c_str())

		loaded_data.size = 0;
		loaded_data.buffer = NULL;
		return loaded_data;
	}

	res[length_read] = '\0';
	loaded_data.size = length_read;
	loaded_data.buffer = res;

	return loaded_data;
}

FileType File::GetFileType() const
{
	return file_type;
}

void File::GetPath(Path* return_value) const
{
	return_value = file_path;
}

void File::CalculateFileInfo()
{
	PHYSFS_Stat file_info;
	std::string file_path_string = file_path->GetFullPath();
	if (PHYSFS_stat(file_path_string.c_str(), &file_info) == 0)
	{
		APP_LOG_ERROR("Error getting %s file info: %s", file_path_string.c_str(), PHYSFS_getLastError())
	}
	file_type = CalculateFileType(file_info.filetype);
}

FileType File::CalculateFileType(const PHYSFS_FileType& file_type) const
{
	std::string file_extension = file_path->GetExtension();
	std::transform(file_extension.begin(), file_extension.end(), file_extension.begin(),
		[](unsigned char letter) { return std::tolower(letter); });

	if (
		file_extension == "png"
		|| file_extension == "tif"
		|| file_extension == "dds"
		|| file_extension == "tga"
		|| file_extension == "jpg"
		|| file_extension == "jfif"
		)
	{
		return FileType::TEXTURE;
	}
	if (file_extension == "fbx")
	{
		return FileType::MODEL;
	}
	if (file_extension == "prefab")
	{
		return FileType::PREFAB;
	}
	if (file_extension == "mesh")
	{
		return FileType::MESH;
	}
	if (file_extension == "mat")
	{
		return FileType::MATERIAL;
	}
	if (file_extension == "meta")
	{
		return FileType::META;
	}
	if (file_extension == "scene")
	{
		return FileType::SCENE;
	}
	if (file_extension == "sk")
	{
		return FileType::SKELETON;
	}
	if (file_extension == "skybox")
	{
		return FileType::SKYBOX;
	}
	if (file_extension == "anim")
	{
		return FileType::ANIMATION;
	}
	if (file_extension == "stm")
	{
		return FileType::STATE_MACHINE;
	}
	if (file_extension == "ttf")
	{
		return FileType::FONT;
	}
	if (file_extension == "bnk")
	{
		return FileType::SOUND;
	}
	if (file_extension == "mp4")
	{
		return FileType::VIDEO;
	}
	if (file_extension == "" && PHYSFS_FileType::PHYSFS_FILETYPE_OTHER == file_type)
	{
		return FileType::ARCHIVE;
	}
	return FileType::UNKNOWN;
}