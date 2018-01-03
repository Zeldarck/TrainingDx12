#pragma once
#include "OBJ_Loader.h"
#include "d3dx12.h"
#include <DirectXMath.h>


struct Vertex {
    Vertex() : pos(0, 0, 0), color(0, 0, 0, 0) {};
    Vertex(float x, float y, float z, float r, float g, float b, float a) : pos(x, y, z), color(r, g, b, z) {}
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
};

class MyMesh
{
public:
    objl::Loader Loader;
    objl::Mesh* m_mesh;

    ID3D12Resource* vertexBuffer;
    ID3D12Resource* indexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW indexBufferView;
    int vBufferSize;
    int vIndexSize;

    D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView();
    D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView();

    void SetObj(std::string a_obj);
    void PushOnGPU(ID3D12Device * device, ID3D12GraphicsCommandList * commandList);
    void SetBufferVertexView();
    void SetBufferIndexView();
    int GetCountIndex();
    MyMesh();
    ~MyMesh();
};
