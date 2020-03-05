//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef _DUDEBUGDRAW_H
#define _DUDEBUGDRAW_H

// Some math headers don't have PI defined.
static const float DU_PI = 3.14159265f;

enum DuDebugDrawPrimitives
{
	DU_DRAW_POINTS,
	DU_DRAW_LINES,
	DU_DRAW_TRIS,
	DU_DRAW_QUADS,	
};

/// Abstract debug draw interface.
struct DuDebugDraw
{
	virtual ~DuDebugDraw() = 0;
	
	virtual void DepthMask(bool state) = 0;

	virtual void Texture(bool state) = 0;

	/// Begin drawing primitives.
	///  @param prim [in] primitive type to draw, one of rcDebugDrawPrimitives.
	///  @param size [in] size of a primitive, applies to point size and line width only.
	virtual void Begin(DuDebugDrawPrimitives prim, float size = 1.0f) = 0;

	/// Submit a vertex
	///  @param pos [in] position of the verts.
	///  @param color [in] color of the verts.
	virtual void Vertex(const float* pos, unsigned int color) = 0;

	/// Submit a vertex
	///  @param x,y,z [in] position of the verts.
	///  @param color [in] color of the verts.
	virtual void Vertex(const float x, const float y, const float z, unsigned int color) = 0;

	/// Submit a vertex
	///  @param pos [in] position of the verts.
	///  @param color [in] color of the verts.
	virtual void Vertex(const float* pos, unsigned int color, const float* uv) = 0;
	
	/// Submit a vertex
	///  @param x,y,z [in] position of the verts.
	///  @param color [in] color of the verts.
	virtual void Vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v) = 0;
	
	/// End drawing primitives.
	virtual void End() = 0;

	/// Compute a color for given area.
	virtual unsigned int AreaToCol(unsigned int area);
};

