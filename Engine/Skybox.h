#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include <vector>
#include <GL/glew.h>

class Skybox 
{

public:
	Skybox();
	~Skybox();

private:
	GLuint skybox_texture;
};
#endif //_SKYBOX_H_
