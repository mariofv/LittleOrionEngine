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

#include "DebugDraw.h"
#include "DetourDebugDraw.h"
#include "Detour/DetourNavMesh.h"
#include "Detour/DetourCommon.h"
#include "Detour/DetourNode.h"


static float distancePtLine2d(const float* pt, const float* p, const float* q)
{
	float pqx = q[0] - p[0];
	float pqz = q[2] - p[2];
	float dx = pt[0] - p[0];
	float dz = pt[2] - p[2];
	float d = pqx*pqx + pqz*pqz;
	float t = pqx*dx + pqz*dz;
	if (d != 0) t /= d;
	dx = p[0] + t*pqx - pt[0];
	dz = p[2] + t*pqz - pt[2];
	return dx*dx + dz*dz;
}

static void drawPolyBoundaries(DuDebugDraw* dd, const dtMeshTile* tile,
							   const unsigned int col, const float linew,
							   bool inner)
{
	static const float thr = 0.01f*0.01f;

	dd->Begin(DU_DRAW_LINES, linew);

	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		const dtPoly* p = &tile->polys[i];
		
		if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION) continue;
		
		const dtPolyDetail* pd = &tile->detailMeshes[i];
		
		for (int j = 0, nj = (int)p->vertCount; j < nj; ++j)
		{
			unsigned int c = col;
			if (inner)
			{
				if (p->neis[j] == 0) continue;
				if (p->neis[j] & DT_EXT_LINK)
				{
					bool con = false;
					for (unsigned int k = p->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
					{
						if (tile->links[k].edge == j)
						{
							con = true;
							break;
						}
					}
					if (con)
						c = DuRGBA(255,255,255,48);
					else
						c = DuRGBA(0,0,0,48);
				}
				else
					c = DuRGBA(0,48,64,32);
			}
			else
			{
				if (p->neis[j] != 0) continue;
			}
			
			const float* v0 = &tile->verts[p->verts[j]*3];
			const float* v1 = &tile->verts[p->verts[(j+1) % nj]*3];
			
			// Draw detail mesh edges which align with the actual poly edge.
			// This is really slow.
			for (int k = 0; k < pd->triCount; ++k)
			{
				const unsigned char* t = &tile->detailTris[(pd->triBase+k)*4];
				const float* tv[3];
				for (int m = 0; m < 3; ++m)
				{
					if (t[m] < p->vertCount)
						tv[m] = &tile->verts[p->verts[t[m]]*3];
					else
						tv[m] = &tile->detailVerts[(pd->vertBase+(t[m]-p->vertCount))*3];
				}
				for (int m = 0, n = 2; m < 3; n=m++)
				{
					if ((dtGetDetailTriEdgeFlags(t[3], n) & DT_DETAIL_EDGE_BOUNDARY) == 0)
						continue;

					if (distancePtLine2d(tv[n],v0,v1) < thr &&
						distancePtLine2d(tv[m],v0,v1) < thr)
					{
						dd->Vertex(tv[n], c);
						dd->Vertex(tv[m], c);
					}
				}
			}
		}
	}
	dd->End();
}



void duDebugDrawNavMeshNodes(struct DuDebugDraw* dd, const dtNavMeshQuery& query)
{
	if (!dd) return;
	
	const dtNodePool* pool = query.getNodePool();
	if (pool)
	{
		const float off = 0.5f;
		dd->Begin(DU_DRAW_POINTS, 4.0f);
		for (int i = 0; i < pool->getHashSize(); ++i)
		{
			for (dtNodeIndex j = pool->getFirst(i); j != DT_NULL_IDX; j = pool->getNext(j))
			{
				const dtNode* node = pool->getNodeAtIdx(j+1);
				if (!node) continue;
				dd->Vertex(node->pos[0],node->pos[1]+off,node->pos[2], DuRGBA(255,192,0,255));
			}
		}
		dd->End();
		
		dd->Begin(DU_DRAW_LINES, 2.0f);
		for (int i = 0; i < pool->getHashSize(); ++i)
		{
			for (dtNodeIndex j = pool->getFirst(i); j != DT_NULL_IDX; j = pool->getNext(j))
			{
				const dtNode* node = pool->getNodeAtIdx(j+1);
				if (!node) continue;
				if (!node->pidx) continue;
				const dtNode* parent = pool->getNodeAtIdx(node->pidx);
				if (!parent) continue;
				dd->Vertex(node->pos[0],node->pos[1]+off,node->pos[2], DuRGBA(255,192,0,128));
				dd->Vertex(parent->pos[0],parent->pos[1]+off,parent->pos[2], DuRGBA(255,192,0,128));
			}
		}
		dd->End();
	}
}


