#include "LOInterfaces.h"
#include "Main/Application.h"




//class GLCheckerTexture
//{
//	unsigned int m_texId;
//public:
//	GLCheckerTexture() : m_texId(0)
//	{
//	}
//
//	~GLCheckerTexture()
//	{
//		if (m_texId != 0)
//			glDeleteTextures(1, &m_texId);
//	}
//	void bind()
//	{
//		if (m_texId == 0)
//		{
//			// Create checker pattern.
//			const unsigned int col0 = duRGBA(215, 215, 215, 255);
//			const unsigned int col1 = duRGBA(255, 255, 255, 255);
//			static const int TSIZE = 64;
//			unsigned int data[TSIZE*TSIZE];
//
//			glGenTextures(1, &m_texId);
//			glBindTexture(GL_TEXTURE_2D, m_texId);
//
//			int level = 0;
//			int size = TSIZE;
//			while (size > 0)
//			{
//				for (int y = 0; y < size; ++y)
//					for (int x = 0; x < size; ++x)
//						data[x + y * size] = (x == 0 || y == 0) ? col0 : col1;
//				glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//				size /= 2;
//				level++;
//			}
//
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		}
//		else
//		{
//			glBindTexture(GL_TEXTURE_2D, m_texId);
//		}
//	}
//};
//GLCheckerTexture g_tex;


enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};

LOInterfaces::LOInterfaces()
{
	//glGenBuffers(1, &vbo);
	//glGenVertexArrays(1, &vao);
	//shader = App->program->GetShaderProgramId("Default");
}

LOInterfaces::~LOInterfaces()
{
	//glDeleteBuffers(1, &vbo);
	//glDeleteVertexArrays(1, &vao);
}

//unsigned int LOInterfaces::areaToCol(unsigned int area)
//{
//	//switch (area)
//	//{
//	//	// Ground (0) : light blue
//	//case SAMPLE_POLYAREA_GROUND: return duRGBA(0, 192, 255, 255);
//	//	// Water : blue
//	//case SAMPLE_POLYAREA_WATER: return duRGBA(0, 0, 255, 255);
//	//	// Road : brown
//	//case SAMPLE_POLYAREA_ROAD: return duRGBA(50, 20, 12, 255);
//	//	// Door : cyan
//	//case SAMPLE_POLYAREA_DOOR: return duRGBA(0, 255, 255, 255);
//	//	// Grass : green
//	//case SAMPLE_POLYAREA_GRASS: return duRGBA(0, 255, 0, 255);
//	//	// Jump : yellow
//	//case SAMPLE_POLYAREA_JUMP: return duRGBA(255, 255, 0, 255);
//	//	// Unexpected : red
//	//default: return duRGBA(255, 0, 0, 255);
//	//}
//
//	return 0;
//}
//
//void LOInterfaces::depthMask(bool state)
//{
//	//glDepthMask(state ? GL_TRUE : GL_FALSE);
//}
//
//void LOInterfaces::texture(bool state)
//{
//	//if (state)
//	//{
//	//	glEnable(GL_TEXTURE_2D);
//	//	g_tex.bind();
//	//}
//	//else
//	//{
//	//	glDisable(GL_TEXTURE_2D);
//	//}
//}
//
//void LOInterfaces::begin(duDebugDrawPrimitives prim, float size)
//{
//	//switch (prim)
//	//{
//	//case DU_DRAW_POINTS:
//	//	glPointSize(size);
//	//	glBegin(GL_POINTS);
//	//	break;
//	//case DU_DRAW_LINES:
//	//	glLineWidth(size);
//	//	glBegin(GL_LINES);
//	//	break;
//	//case DU_DRAW_TRIS:
//	//	glBegin(GL_TRIANGLES);
//	//	break;
//	//case DU_DRAW_QUADS:
//	//	glBegin(GL_QUADS);
//	//	break;
//	//};
//}
//
//
//void LOInterfaces::vertex(const float * pos, unsigned int color)
//{
//	//glColor4ubv((GLubyte*)&color);
//	//glVertex3fv(pos);
//}
//
//void LOInterfaces::vertex(const float x, const float y, const float z, unsigned int color)
//{
//	////glColor4ubv((GLubyte*)&color);
//	////glVertex3f(x, y, z);
//	//float3 vert = math::float3(x, y, z);
//	//vertices.push_back(vert);
//}
//
//void LOInterfaces::vertex(const float * pos, unsigned int color, const float * uv)
//{
//	//glColor4ubv((GLubyte*)&color);
//	//glTexCoord2fv(uv);
//	//glVertex3fv(pos);
//}
//
//void LOInterfaces::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
//{
//	//glColor4ubv((GLubyte*)&color);
//	//glTexCoord2f(u, v);
//	//glVertex3f(x, y, z);
//}
//
//void LOInterfaces::end()
//{
//	/*
//	if (vertices.size() == 0) return;
//	glBindVertexArray(vao);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)* vertices.size(), &vertices[0], GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(
//		0,  // attribute
//		3,                  // number of elements per vertex, here (x,y,z)
//		GL_FLOAT,           // the type of each element
//		GL_FALSE,           // take our values as-is
//		sizeof(float3),                  // no extra data between each position
//		(void*)0                  // offset of first element
//	);
//
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(
//		1,  // attribute
//		4,                  // number of elements per vertex, here (x,y,z)
//		GL_FLOAT,           // the type of each element
//		GL_FALSE,           // take our values as-is
//		sizeof(float3),                  // no extra data between each position
//		(void*)sizeof(math::float3)     // offset of first element
//	);
//
//	glBindVertexArray(0);
//	glDisableVertexAttribArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	*/
//
//	//glEnd();
//	//glLineWidth(1.0f);
//	//glPointSize(1.0f);
//}
