#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Globals.h"
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
	Texture(unsigned char * data, int width, int height, const std::string& path, TextureType type = TextureType::DIFUSSE);

	~Texture();

	void GenerateMipMap();
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
	void InitTexture(unsigned char * data);
	char* GLEnumToString(GLenum gl_enum) const;

public:

	std::string texture_path;
	GLuint opengl_texture = 0;
	TextureType type;

	int width = 0;
	int height = 0;
private:
	bool mip_map = false;

	GLenum wrap_s;
	GLenum wrap_t;

	GLenum min_filter;	
	GLenum mag_filter;
};

#endif //_TEXTURE_H_