static void drawMeshTileBVTree(DuDebugDraw* dd, const dtMeshTile* tile)
{
	// Draw BV nodes.
	const float cs = 1.0f / tile->header->bvQuantFactor;
	dd->Begin(DU_DRAW_LINES, 1.0f);
	for (int i = 0; i < tile->header->bvNodeCount; ++i)
	{
		const dtBVNode* n = &tile->bvTree[i];
		if (n->i < 0) // Leaf indices are positive.
			continue;
		DuAppendBoxWire(dd, tile->header->bmin[0] + n->bmin[0]*cs,
						tile->header->bmin[1] + n->bmin[1]*cs,
						tile->header->bmin[2] + n->bmin[2]*cs,
						tile->header->bmin[0] + n->bmax[0]*cs,
						tile->header->bmin[1] + n->bmax[1]*cs,
						tile->header->bmin[2] + n->bmax[2]*cs,
						DuRGBA(255,255,255,128));
	}
	dd->End();
}

void duDebugDrawNavMeshBVTree(DuDebugDraw* dd, const dtNavMesh& mesh)
{
	if (!dd) return;
	
	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile->header) continue;
		drawMeshTileBVTree(dd, tile);
	}
}

static void drawMeshTilePortal(DuDebugDraw* dd, const dtMeshTile* tile)
{
	// Draw portals
	const float padx = 0.04f;
	const float pady = tile->header->walkableClimb;

	dd->Begin(DU_DRAW_LINES, 2.0f);

	for (int side = 0; side < 8; ++side)
	{
		unsigned short m = DT_EXT_LINK | (unsigned short)side;
		
		for (int i = 0; i < tile->header->polyCount; ++i)
		{
			dtPoly* poly = &tile->polys[i];
			
			// Create new links.
			const int nv = poly->vertCount;
			for (int j = 0; j < nv; ++j)
			{
				// Skip edges which do not point to the right side.
				if (poly->neis[j] != m)
					continue;
				
				// Create new links
				const float* va = &tile->verts[poly->verts[j]*3];
				const float* vb = &tile->verts[poly->verts[(j+1) % nv]*3];
				
				if (side == 0 || side == 4)
				{
					unsigned int col = side == 0 ? DuRGBA(128,0,0,128) : DuRGBA(128,0,128,128);

					const float x = va[0] + ((side == 0) ? -padx : padx);
					
					dd->Vertex(x,va[1]-pady,va[2], col);
					dd->Vertex(x,va[1]+pady,va[2], col);

					dd->Vertex(x,va[1]+pady,va[2], col);
					dd->Vertex(x,vb[1]+pady,vb[2], col);

					dd->Vertex(x,vb[1]+pady,vb[2], col);
					dd->Vertex(x,vb[1]-pady,vb[2], col);

					dd->Vertex(x,vb[1]-pady,vb[2], col);
					dd->Vertex(x,va[1]-pady,va[2], col);
				}
				else if (side == 2 || side == 6)
				{
					unsigned int col = side == 2 ? DuRGBA(0,128,0,128) : DuRGBA(0,128,128,128);

					const float z = va[2] + ((side == 2) ? -padx : padx);
					
					dd->Vertex(va[0],va[1]-pady,z, col);
					dd->Vertex(va[0],va[1]+pady,z, col);
					
					dd->Vertex(va[0],va[1]+pady,z, col);
					dd->Vertex(vb[0],vb[1]+pady,z, col);
					
					dd->Vertex(vb[0],vb[1]+pady,z, col);
					dd->Vertex(vb[0],vb[1]-pady,z, col);
					
					dd->Vertex(vb[0],vb[1]-pady,z, col);
					dd->Vertex(va[0],va[1]-pady,z, col);
				}

			}
		}
	}
	
	dd->End();
}

