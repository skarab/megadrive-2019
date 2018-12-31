#include "sme_scene.h"
#include "sme_raster.h"

smeScene smeSCENE;

void smeSCENE_Create()
{
    memset(smeSCENE.Meshes, 0, sizeof(smeMesh*)*smeSCENE_MAX_MESHES);
    smeSCENE.MeshCount = 0;
}

void smeSCENE_Destroy()
{
    memset(smeSCENE.Meshes, 0, sizeof(smeMesh*)*smeSCENE_MAX_MESHES);
    smeSCENE.MeshCount = 0;
}

void smeSCENE_QuickSort(int deb, int fin)
{
    int i=deb;
    int j=fin;
    int milieu=smeSCENE.PolygonDistances[smeSCENE.PolygonIndexes[(deb+fin)/2]];
    int temp;

    while (i<=j)
    {
        while (smeSCENE.PolygonDistances[smeSCENE.PolygonIndexes[i]]>milieu && i<j) i++;
        while (smeSCENE.PolygonDistances[smeSCENE.PolygonIndexes[j]]<milieu && j>0) j--;

        if (i<=j)
        {
            temp = smeSCENE.PolygonIndexes[i];
            smeSCENE.PolygonIndexes[i]=smeSCENE.PolygonIndexes[j];
            smeSCENE.PolygonIndexes[j]=temp;
            ++i;
            --j;
        }
    }

    if (i<fin) smeSCENE_QuickSort(i,fin);
    if (deb<j) smeSCENE_QuickSort(deb,j);
}

inline u8 smeSCENE_GetColor(int distance)
{
    u8 color;
    if (smeSCENE.MaximumDistance-smeSCENE.MinimumDistance==0
        || distance>smeSCENE.MaximumDistance)
    {
        color = 0;
    }
    else if (distance<smeSCENE.MinimumDistance)
    {
        return 15;
    }
    else
    {
        int depth = (distance-smeSCENE.MinimumDistance)*15/(smeSCENE.MaximumDistance-smeSCENE.MinimumDistance);
        color = 15-(depth&15);
    }
    return color;
}

inline u8 smeSCENE_GetPolygonColor(u16 index)
{
    u8 color = smeSCENE_GetColor(smeSCENE.PolygonDistances[index]);
    return color|(color<<4);
}

inline u8 smeSCENE_GetVertexColor(u16 index)
{
    return smeSCENE_GetColor(smeSCENE.VertexesWS[index].z);
}

