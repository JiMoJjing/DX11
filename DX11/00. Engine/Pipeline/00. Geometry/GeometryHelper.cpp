#include "pch.h"
#include "GeometryHelper.h"

void GeometryHelper::CreateRectangle(shared_ptr<Geometry<VertexTextureData>> geometry)
{
    vector<VertexTextureData> vertices;
    vertices.resize(4);

    vertices[0].position = Vec3(-0.5f, -0.5f, 0.0f);
    vertices[1].position = Vec3(-0.5f,  0.5f, 0.0f);
    vertices[2].position = Vec3(0.5f, -0.5f, 0.0f);
    vertices[3].position = Vec3(0.5f,  0.5f, 0.0f);
    
    vertices[0].uv = Vec2(0.0f, 1.0f);
    vertices[1].uv = Vec2(0.0f, 0.0f);
    vertices[2].uv = Vec2(1.0f, 1.0f);
    vertices[3].uv = Vec2(1.0f, 0.0f);
        
    geometry->SetVertices(vertices);
    
    vector<uint32> indices = {0, 1, 2, 2, 1, 3};
    geometry->SetIndices(indices);
}

void GeometryHelper::CreateRectangle(shared_ptr<Geometry<VertexColorData>> geometry, Color color)
{
    vector<VertexColorData> vertices;
    vertices.resize(4);
 
    vertices[0].position = Vec3(-0.5f, -0.5f, 0.0f);
    vertices[1].position = Vec3(-0.5f,  0.5f, 0.0f);
    vertices[2].position = Vec3(0.5f, -0.5f, 0.0f);
    vertices[3].position = Vec3(0.5f,  0.5f, 0.0f);
    
    vertices[0].color = color;
    vertices[1].color = color;
    vertices[2].color = color;
    vertices[3].color = color;
    
    geometry->SetVertices(vertices);
    
    vector<uint32> indices = {0, 1, 2, 2, 1, 3};
    geometry->SetIndices(indices);
}