void duDebugDrawNavMeshPortals(DuDebugDraw* dd, const dtNavMesh& mesh)
{
	if (!dd) return;
	
	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile->header) continue;
		drawMeshTilePortal(dd, tile);
	}
}

void duDebugDrawNavMeshPolysWithFlags(struct DuDebugDraw* dd, const dtNavMesh& mesh,
									  const unsigned short polyFlags, const unsigned int col)
{
	if (!dd) return;
	
	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile->header) continue;
		dtPolyRef base = mesh.getPolyRefBase(tile);

		for (int j = 0; j < tile->header->polyCount; ++j)
		{
			const dtPoly* p = &tile->polys[j];
			if ((p->flags & polyFlags) == 0) continue;
			duDebugDrawNavMeshPoly(dd, mesh, base|(dtPolyRef)j, col);
		}
	}
}

void duDebugDrawNavMeshPoly(DuDebugDraw* dd, const dtNavMesh& mesh, dtPolyRef ref, const unsigned int col)
{
	if (!dd) return;
	
	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	if (dtStatusFailed(mesh.getTileAndPolyByRef(ref, &tile, &poly)))
		return;
	
	dd->DepthMask(false);
	
	const unsigned int c = DuTransCol(col, 64);
	const unsigned int ip = (unsigned int)(poly - tile->polys);

	if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
	{
		dtOffMeshConnection* con = &tile->offMeshCons[ip - tile->header->offMeshBase];

		dd->Begin(DU_DRAW_LINES, 2.0f);

		// Connection arc.
		DuAppendArc(dd, con->pos[0],con->pos[1],con->pos[2], con->pos[3],con->pos[4],con->pos[5], 0.25f,
					(con->flags & 1) ? 0.6f : 0.0f, 0.6f, c);
		
		dd->End();
	}
	else
	{
		const dtPolyDetail* pd = &tile->detailMeshes[ip];

		dd->Begin(DU_DRAW_TRIS);
		for (int i = 0; i < pd->triCount; ++i)
		{
			const unsigned char* t = &tile->detailTris[(pd->triBase+i)*4];
			for (int j = 0; j < 3; ++j)
			{
				if (t[j] < poly->vertCount)
					dd->Vertex(&tile->verts[poly->verts[t[j]]*3], c);
				else
					dd->Vertex(&tile->detailVerts[(pd->vertBase+t[j]-poly->vertCount)*3], c);
			}
		}
		dd->End();
	}
	
	dd->DepthMask(true);

}

static void debugDrawTileCachePortals(struct DuDebugDraw* dd, const dtTileCacheLayer& layer, const float cs, const float ch)
{
	const int w = (int)layer.header->width;
	const int h = (int)layer.header->height;
	const float* bmin = layer.header->bmin;

	// Portals
	unsigned int pcol = DuRGBA(255,255,255,255);
	
	const int segs[4*4] = {0,0,0,1, 0,1,1,1, 1,1,1,0, 1,0,0,0};
	
	// Layer portals
	dd->Begin(DU_DRAW_LINES, 2.0f);
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			const int idx = x+y*w;
			const int lh = (int)layer.heights[idx];
			if (lh == 0xff) continue;
			
			for (int dir = 0; dir < 4; ++dir)
			{
				if (layer.cons[idx] & (1<<(dir+4)))
				{
					const int* seg = &segs[dir*4];
					const float ax = bmin[0] + (x+seg[0])*cs;
					const float ay = bmin[1] + (lh+2)*ch;
					const float az = bmin[2] + (y+seg[1])*cs;
					const float bx = bmin[0] + (x+seg[2])*cs;
					const float by = bmin[1] + (lh+2)*ch;
					const float bz = bmin[2] + (y+seg[3])*cs;
					dd->Vertex(ax, ay, az, pcol);
					dd->Vertex(bx, by, bz, pcol);
				}
			}
		}
	}
	dd->End();
}