inline unsigned int DuRGBA(int r, int g, int b, int a)
{
	return ((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16) | ((unsigned int)a << 24);
}

inline unsigned int DuRGBAf(float fr, float fg, float fb, float fa)
{
	unsigned char r = (unsigned char)(fr*255.0f);
	unsigned char g = (unsigned char)(fg*255.0f);
	unsigned char b = (unsigned char)(fb*255.0f);
	unsigned char a = (unsigned char)(fa*255.0f);
	return DuRGBA(r,g,b,a);
}

unsigned int DuIntToCol(int i, int a);
void DuIntToCol(int i, float* col);

inline unsigned int DuMultCol(const unsigned int col, const unsigned int d)
{
	const unsigned int r = col & 0xff;
	const unsigned int g = (col >> 8) & 0xff;
	const unsigned int b = (col >> 16) & 0xff;
	const unsigned int a = (col >> 24) & 0xff;
	return DuRGBA((r*d) >> 8, (g*d) >> 8, (b*d) >> 8, a);
}

inline unsigned int DuDarkenCol(unsigned int col)
{
	return ((col >> 1) & 0x007f7f7f) | (col & 0xff000000);
}

inline unsigned int DuLerpCol(unsigned int ca, unsigned int cb, unsigned int u)
{
	const unsigned int ra = ca & 0xff;
	const unsigned int ga = (ca >> 8) & 0xff;
	const unsigned int ba = (ca >> 16) & 0xff;
	const unsigned int aa = (ca >> 24) & 0xff;
	const unsigned int rb = cb & 0xff;
	const unsigned int gb = (cb >> 8) & 0xff;
	const unsigned int bb = (cb >> 16) & 0xff;
	const unsigned int ab = (cb >> 24) & 0xff;
	
	unsigned int r = (ra*(255-u) + rb*u)/255;
	unsigned int g = (ga*(255-u) + gb*u)/255;
	unsigned int b = (ba*(255-u) + bb*u)/255;
	unsigned int a = (aa*(255-u) + ab*u)/255;
	return DuRGBA(r,g,b,a);
}

inline unsigned int DuTransCol(unsigned int c, unsigned int a)
{
	return (a<<24) | (c & 0x00ffffff);
}

void DuCalcBoxColors(unsigned int* colors, unsigned int colTop, unsigned int colSide);

void DuDebugDrawCylinderWire(struct DuDebugDraw* dd, float minx, float miny, float minz,
							 float maxx, float maxy, float maxz, unsigned int col, const float lineWidth);

void DuDebugDrawBoxWire(struct DuDebugDraw* dd, float minx, float miny, float minz,
						float maxx, float maxy, float maxz, unsigned int col, const float lineWidth);

void DuDebugDrawArc(struct DuDebugDraw* dd, const float x0, const float y0, const float z0,
					const float x1, const float y1, const float z1, const float h,
					const float as0, const float as1, unsigned int col, const float lineWidth);

void DuDebugDrawArrow(struct DuDebugDraw* dd, const float x0, const float y0, const float z0,
					  const float x1, const float y1, const float z1,
					  const float as0, const float as1, unsigned int col, const float lineWidth);

void DuDebugDrawCircle(struct DuDebugDraw* dd, const float x, const float y, const float z,
					   const float r, unsigned int col, const float lineWidth);

void DuDebugDrawCross(struct DuDebugDraw* dd, const float x, const float y, const float z,
					  const float size, unsigned int col, const float lineWidth);

void DuDebugDrawBox(struct DuDebugDraw* dd, float minx, float miny, float minz,
					float maxx, float maxy, float maxz, const unsigned int* fcol);

void DuDebugDrawCylinder(struct DuDebugDraw* dd, float minx, float miny, float minz,
						 float maxx, float maxy, float maxz, unsigned int col);

void DuDebugDrawGridXZ(struct DuDebugDraw* dd, const float ox, const float oy, const float oz,
					   const int w, const int h, const float size,
					   const unsigned int col, const float lineWidth);


// Versions without begin/end, can be used to draw multiple primitives.
void DuAppendCylinderWire(struct DuDebugDraw* dd, float minx, float miny, float minz,
						  float maxx, float maxy, float maxz, unsigned int col);

void DuAppendBoxWire(struct DuDebugDraw* dd, float minx, float miny, float minz,
					 float maxx, float maxy, float maxz, unsigned int col);

void DuAppendBoxPoints(struct DuDebugDraw* dd, float minx, float miny, float minz,
					   float maxx, float maxy, float maxz, unsigned int col);

void DuAppendArc(struct DuDebugDraw* dd, const float x0, const float y0, const float z0,
				 const float x1, const float y1, const float z1, const float h,
				 const float as0, const float as1, unsigned int col);

void DuAppendArrow(struct DuDebugDraw* dd, const float x0, const float y0, const float z0,
				   const float x1, const float y1, const float z1,
				   const float as0, const float as1, unsigned int col);

void DuAppendCircle(struct DuDebugDraw* dd, const float x, const float y, const float z,
					const float r, unsigned int col);

void DuAppendCross(struct DuDebugDraw* dd, const float x, const float y, const float z,
				   const float size, unsigned int col);

void DuAppendBox(struct DuDebugDraw* dd, float minx, float miny, float minz,
				 float maxx, float maxy, float maxz, const unsigned int* fcol);

void DuAppendCylinder(struct DuDebugDraw* dd, float minx, float miny, float minz,
					  float maxx, float maxy, float maxz, unsigned int col);


class DuDisplayList : public DuDebugDraw
{
	float* m_pos;
	unsigned int* m_color;
	int m_size;
	int m_cap;

	bool m_depthMask;
	DuDebugDrawPrimitives m_prim;
	float m_primSize;
	void resize(int cap);
	
public:
	DuDisplayList(int cap = 512);
	~DuDisplayList();
	virtual void DepthMask(bool state);
	virtual void Begin(DuDebugDrawPrimitives prim, float size = 1.0f);
	virtual void Vertex(const float x, const float y, const float z, unsigned int color);
	virtual void Vertex(const float* pos, unsigned int color);
	virtual void End();
	void clear();
	void draw(struct DuDebugDraw* dd);

private:
	// Explicitly disabled copy constructor and copy assignment operator.
	DuDisplayList(const DuDisplayList&);
	DuDisplayList& operator=(const DuDisplayList&);
};


#endif // _DUDEBUGDRAW_H
