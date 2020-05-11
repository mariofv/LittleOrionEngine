#include "ComponentBillboard.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleRender.h"

ComponentBillboard::ComponentBillboard() : Component(nullptr, ComponentType::BILLBOARD)
{
	self_timer.Start();
	this->alignment_type = ComponentBillboard::AlignmentType::VIEW_POINT;
	//owner->aabb.GenerateBoundingBox();
}

ComponentBillboard::ComponentBillboard(GameObject * _owner) : Component(owner, ComponentType::BILLBOARD)
{
	self_timer.Start();

	//owner->aabb.GenerateBoundingBox();
}

ComponentBillboard::ComponentBillboard(const std::string& texture_path, float width, float height, AlignmentType a_type) : width(width), height(height), alignment_type(a_type), Component(nullptr, ComponentType::BILLBOARD)
{
	self_timer.Start();
	billboard_texture = App->resources->Load<Texture>(texture_path.c_str());
	is_spritesheet = false;
	oriented_to_camera = true;

	if (a_type == SPRITESHEET)
		is_spritesheet = true;
	x_tiles = 6;
	y_tiles = 6;
	sheet_speed = 1;
}


ComponentBillboard::~ComponentBillboard()
{
	// TODO: Check if not deleting billboard_quad causes a memory leak.
}

void ComponentBillboard::SwitchFrame() {

	APP_LOG_INFO("%.1f", self_timer.Read());

	if (self_timer.Read() * sheet_speed >= 1)
	{
		current_sprite_x += 1;

		if ((int)current_sprite_x >= x_tiles) {
			current_sprite_y--;
			current_sprite_x = 0;
		}

		if ((int)current_sprite_y <= 0) {
			current_sprite_y = y_tiles - 1;
		}
		self_timer.Stop();
		self_timer.Start();
	}


}

void ComponentBillboard::ChangeBillboardType(ComponentBillboard::AlignmentType _alignment_type)
{
	alignment_type = _alignment_type;

	if (alignment_type == SPRITESHEET)
		is_spritesheet = true;
	else
		is_spritesheet = false;
}

void ComponentBillboard::Render(const float3& position)
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
	//GLuint hola_subroutine = glGetSubroutineIndex(shader_program, GL_VERTEX_SHADER, "hola");

	//Subroutine uniform
	int selector = glGetSubroutineUniformLocation(shader_program, GL_VERTEX_SHADER, "alignment_selector");

	switch (alignment_type) {
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
		if(oriented_to_camera)
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, n, &viewpoint_subroutine);

		else
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, n, &crossed_subroutine);

		glUniform1i(glGetUniformLocation(shader_program, "billboard.XTiles"), x_tiles);
		glUniform1i(glGetUniformLocation(shader_program, "billboard.YTiles"), y_tiles);

		glUniform1f(glGetUniformLocation(shader_program, "X"), current_sprite_x);
		glUniform1f(glGetUniformLocation(shader_program, "Y"), current_sprite_y);

		glUniform1f(glGetUniformLocation(shader_program, "billboard.speed"), sheet_speed);
		SwitchFrame();
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
	glUniform1f(glGetUniformLocation(shader_program, "billboard.isSpritesheet"), is_spritesheet);
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

	glUseProgram(0);
}

Component* ComponentBillboard::Clone(bool original_prefab) const
{
	ComponentBillboard * created_component;
	if (original_prefab)
	{
		created_component = new ComponentBillboard();
	}
	else
	{
		created_component = App->renderer->CreateComponentBillboard();
	}
	*created_component = *this;
	return created_component;
};

void ComponentBillboard::Copy(Component * component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentBillboard*>(component_to_copy) = *this;
}


void ComponentBillboard::Delete()
{
	App->renderer->RemoveComponentBillboard(this);
}

void ComponentBillboard::Save(Config& config) const
{
	/*config.AddUInt(UUID, "UUID");
	config.AddInt((unsigned int)type, "ComponentType");
	config.AddBool(active, "Active");
	config.AddFloat(sheet_speed, "SheetSpeed");
	config.AddInt(static_cast<int>(alignment_type), "BillboardType");
	config.AddString(billboard_texture->exported_file, "TexturePath");
	config.AddFloat(width, "Width");
	config.AddFloat(height, "Height");
	config.AddInt((unsigned int)x_tiles, "Rows");
	config.AddInt((unsigned int)y_tiles, "Columns");
	*/
}

void ComponentBillboard::Load(const Config& config)
{
	/*UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	sheet_speed = config.GetFloat("SheetSpeed", 1.f);
	alignment_type= static_cast<AlignmentType>(config.GetInt("BillboardType", static_cast<int>(AlignmentType::SPRITESHEET)));
	config.GetString("TexturePath", billboard_texture->exported_file, "");
	width = config.GetFloat("Width", 1.f);
	height = config.GetFloat("Height", 1.f);
	x_tiles = config.GetInt("Rows", 1.f);
	y_tiles = config.GetInt("Columns", 1.f);
	*/
}

void ComponentBillboard::ChangeTexture(std::string texture_path) {
	billboard_texture = App->resources->Load<Texture>(texture_path.c_str());
}