void duDebugDrawTileCacheLayerAreas(struct DuDebugDraw* dd, const dtTileCacheLayer& layer, const float cs, const float ch)
{
	const int w = (int)layer.header->width;
	const int h = (int)layer.header->height;
	const float* bmin = layer.header->bmin;
	const float* bmax = layer.header->bmax;
	const int idx = layer.header->tlayer;
	
	unsigned int color = DuIntToCol(idx+1, 255);
	
	// Layer bounds
	float lbmin[3], lbmax[3];
	lbmin[0] = bmin[0] + layer.header->minx*cs;
	lbmin[1] = bmin[1];
	lbmin[2] = bmin[2] + layer.header->miny*cs;
	lbmax[0] = bmin[0] + (layer.header->maxx+1)*cs;
	lbmax[1] = bmax[1];
	lbmax[2] = bmin[2] + (layer.header->maxy+1)*cs;
	DuDebugDrawBoxWire(dd, lbmin[0],lbmin[1],lbmin[2], lbmax[0],lbmax[1],lbmax[2], DuTransCol(color,128), 2.0f);
	
	// Layer height
	dd->Begin(DU_DRAW_QUADS);
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			const int lidx = x+y*w;
			const int lh = (int)layer.heights[lidx];
			if (lh == 0xff) continue;

			const unsigned char area = layer.areas[lidx];
			unsigned int col;
			if (area == 63)
				col = DuLerpCol(color, DuRGBA(0,192,255,64), 32);
			else if (area == 0)
				col = DuLerpCol(color, DuRGBA(0,0,0,64), 32);
			else
				col = DuLerpCol(color, dd->AreaToCol(area), 32);
			
			const float fx = bmin[0] + x*cs;
			const float fy = bmin[1] + (lh+1)*ch;
			const float fz = bmin[2] + y*cs;
			
			dd->Vertex(fx, fy, fz, col);
			dd->Vertex(fx, fy, fz+cs, col);
			dd->Vertex(fx+cs, fy, fz+cs, col);
			dd->Vertex(fx+cs, fy, fz, col);
		}
	}
	dd->End();
	
	debugDrawTileCachePortals(dd, layer, cs, ch);
}

void duDebugDrawTileCacheLayerRegions(struct DuDebugDraw* dd, const dtTileCacheLayer& layer, const float cs, const float ch)
{
	const int w = (int)layer.header->width;
	const int h = (int)layer.header->height;
	const float* bmin = layer.header->bmin;
	const float* bmax = layer.header->bmax;
	const int idx = layer.header->tlayer;
	
	unsigned int color = DuIntToCol(idx+1, 255);
	
	// Layer bounds
	float lbmin[3], lbmax[3];
	lbmin[0] = bmin[0] + layer.header->minx*cs;
	lbmin[1] = bmin[1];
	lbmin[2] = bmin[2] + layer.header->miny*cs;
	lbmax[0] = bmin[0] + (layer.header->maxx+1)*cs;
	lbmax[1] = bmax[1];
	lbmax[2] = bmin[2] + (layer.header->maxy+1)*cs;
	DuDebugDrawBoxWire(dd, lbmin[0],lbmin[1],lbmin[2], lbmax[0],lbmax[1],lbmax[2], DuTransCol(color,128), 2.0f);
	
	// Layer height
	dd->Begin(DU_DRAW_QUADS);
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			const int lidx = x+y*w;
			const int lh = (int)layer.heights[lidx];
			if (lh == 0xff) continue;
			const unsigned char reg = layer.regs[lidx];
			
			unsigned int col = DuLerpCol(color, DuIntToCol(reg, 255), 192);
			
			const float fx = bmin[0] + x*cs;
			const float fy = bmin[1] + (lh+1)*ch;
			const float fz = bmin[2] + y*cs;
			
			dd->Vertex(fx, fy, fz, col);
			dd->Vertex(fx, fy, fz+cs, col);
			dd->Vertex(fx+cs, fy, fz+cs, col);
			dd->Vertex(fx+cs, fy, fz, col);
		}
	}
	dd->End();
	
	debugDrawTileCachePortals(dd, layer, cs, ch);
}




/*struct dtTileCacheContour
{
	int nverts;
	unsigned char* verts;
	unsigned char reg;
	unsigned char area;
};

struct dtTileCacheContourSet
{
	int nconts;
	dtTileCacheContour* conts;
};*/

