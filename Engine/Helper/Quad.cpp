#include "Quad.h"

Quad::Quad(float size, float depth)
{
	float half_size = size * 0.5f;
	GLfloat vertices[] = {
		// Pos      // Tex
		-half_size, half_size, depth, 0.0f, 1.0f,
		half_size, -half_size, depth, 1.0f, 0.0f,
		-half_size, -half_size, depth, 0.0f, 0.0f,

		-half_size, half_size, depth, 0.0f, 1.0f,
		half_size, half_size, depth, 1.0f, 1.0f,
		half_size, -half_size, depth, 1.0f, 0.0f
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
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Quad::Render() const
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
