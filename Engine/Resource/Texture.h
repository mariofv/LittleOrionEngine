#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Main/Globals.h"
#include <GL/glew.h>
#include <string>

class Texture
{
public:
	enum TextureType
	{
		DIFUSSE,
		SPECULAR,
		EMISSIVE,
		OCLUSION,
		UNKNOWN
	};
	static const size_t MAX_TEXTURE_TYPES = static_cast<size_t>(TextureType::UNKNOWN);
public:
	Texture() = default;
	Texture(char * data, size_t image_size, int width, int height, const std::string& path, TextureType type = TextureType::DIFUSSE);

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
	void InitTexture(char * data);
	char* GLEnumToString(GLenum gl_enum) const;

public:

	std::string texture_path;
	GLuint opengl_texture = 0;
	TextureType type;

	int width = 0;
	int height = 0;
	size_t image_size;
private:
	bool mip_map = false;

	GLenum wrap_s;
	GLenum wrap_t;

	GLenum min_filter;	
	GLenum mag_filter;
};

#endif //_TEXTURE_H_