#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include <vector>
#include <GL/glew.h>

class ComponentCamera;
class Skybox 
{

public:
	Skybox();
	~Skybox();

	void LoadDefaultSkybox();
	void GenerateSkyboxCube();
	void Render(const ComponentCamera & camera) const;
private:
	GLuint skybox_texture;
	unsigned int skybox_VAO, skybox_VBO;


};


#endif //_SKYBOX_H_
