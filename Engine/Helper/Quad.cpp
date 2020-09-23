#include "Quad.h"

Quad::Quad(float size, float depth, float2 offset) : depth(depth), offset(offset)
{
	half_size = size * half_size;
}

void Quad::InitQuadBillboard()
{
	float vertices[20] =
	{
		half_size + offset.x,  half_size + offset.y, depth,		1.0f, 1.0f,
		half_size + offset.x, -half_size + offset.y, depth,		1.0f, 0.0f,
		-half_size + offset.x, -half_size + offset.y, depth,	0.0f, 0.0f,
		-half_size + offset.x,  half_size + offset.y, depth,	0.0f, 1.0f
	};
	unsigned int indices[6] =
	{
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Quad::InitQuadUI()
{
		GLfloat vertices[] = {
		-half_size + offset.x, half_size + offset.y, depth, 0.0f, 1.0f,
		half_size + offset.x, -half_size + offset.y, depth, 1.0f, 0.0f,
		-half_size + offset.x, -half_size + offset.y, depth, 0.0f, 0.0f,

		-half_size + offset.x, half_size + offset.y, depth, 0.0f, 1.0f,
		half_size + offset.x, half_size + offset.y, depth, 1.0f, 1.0f,
		half_size + offset.x, -half_size + offset.y, depth, 1.0f, 0.0f
	};

	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Quad::InitQuadText()
{
	GLfloat vertices[] = {
	-half_size + offset.x, half_size + offset.y, depth, 0.0f, 0.0f,
	half_size + offset.x, -half_size + offset.y, depth, 1.0f, 1.0f,
	-half_size + offset.x, -half_size + offset.y, depth, 0.0f, 1.0f,

	-half_size + offset.x, half_size + offset.y, depth, 0.0f, 0.0f,
	half_size + offset.x, half_size + offset.y, depth, 1.0f, 0.0f,
	half_size + offset.x, -half_size + offset.y, depth, 1.0f, 1.0f
	};

	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Quad::~Quad()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void Quad::RenderArray() const
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Quad::RenderElement() const
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Quad::RenderInstanced(size_t size) const
{
	glBindVertexArray(vao);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, size);
	glBindVertexArray(0);
}