void duDebugDrawTileCacheContours(DuDebugDraw* dd, const struct dtTileCacheContourSet& lcset,
								  const float* orig, const float cs, const float ch)
{
	if (!dd) return;
	
	const unsigned char a = 255;// (unsigned char)(alpha*255.0f);
	
	const int offs[2*4] = {-1,0, 0,1, 1,0, 0,-1};
	
	dd->Begin(DU_DRAW_LINES, 2.0f);
	
	for (int i = 0; i < lcset.nconts; ++i)
	{
		const dtTileCacheContour& c = lcset.conts[i];
		unsigned int color = 0;
		
		color = DuIntToCol(i, a);
		
		for (int j = 0; j < c.nverts; ++j)
		{
			const int k = (j+1) % c.nverts;
			const unsigned char* va = &c.verts[j*4];
			const unsigned char* vb = &c.verts[k*4];
			const float ax = orig[0] + va[0]*cs;
			const float ay = orig[1] + (va[1]+1+(i&1))*ch;
			const float az = orig[2] + va[2]*cs;
			const float bx = orig[0] + vb[0]*cs;
			const float by = orig[1] + (vb[1]+1+(i&1))*ch;
			const float bz = orig[2] + vb[2]*cs;
			unsigned int col = color;
			if ((va[3] & 0xf) != 0xf)
			{
				// Portal segment
				col = DuRGBA(255,255,255,128);
				int d = va[3] & 0xf;
				
				const float cx = (ax+bx)*0.5f;
				const float cy = (ay+by)*0.5f;
				const float cz = (az+bz)*0.5f;
				
				const float dx = cx + offs[d*2+0]*2*cs;
				const float dy = cy;
				const float dz = cz + offs[d*2+1]*2*cs;
				
				dd->Vertex(cx,cy,cz,DuRGBA(255,0,0,255));
				dd->Vertex(dx,dy,dz,DuRGBA(255,0,0,255));
			}
			
			DuAppendArrow(dd, ax,ay,az, bx,by,bz, 0.0f, cs*0.5f, col);
		}
	}
	dd->End();
	
	dd->Begin(DU_DRAW_POINTS, 4.0f);	
	
	for (int i = 0; i < lcset.nconts; ++i)
	{
		const dtTileCacheContour& c = lcset.conts[i];
		unsigned int color = 0;
		
		for (int j = 0; j < c.nverts; ++j)
		{
			const unsigned char* va = &c.verts[j*4];
			
			color = DuDarkenCol(DuIntToCol(i, a));
			if (va[3] & 0x80)
			{
				// Border vertex
				color = DuRGBA(255,0,0,255);
			}
			
			float fx = orig[0] + va[0]*cs;
			float fy = orig[1] + (va[1]+1+(i&1))*ch;
			float fz = orig[2] + va[2]*cs;
			dd->Vertex(fx,fy,fz, color);
		}
	}
	dd->End();
}

