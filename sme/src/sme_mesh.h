#ifndef _SME_MESH_H_
#define _SME_MESH_H_

#include "genesis.h"

typedef enum
{
    smeMESH_Material_Dot,
    smeMESH_Material_Wire,
    smeMESH_Material_Flat,
    smeMESH_Material_Gouraud,
    smeMESH_Material_TexMap    
} smeMESH_Material;

typedef struct
{
    Vect3D_f16* Vertexes;
    Vect2D_s16* UVs;
    u16 VertexCount;
    u16* Polygons;
    u16 PolygonCount;
    Transformation3D Transform;
    Translation3D Translation;
    Rotation3D Rotation;
    smeMESH_Material Material;
    u8* TexMap;
} smeMesh;

smeMesh* smeMESH_Create();
void smeMESH_Destroy(smeMesh* mesh);
void smeMESH_SetTranslation(smeMesh* mesh, fix16 x, fix16 y, fix16 z);
void smeMESH_SetRotation(smeMesh* mesh, fix16 x, fix16 y, fix16 z);

smeMesh* smeMESH_CreateCube();

#endif
