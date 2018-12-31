#ifndef _SME_SCENE_H_
#define _SME_SCENE_H_

#include "sme_mesh.h"

#define smeSCENE_MAX_MESHES       4
#define smeSCENE_MAX_VERTEXES     600
#define smeSCENE_MAX_POLYGONS     300

typedef struct
{
    smeMesh* Meshes[smeSCENE_MAX_MESHES];
    int MeshCount;
    Vect3D_f16 VertexesWS[smeSCENE_MAX_VERTEXES];   // World space vertexes
    Vect2D_s16 VertexesVS[smeSCENE_MAX_VERTEXES];   // Viewport space vertexes
    u16 Polygons[smeSCENE_MAX_POLYGONS*3];
    u8 PolygonMeshes[smeSCENE_MAX_POLYGONS];
    u16 PolygonIndexes[smeSCENE_MAX_POLYGONS];
    u16 PolygonDistances[smeSCENE_MAX_POLYGONS];
    int MinimumDistance;
    int MaximumDistance;
} smeScene;

extern smeScene smeSCENE;

void smeSCENE_Create();
void smeSCENE_Destroy();
void smeSCENE_Update();

u16 smeSCENE_Add(smeMesh* mesh);
void smeSCENE_Remove(u16 mesh);

#endif
