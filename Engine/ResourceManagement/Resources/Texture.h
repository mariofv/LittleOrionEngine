#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Main/Globals.h"
#include <GL/glew.h>
#include <string>
#include "Resource.h"
#include <ResourceManagement/Loaders/TextureLoader.h>
class Texture : public Resource
{
public:

	enum TextureType
	{
		DIFUSSE,
		SPECULAR,
		EMISSIVE,
		OCLUSION,
		NORMAL,
		UNKNOWN
	};
	static const size_t MAX_TEXTURE_TYPES = static_cast<size_t>(TextureType::UNKNOWN);
public:
	Texture(char * data, size_t image_size, int width, int height, const std::string& path, bool normal_map = false,TextureType type = TextureType::DIFUSSE);

	Texture(char * data, size_t image_size, int width, int height, const std::string& path, bool normal_map = false);


	~Texture();

	bool IsMipMapped() const;

	void SetWrapS(GLenum wrap_s);
	GLenum GetWrapS() const;
	char* GetWrapS_C_Str() const;

	void SetWrapT(GLenum wrap_t);
	GLenum GetWrapT() const;
	char* GetWrapT_C_Str() const;

	void SetMinFilter(GLenum min_filter);
	GLenum GetMinFilter() const;
	char* GetMinFilter_C_Str() const;

	void SetMagFilter(GLenum mag_filter);
	GLenum GetMagFilter() const;
	char* GetMagFilter_C_Str() const;


private:
	void GenerateMipMap();
	void LoadInMemory() override;
	char* GLEnumToString(GLenum gl_enum) const;

public:

	GLuint opengl_texture = 0;

	int width = 0;
	int height = 0;
	size_t image_size;
	bool normal_map = false;
private:
	bool mip_map = false;

	GLenum wrap_s;
	GLenum wrap_t;

	GLenum min_filter;	
	GLenum mag_filter;
	char * data;
};


namespace Loader
{
	template<>
	static std::shared_ptr<Texture> Load(const std::string& uid) {
		return TextureLoader::Load(uid);
	}
}

#endif //_TEXTURE_H_