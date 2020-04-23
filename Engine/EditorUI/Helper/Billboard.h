#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include "ResourceManagement/Resources/Mesh.h"
#include "ResourceManagement/Resources/Texture.h"

#include <string>
#include <MathGeoLib.h>
#include <GL/glew.h>
#include "Helper/Timer.h"


class Billboard
{
public:

	enum AlignmentType {
		SCREEN,		// TODO: Parallel to the screen normal: UI, text, ...
		VIEW_POINT,	// Alligned to the camera position
		AXIAL,		// Rotates over Y axis
		CROSSED,		// Billboards crossed between them
		SPRITESHEET
	};

	Billboard(const std::string& texture_path, float width, float height, AlignmentType type);
	~Billboard();

	void Render(const float3& position);
	void switchFrame();

	AlignmentType type;

	//Spritesheet params
	int XTiles = 0;
	int YTiles = 0;
	float X = 0, Y = 0;

	float cell_size_x = 0;
	float cell_Size_y = 0;

	Timer bbtimer;

	bool top_down = false;
	
	float sheet_speed = 0;

private:
	std::shared_ptr<Mesh> billboard_quad;
	bool isSpritesheet;
	float width = 5.f;
	float height = 5.f;
	std::shared_ptr<Texture> billboard_texture = nullptr;

	float vertices[20] = {
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,	// bottom right
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,	// bottom left
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f	// top left 
	};
	unsigned int indices[6] = { 
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	unsigned int EBO;

	int innerCount = 0;

};

#endif //_BILLBOARD_H_