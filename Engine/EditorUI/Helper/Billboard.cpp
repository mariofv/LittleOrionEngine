#include "Billboard.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"

Billboard::Billboard(const std::string& texture_path, float width, float height, AlignmentType type) : width(width), height(height), type(type)
{
	self_timer.Start();
	billboard_texture = App->resources->Load<Texture>(texture_path.c_str());
	isSpritesheet = false;

	if (type == SPRITESHEET)
		isSpritesheet = true;
	XTiles = 6;
	YTiles = 6;
	sheet_speed = 30;

}


Billboard::~Billboard()
{
	// TODO: Check if not deleting billboard_quad causes a memory leak.
}

void Billboard::switchFrame() {
	
	if (self_timer.Read() * sheet_speed >= 1)
	{
		X += 1;

		if ((int)X >= XTiles) {
			Y--;
			X = 0;
		}

		if ((int)Y <= 0) {
			Y = YTiles - 1;
		}
		self_timer.Stop();
		self_timer.Start();
	}


}

void Billboard::Render(const float3& position)
{
	GLuint shader_program = App->program->GetShaderProgramId("Billboard");
	glUseProgram(shader_program);

	int n;
	glGetProgramStageiv(shader_program, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &n);
	unsigned* subroutines_indices = new unsigned[n];

	//Subroutine functions
	GLuint viewpoint_subroutine = glGetSubroutineIndex(shader_program, GL_VERTEX_SHADER, "view_point_alignment");
	GLuint crossed_subroutine = glGetSubroutineIndex(shader_program, GL_VERTEX_SHADER, "crossed_alignment");
	GLuint axial_subroutine = glGetSubroutineIndex(shader_program, GL_VERTEX_SHADER, "axial_alignment");

	GLuint hola_subroutine = glGetSubroutineIndex(shader_program, GL_VERTEX_SHADER, "hola");

	//Subroutine uniform
	int selector = glGetSubroutineUniformLocation(shader_program, GL_VERTEX_SHADER, "alignment_selector");

	switch (type) {
	case VIEW_POINT:
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, n, &viewpoint_subroutine);
		break;
	case CROSSED:
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, n, &crossed_subroutine);
		break;

	case AXIAL:
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, n, &axial_subroutine);
		break;

	case SPRITESHEET:
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, n, &crossed_subroutine);
		glUniform1i(glGetUniformLocation(shader_program, "billboard.XTiles"), XTiles);
		glUniform1i(glGetUniformLocation(shader_program, "billboard.YTiles"), YTiles);

		glUniform1f(glGetUniformLocation(shader_program, "X"), X);
		glUniform1f(glGetUniformLocation(shader_program, "Y"), Y);

		glUniform1f(glGetUniformLocation(shader_program, "billboard.speed"), sheet_speed);
		switchFrame();


		break;

	default:
		// viewpoint by default, the most consistent one
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, n, &viewpoint_subroutine);
		break;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, billboard_texture->opengl_texture);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.texture"), 0);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.width"), width);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.height"), height);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.isSpritesheet"), isSpritesheet);
	glUniform3fv(glGetUniformLocation(shader_program, "billboard.center_pos"), 1, position.ptr());

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);




	/*int b;
	glGetProgramStageiv(shader_program, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINES, &b);

	APP_LOG_INFO("%d", b);*/


	glUseProgram(0);



}

