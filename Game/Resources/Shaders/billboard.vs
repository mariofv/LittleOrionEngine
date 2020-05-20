#version 430 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;

subroutine vec4 alignment_subroutine();
subroutine uniform alignment_subroutine alignment_selector;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

struct Billboard
{
  vec3 center_pos;
  float width;
  float height;
  sampler2D texture;
  bool isSpritesheet;

  vec4 color;

  int XTiles;
  int YTiles;
  float speed;
  int alignment_type;
};
uniform Billboard billboard;

out vec2 texCoord;

//Frames we receive from billboard.cpp
uniform float X;
uniform float Y;

//Frames calculations in the shader
float shader_X = 0;
float shader_Y = 0;

float U;
float V;

out vec2 frame;


void main()
{
  gl_Position = alignment_selector();

  if(billboard.isSpritesheet){
	
	shader_X = mix(X, X+1, vertex_uv0.x);
	shader_Y = mix(Y, Y+1, vertex_uv0.y);

	U = shader_X/billboard.XTiles;
	V = shader_Y/billboard.YTiles;

	frame = vec2(U, V);

	
  }

  texCoord = vertex_uv0;
}

subroutine (alignment_subroutine) vec4 view_point_alignment() //probably aligned to viewplane TODO: check it
{
	return matrices.proj*(matrices.view*vec4(billboard.center_pos,1.0) + vec4(billboard.width*vertex_position.x, billboard.height*vertex_position.y, 0.0, 0.0));
}

subroutine (alignment_subroutine) vec4 crossed_alignment()
{
	return matrices.proj*matrices.view*vec4(-billboard.width*vertex_position.x + billboard.center_pos.x, billboard.height*vertex_position.y + billboard.center_pos.y, 
											vertex_position.z + billboard.center_pos.z,1.0);
}

subroutine (alignment_subroutine) vec4 axial_alignment()
{
	return matrices.proj*(matrices.view*vec4(billboard.center_pos,1.0) + vec4(billboard.width*vertex_position.x, billboard.height*vertex_position.y, 0.0, 0.0));
}





