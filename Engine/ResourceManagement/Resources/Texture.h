#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Resource.h"
#include "ResourceManagement/Manager/TextureManager.h"
#include "ResourceManagement/Metafile/TextureMetafile.h"

#include <GL/glew.h>
#include <string>

class Metafile;
enum WrapMode;
enum FilterMode;

struct TextureLoadData
{
	std::string path;
	int width = 0;
	int height = 0;
	int num_channels = 0;
	TextureOptions texture_options;
	std::vector<char> data;
	uint32_t uuid = 0;

	//For material type enum
	unsigned texture_type = 0;
};

class Texture : public Resource
{
public:
	Texture(uint32_t uuid, char* data, size_t image_size, int width, int height, int num_channels, TextureOptions& options, bool async = false);

	~Texture();

	void SetWrap(WrapMode wrap);
	GLenum GetWrap() const;
	char* GetWrap_C_Str() const;

	void SetFilter(FilterMode filter);
	GLenum GetFilter() const;
	char* GetFilter_C_Str() const;


	void LoadInMemory();

private:
	void GenerateMipMap();
	char* GLEnumToString(GLenum gl_enum) const;

public:
	GLuint opengl_texture = 0;

	int width = 0;
	int height = 0;
	int num_channels = 0;
	TextureOptions texture_options;

private:

	GLenum wrap;
	GLenum filter;
	std::vector<char> data;

	friend class Skybox;
};

namespace ResourceManagement
{
	template<>
	static std::shared_ptr<Texture> Load(uint32_t uuid, const FileData& resource_data, bool async)
	{
		return TextureManager::Load(uuid, resource_data, async);
	}
	
}


#endif //_TEXTURE_H_