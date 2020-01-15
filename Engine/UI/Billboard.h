#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include <string>
#include <MathGeoLib.h>
#include <GL/glew.h>

class Billboard
{
public:
	Billboard();
	~Billboard();

	void Render(const std::string& texture_path, const float3& position) const;

private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;

	float size = 10.f;

};

#endif //_BILLBOARD_H_