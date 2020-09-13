#ifndef _FILE_H_
#define _FILE_H_

#include "Path.h"

#include <vector>
#include <memory>
#include <physfs/physfs.h>

enum class FileType
{
	ANIMATION,
	ARCHIVE,
	FONT,
	MATERIAL,
	MESH,
	META,
	MODEL,
	PREFAB,
	SCENE,
	SKELETON,
	SKYBOX,
	STATE_MACHINE,
	TEXTURE,
	SOUND,
	UNKNOWN,
	VIDEO
};


struct FileData
{
	const void* buffer; // NOTE: It's important to free this memory after usage, if not you will have a memory leak!
	unsigned int size;
};

class File
{
public:
	explicit File(Path* path); // Without explicit compiler is able to convert Paths to Files without noticing the user.
	~File();

	FileData Load() const;

	FileType GetFileType() const;
	void GetPath(Path* return_value) const;

private:
	void CalculateFileInfo();
	FileType CalculateFileType(const PHYSFS_FileType& file_type = PHYSFS_FileType::PHYSFS_FILETYPE_OTHER) const;

private:
	Path* file_path;
	FileType file_type = FileType::UNKNOWN;
};

#endif // !_FILE_H_