void smeSCENE_Update()
{
    u16 vertex_count = 0;
    u16 polygon_count = 0;
    u16 i,j;
    smeSCENE.MinimumDistance = 99999;
    smeSCENE.MaximumDistance = -99999;
    for (i=0 ; i<smeSCENE.MeshCount ; ++i)
    {
        if (vertex_count+smeSCENE.Meshes[i]->VertexCount<smeSCENE_MAX_VERTEXES)
        {
            smeSCENE.Meshes[i]->Transform.rebuildMat = 1;
            M3D_transform(&smeSCENE.Meshes[i]->Transform, smeSCENE.Meshes[i]->Vertexes, smeSCENE.VertexesWS+vertex_count, smeSCENE.Meshes[i]->VertexCount);
             
            for (j=0 ; j<smeSCENE.Meshes[i]->PolygonCount ; ++j)
            {
                smeSCENE.PolygonMeshes[polygon_count] = i;
                smeSCENE.Polygons[polygon_count*3+0] = smeSCENE.Meshes[i]->Polygons[j*3+0]+vertex_count;
                smeSCENE.Polygons[polygon_count*3+1] = smeSCENE.Meshes[i]->Polygons[j*3+1]+vertex_count;
                smeSCENE.Polygons[polygon_count*3+2] = smeSCENE.Meshes[i]->Polygons[j*3+2]+vertex_count;
                
                smeSCENE.PolygonDistances[polygon_count] = smeSCENE.VertexesWS[smeSCENE.Polygons[polygon_count*3+0]].z/3
                    + smeSCENE.VertexesWS[smeSCENE.Polygons[polygon_count*3+1]].z/3
                    + smeSCENE.VertexesWS[smeSCENE.Polygons[polygon_count*3+2]].z/3;
                
                if (smeSCENE.PolygonDistances[polygon_count]<smeSCENE.MinimumDistance)
                    smeSCENE.MinimumDistance = smeSCENE.PolygonDistances[polygon_count];
                    
                if (smeSCENE.PolygonDistances[polygon_count]>smeSCENE.MaximumDistance)
                    smeSCENE.MaximumDistance = smeSCENE.PolygonDistances[polygon_count];
                
                smeSCENE.PolygonIndexes[polygon_count] = polygon_count;                
                ++polygon_count;
            }
            vertex_count += smeSCENE.Meshes[i]->VertexCount;
        }
    }
    
    smeSCENE_QuickSort(0, polygon_count-1);
            
    M3D_project_s16(smeSCENE.VertexesWS, smeSCENE.VertexesVS, vertex_count);
    
    for (i=0 ; i<polygon_count ; ++i)
    {
        u16 iv[3];
        u16 sf = smeSCENE.PolygonIndexes[i];
        iv[0] = smeSCENE.Polygons[sf*3+0];
        iv[1] = smeSCENE.Polygons[sf*3+1];
        iv[2] = smeSCENE.Polygons[sf*3+2];
                  
        if (smeSCENE.VertexesWS[iv[0]].z<=0 || smeSCENE.VertexesWS[iv[1]].z<=0 || smeSCENE.VertexesWS[iv[2]].z<=0)
            continue;
        
        switch (smeSCENE.Meshes[smeSCENE.PolygonMeshes[sf]]->Material)
        {
            case smeMESH_Material_Dot:
            {
                smeRASTER_PolygonDot(smeSCENE.VertexesVS+iv[0], smeSCENE.VertexesVS+iv[1], smeSCENE.VertexesVS+iv[2], smeSCENE_GetPolygonColor(sf));
            } break;
            
            case smeMESH_Material_Wire:
            {
                smeRASTER_PolygonWire(smeSCENE.VertexesVS+iv[0], smeSCENE.VertexesVS+iv[1], smeSCENE.VertexesVS+iv[2], smeSCENE_GetPolygonColor(sf));
            } break;
            
            case smeMESH_Material_Flat:
            {                
                smeRASTER_PolygonFlat(smeSCENE.VertexesVS+iv[0], smeSCENE.VertexesVS+iv[1], smeSCENE.VertexesVS+iv[2], smeSCENE_GetPolygonColor(sf));
            } break;
            
            case smeMESH_Material_Gouraud:
            {                
                smeRASTER_PolygonGouraud(
                    smeSCENE.VertexesVS+iv[0], smeSCENE_GetVertexColor(iv[0]),
                    smeSCENE.VertexesVS+iv[1], smeSCENE_GetVertexColor(iv[1]),
                    smeSCENE.VertexesVS+iv[2], smeSCENE_GetVertexColor(iv[2]));
            } break;
            
            case smeMESH_Material_TexMap:
            {                
                smeRASTER_PolygonTexMap(
                    smeSCENE.VertexesVS+iv[0], smeSCENE.Meshes[0]->UVs+iv[0],
                    smeSCENE.VertexesVS+iv[1], smeSCENE.Meshes[0]->UVs+iv[1],
                    smeSCENE.VertexesVS+iv[2], smeSCENE.Meshes[0]->UVs+iv[2],
                    smeSCENE.Meshes[smeSCENE.PolygonMeshes[sf]]->TexMap);
            } break;
        }
    }
}

u16 smeSCENE_Add(smeMesh* mesh)
{
    if (smeSCENE.MeshCount<smeSCENE_MAX_MESHES)
    {
        smeSCENE.Meshes[smeSCENE.MeshCount++] = mesh;
        return smeSCENE.MeshCount-1;
    }
    return -1;
}

void smeSCENE_Remove(u16 mesh)
{
    if (mesh<smeSCENE.MeshCount)
    {
        int i;
        for (i=mesh ; i<smeSCENE.MeshCount-1 ; ++i)
            smeSCENE.Meshes[i] = smeSCENE.Meshes[i+1];
        --smeSCENE.MeshCount;        
        smeSCENE.Meshes[smeSCENE.MeshCount] = NULL;
    }
}


