#include "sme_mesh.h"

smeMesh* smeMESH_Create()
{
    smeMesh* mesh = (smeMesh*)MEM_alloc(sizeof(smeMesh));
    memset(mesh, 0, sizeof(smeMesh));
    mesh->Material = smeMESH_Material_Flat;
    M3D_setTransform(&mesh->Transform, &mesh->Translation, &mesh->Rotation);
    smeMESH_SetTranslation(mesh, FIX16(0), FIX16(0), FIX16(0));
    smeMESH_SetRotation(mesh, FIX16(0), FIX16(0), FIX16(0));
    return mesh;
}

void smeMESH_Destroy(smeMesh* mesh)
{
    if (mesh->Vertexes!=NULL)
        MEM_free(mesh->Vertexes);
    if (mesh->UVs!=NULL)
        MEM_free(mesh->UVs);
    if (mesh->Polygons!=NULL)
        MEM_free(mesh->Polygons);
    MEM_free(mesh);
}

void smeMESH_SetTranslation(smeMesh* mesh, fix16 x, fix16 y, fix16 z)
{
    M3D_setTranslation(&mesh->Transform, x, y, z);
}

void smeMESH_SetRotation(smeMesh* mesh, fix16 x, fix16 y, fix16 z)
{   
    M3D_setRotation(&mesh->Transform, x, y, z);
}
/*
const Vect3D_f16 smeMESH_CubeVertexes[8] =
{
    {FIX16(-10), FIX16(-10), FIX16(-10)},
    {FIX16(10), FIX16(-10), FIX16(-10)},
    {FIX16(10), FIX16(10), FIX16(-10)},
    {FIX16(-10), FIX16(10), FIX16(-10)},
    {FIX16(-10), FIX16(-10), FIX16(10)},
    {FIX16(10), FIX16(-10), FIX16(10)},
    {FIX16(10), FIX16(10), FIX16(10)},
    {FIX16(-10), FIX16(10), FIX16(10)}
};
const Vect2D_s16 smeMESH_CubeUVs[8] =
{
    {FIX16(0), FIX16(0)},
    {FIX16(1), FIX16(0)},
    {FIX16(1), FIX16(1)},
    {FIX16(0), FIX16(1)},
    {FIX16(0), FIX16(0)},
    {FIX16(1), FIX16(0)},
    {FIX16(1), FIX16(1)},
    {FIX16(0), FIX16(1)}
};
const u16 smeMESH_CubePolygons[36] = { 5, 4, 7, 6, 5, 7, 2, 3, 0, 1, 2, 0, 6, 7, 3, 2, 6, 3, 1, 0, 4, 5, 1, 4, 6, 2, 1, 5, 6, 1, 3, 7, 4, 0, 3, 4 };
*/

const Vect3D_f16 smeMESH_CubeVertexes[24] =
{
    {FIX16(-10), FIX16(-10), FIX16(-10)},
    {FIX16(10), FIX16(-10), FIX16(-10)},
    {FIX16(10), FIX16(10), FIX16(-10)},
    {FIX16(-10), FIX16(10), FIX16(-10)},
    
    {FIX16(-10), FIX16(10), FIX16(10)},
    {FIX16(10), FIX16(10), FIX16(10)},
    {FIX16(10), FIX16(-10), FIX16(10)},
    {FIX16(-10), FIX16(-10), FIX16(10)},
    
    {FIX16(-10), FIX16(-10), FIX16(-10)},
    {FIX16(-10), FIX16(-10), FIX16(10)},
    {FIX16(-10), FIX16(10), FIX16(10)},
    {FIX16(-10), FIX16(10), FIX16(-10)},

    {FIX16(10), FIX16(10), FIX16(-10)},
    {FIX16(10), FIX16(10), FIX16(10)},
    {FIX16(10), FIX16(-10), FIX16(10)},
    {FIX16(10), FIX16(-10), FIX16(-10)},

    {FIX16(-10), FIX16(10), FIX16(-10)},
    {FIX16(10), FIX16(10), FIX16(-10)},
    {FIX16(10), FIX16(10), FIX16(10)},
    {FIX16(-10), FIX16(10), FIX16(10)},

    {FIX16(-10), FIX16(-10), FIX16(10)},     
    {FIX16(10), FIX16(-10), FIX16(10)},
    {FIX16(10), FIX16(-10), FIX16(-10)},
    {FIX16(-10), FIX16(-10), FIX16(-10)}
};

const Vect2D_s16 smeMESH_CubeUVs[24] =
{
    {FIX16(0.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(1.0f)}, {FIX16(0.0f), FIX16(1.0f)},
    {FIX16(0.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(1.0f)}, {FIX16(0.0f), FIX16(1.0f)},
    {FIX16(0.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(1.0f)}, {FIX16(0.0f), FIX16(1.0f)},
    {FIX16(0.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(1.0f)}, {FIX16(0.0f), FIX16(1.0f)},
    {FIX16(0.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(1.0f)}, {FIX16(0.0f), FIX16(1.0f)},
    {FIX16(0.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(0.0f)}, {FIX16(1.0f), FIX16(1.0f)}, {FIX16(0.0f), FIX16(1.0f)},
};

const u16 smeMESH_CubePolygons[36] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 10, 9, 10, 8, 11,
    12, 14, 13, 14, 12, 15,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20
};


smeMesh* smeMESH_CreateCube()
{
    smeMesh* cube = smeMESH_Create();
    cube->VertexCount = 24;
    cube->Vertexes = (Vect3D_f16*)MEM_alloc(cube->VertexCount*sizeof(Vect3D_f16));
    memcpy(cube->Vertexes, smeMESH_CubeVertexes, cube->VertexCount*sizeof(Vect3D_f16));
    cube->UVs = (Vect2D_s16*)MEM_alloc(cube->VertexCount*sizeof(Vect2D_s16));
    memcpy(cube->UVs, smeMESH_CubeUVs, cube->VertexCount*sizeof(Vect2D_s16));
    cube->PolygonCount = 12;
    cube->Polygons = (u16*)MEM_alloc(cube->PolygonCount*3*sizeof(u16));
    memcpy(cube->Polygons, smeMESH_CubePolygons, cube->PolygonCount*3*sizeof(u16));
    return cube;
}
