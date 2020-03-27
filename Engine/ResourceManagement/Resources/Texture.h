#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Resource.h"

#include <GL/glew.h>
#include <string>

class Metafile;
class Skybox;

class Texture : public Resource
{
public:
	Texture(Metafile* resource_metafile, char* data, size_t image_size, int width, int height, bool normal_map = false);

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
	void LoadInMemory();
	char* GLEnumToString(GLenum gl_enum) const;

public:
	GLuint opengl_texture = 0;

	int width = 0;
	int height = 0;
	bool normal_map = false;

private:
	bool mip_map = false;

	GLenum wrap_s;
	GLenum wrap_t;

	GLenum min_filter;	
	GLenum mag_filter;
	std::vector<char> data;

	friend class Skybox;
};


#endif //_TEXTURE_H_