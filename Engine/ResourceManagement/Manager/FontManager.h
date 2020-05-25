#ifndef _FONTMANAGER_H_
#define _FONTMANAGER_H_

#include <memory>
#include <string>

#include "Filesystem/File.h"

class Font;
class Metafile;
class Path;

class FontManager
{
public:
	FontManager() = default;
	~FontManager() = default;

	static std::shared_ptr<Font> Load(uint32_t uuid, const FileData& resource_data);
};

#endif // !_FONTMANAGER_H_



