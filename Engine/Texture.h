#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Globals.h"
#include <GL/glew.h>
#include <string>

class Texture
{
public:
	enum class TextureType
	{
		NONE,
		DIFUSSE,
		SPECULAR,
		AMBIENT,
		EMISSIVE,
		OCLUSION,
		UNKNOWN
	};

public:
	Texture() = default;
	Texture(unsigned char * data, int width, int height, const char* path, TextureType type = TextureType::DIFUSSE);

	~Texture();

	void GenerateMipMap();
	bool IsMipMapped() const;

	void SetWrapS(const GLenum wrap_s);
	GLenum GetWrapS() const;
	char* GetWrapS_C_Str() const;

	void SetWrapT(const GLenum wrap_t);
	GLenum GetWrapT() const;
	char* GetWrapT_C_Str() const;

	void SetMinFilter(const GLenum min_filter);
	GLenum GetMinFilter() const;
	char* GetMinFilter_C_Str() const;

	void SetMagFilter(const GLenum mag_filter);
	GLenum GetMagFilter() const;
	char* GetMagFilter_C_Str() const;

private:
	void InitTexture();
	char* GLEnumToString(const GLenum gl_enum) const;

public:

	std::string texture_path;
	GLuint opengl_texture = 0;
	const unsigned char* data = nullptr;
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