void duDebugDrawTileCachePolyMesh(DuDebugDraw* dd, const struct dtTileCachePolyMesh& lmesh,
								  const float* orig, const float cs, const float ch)
{
	if (!dd) return;
	
	const int nvp = lmesh.nvp;
	
	const int offs[2*4] = {-1,0, 0,1, 1,0, 0,-1};
	
	dd->Begin(DU_DRAW_TRIS);
	
	for (int i = 0; i < lmesh.npolys; ++i)
	{
		const unsigned short* p = &lmesh.polys[i*nvp*2];
		const unsigned char area = lmesh.areas[i];
		
		unsigned int color;
		if (area == DT_TILECACHE_WALKABLE_AREA)
			color = DuRGBA(0,192,255,64);
		else if (area == DT_TILECACHE_NULL_AREA)
			color = DuRGBA(0,0,0,64);
		else
			color = dd->AreaToCol(area);
		
		unsigned short vi[3];
		for (int j = 2; j < nvp; ++j)
		{
			if (p[j] == DT_TILECACHE_NULL_IDX) break;
			vi[0] = p[0];
			vi[1] = p[j-1];
			vi[2] = p[j];
			for (int k = 0; k < 3; ++k)
			{
				const unsigned short* v = &lmesh.verts[vi[k]*3];
				const float x = orig[0] + v[0]*cs;
				const float y = orig[1] + (v[1]+1)*ch;
				const float z = orig[2] + v[2]*cs;
				dd->Vertex(x,y,z, color);
			}
		}
	}
	dd->End();
	
	// Draw neighbours edges
	const unsigned int coln = DuRGBA(0,48,64,32);
	dd->Begin(DU_DRAW_LINES, 1.5f);
	for (int i = 0; i < lmesh.npolys; ++i)
	{
		const unsigned short* p = &lmesh.polys[i*nvp*2];
		for (int j = 0; j < nvp; ++j)
		{
			if (p[j] == DT_TILECACHE_NULL_IDX) break;
			if (p[nvp+j] & 0x8000) continue;
			const int nj = (j+1 >= nvp || p[j+1] == DT_TILECACHE_NULL_IDX) ? 0 : j+1; 
			int vi[2] = {p[j], p[nj]};
			
			for (int k = 0; k < 2; ++k)
			{
				const unsigned short* v = &lmesh.verts[vi[k]*3];
				const float x = orig[0] + v[0]*cs;
				const float y = orig[1] + (v[1]+1)*ch + 0.1f;
				const float z = orig[2] + v[2]*cs;
				dd->Vertex(x, y, z, coln);
			}
		}
	}
	dd->End();
	
	// Draw boundary edges
	const unsigned int colb = DuRGBA(0,48,64,220);
	dd->Begin(DU_DRAW_LINES, 2.5f);
	for (int i = 0; i < lmesh.npolys; ++i)
	{
		const unsigned short* p = &lmesh.polys[i*nvp*2];
		for (int j = 0; j < nvp; ++j)
		{
			if (p[j] == DT_TILECACHE_NULL_IDX) break;
			if ((p[nvp+j] & 0x8000) == 0) continue;
			const int nj = (j+1 >= nvp || p[j+1] == DT_TILECACHE_NULL_IDX) ? 0 : j+1; 
			int vi[2] = {p[j], p[nj]};
			
			unsigned int col = colb;
			if ((p[nvp+j] & 0xf) != 0xf)
			{
				const unsigned short* va = &lmesh.verts[vi[0]*3];
				const unsigned short* vb = &lmesh.verts[vi[1]*3];
				
				const float ax = orig[0] + va[0]*cs;
				const float ay = orig[1] + (va[1]+1+(i&1))*ch;
				const float az = orig[2] + va[2]*cs;
				const float bx = orig[0] + vb[0]*cs;
				const float by = orig[1] + (vb[1]+1+(i&1))*ch;
				const float bz = orig[2] + vb[2]*cs;
				
				const float cx = (ax+bx)*0.5f;
				const float cy = (ay+by)*0.5f;
				const float cz = (az+bz)*0.5f;
				
				int d = p[nvp+j] & 0xf;
				
				const float dx = cx + offs[d*2+0]*2*cs;
				const float dy = cy;
				const float dz = cz + offs[d*2+1]*2*cs;
				
				dd->Vertex(cx,cy,cz,DuRGBA(255,0,0,255));
				dd->Vertex(dx,dy,dz,DuRGBA(255,0,0,255));
				
				col = DuRGBA(255,255,255,128);
			}
			
			for (int k = 0; k < 2; ++k)
			{
				const unsigned short* v = &lmesh.verts[vi[k]*3];
				const float x = orig[0] + v[0]*cs;
				const float y = orig[1] + (v[1]+1)*ch + 0.1f;
				const float z = orig[2] + v[2]*cs;
				dd->Vertex(x, y, z, col);
			}
		}
	}
	dd->End();
	
	dd->Begin(DU_DRAW_POINTS, 3.0f);
	const unsigned int colv = DuRGBA(0,0,0,220);
	for (int i = 0; i < lmesh.nverts; ++i)
	{
		const unsigned short* v = &lmesh.verts[i*3];
		const float x = orig[0] + v[0]*cs;
		const float y = orig[1] + (v[1]+1)*ch + 0.1f;
		const float z = orig[2] + v[2]*cs;
		dd->Vertex(x,y,z, colv);
	}
	dd->End();
}



