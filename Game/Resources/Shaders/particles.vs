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
  bool isSpritesheet;
  int XTiles;
  int YTiles;

  sampler2D texture;
};
uniform Billboard billboard;

out vec2 texCoord;
out vec2 frame;
out vec4 color;

struct Particle
{
	vec4 position;
	vec4 color;
	float width;
	float height;

	float X;
    float Y;
};


layout(std430, binding = 2) buffer particles_data
{
    Particle particles[];
};


void main()
{
	gl_Position = alignment_selector();

	 if(billboard.isSpritesheet){
	
		float shader_X = mix(particles[gl_InstanceID].X, particles[gl_InstanceID].X+1, vertex_uv0.x);
		float shader_Y = mix(particles[gl_InstanceID].Y, particles[gl_InstanceID].Y+1, vertex_uv0.y);

		float U = shader_X/billboard.XTiles;
		float V = shader_Y/billboard.YTiles;

		frame = vec2(U, V);
	}
	texCoord = vertex_uv0; 
	color = particles[gl_InstanceID].color;
}  


subroutine (alignment_subroutine) vec4 view_point_alignment() //probably aligned to viewplane TODO: check it
{
	return matrices.proj*(matrices.view*vec4(particles[gl_InstanceID].position.rgb,1.0) + vec4(particles[gl_InstanceID].width*vertex_position.x, particles[gl_InstanceID].height*vertex_position.y, 0.0, 0.0));
}

subroutine (alignment_subroutine) vec4 crossed_alignment()
{
	return matrices.proj*matrices.view*vec4(-particles[gl_InstanceID].width*vertex_position.x + particles[gl_InstanceID].position.rgb.x, particles[gl_InstanceID].height*vertex_position.y + particles[gl_InstanceID].position.rgb.y, 
											vertex_position.z + particles[gl_InstanceID].position.rgb.z,1.0);
}

subroutine (alignment_subroutine) vec4 axial_alignment()
{
	return matrices.proj*(matrices.view*vec4(particles[gl_InstanceID].position.rgb,1.0) + vec4(particles[gl_InstanceID].width*vertex_position.x, particles[gl_InstanceID].height*vertex_position.y, 0.0, 0.0));
}


