#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include <vector>

class Skybox 
{

public:
	Skybox(std::vector<std::string> faces);
	Skybox() = default;
	~Skybox() = default;
private:
	std::vector<std::string> faces;
	unsigned int skyboxTexture;
};
#endif //_SKYBOX_H_
