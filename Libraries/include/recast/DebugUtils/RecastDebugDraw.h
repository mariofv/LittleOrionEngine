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

#ifndef RECAST_DEBUGDRAW_H
#define RECAST_DEBUGDRAW_H

void duDebugDrawTriMesh(struct DuDebugDraw* dd, const float* verts, int nverts, const int* tris, const float* normals, int ntris, const unsigned char* flags, const float texScale);
void duDebugDrawTriMeshSlope(struct DuDebugDraw* dd, const float* verts, int nverts, const int* tris, const float* normals, int ntris, const float walkableSlopeAngle, const float texScale);

void duDebugDrawHeightfieldSolid(struct DuDebugDraw* dd, const struct rcHeightfield& hf);
void duDebugDrawHeightfieldWalkable(struct DuDebugDraw* dd, const struct rcHeightfield& hf);

void duDebugDrawCompactHeightfieldSolid(struct DuDebugDraw* dd, const struct rcCompactHeightfield& chf);
void duDebugDrawCompactHeightfieldRegions(struct DuDebugDraw* dd, const struct rcCompactHeightfield& chf);
void duDebugDrawCompactHeightfieldDistance(struct DuDebugDraw* dd, const struct rcCompactHeightfield& chf);

void duDebugDrawHeightfieldLayer(DuDebugDraw* dd, const struct rcHeightfieldLayer& layer, const int idx);
void duDebugDrawHeightfieldLayers(DuDebugDraw* dd, const struct rcHeightfieldLayerSet& lset);
void duDebugDrawHeightfieldLayersRegions(DuDebugDraw* dd, const struct rcHeightfieldLayerSet& lset);

void duDebugDrawRegionConnections(struct DuDebugDraw* dd, const struct rcContourSet& cset, const float alpha = 1.0f);
void duDebugDrawRawContours(struct DuDebugDraw* dd, const struct rcContourSet& cset, const float alpha = 1.0f);
void duDebugDrawContours(struct DuDebugDraw* dd, const struct rcContourSet& cset, const float alpha = 1.0f);
void duDebugDrawPolyMesh(struct DuDebugDraw* dd, const struct rcPolyMesh& mesh);
void duDebugDrawPolyMeshDetail(struct DuDebugDraw* dd, const struct rcPolyMeshDetail& dmesh);

#endif // RECAST_DEBUGDRAW_H
