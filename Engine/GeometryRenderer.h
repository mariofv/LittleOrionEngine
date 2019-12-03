#ifndef _GEOMETRYRENDERER_H_
#define _GEOMETRYRENDERER_H_

#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentCamera;

class GeometryRenderer
{
	class Geometry
	{
		public:
			Geometry()
			{
				glGenVertexArrays(1, &vao);
				glGenBuffers(1, &vbo);
				glGenBuffers(1, &ebo);
			};

			~Geometry()
			{
				glDeleteBuffers(1, &vbo);
				glDeleteBuffers(1, &ebo);
				glDeleteVertexArrays(1, &vao);
			}

		public:
			int num_indices;
			GLuint vao = 0;
			GLuint vbo = 0;
			GLuint ebo = 0;
	};


public:
	GeometryRenderer();
	~GeometryRenderer();

	void InitHexahedron();

	void RenderGeometry(const ComponentCamera &camera, const GeometryRenderer::Geometry &geometry) const;
	void RenderHexahedron(const ComponentCamera &camera, const std::vector<float> &vertices);

private:
	Geometry *hexahedron = nullptr; // Cube like shapes

};

#endif // _GEOMETRYRENDERER_H_