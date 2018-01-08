#pragma once
#include "OBJ_Loader.h"
#include "d3dx12.h"
#include <DirectXMath.h>
#include <wincodec.h>
#include "PSO.h"

struct Vertex {
    Vertex() : pos(0, 0, 0), color(0, 0, 0, 0) {};
    Vertex(float x, float y, float z, float r, float g, float b, float a) : pos(x, y, z), color(r, g, b, z) {}
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;

};


struct VertexTexture : public Vertex{
    VertexTexture() : Vertex(),texCoord(0,0) {};
    VertexTexture(float x, float y, float z, float r, float g, float b, float a, float u, float v) : Vertex(x, y, z, r, g, b, z), texCoord(u,v) {}
    DirectX::XMFLOAT2 texCoord;
};



class MyMesh
{
    protected:
        ID3D12Resource* m_vertexBuffer = nullptr;
        ID3D12Resource* m_indexBuffer = nullptr;
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
        int m_vBufferSize;
        int m_vIndexSize;
        objl::Mesh* m_mesh = nullptr;
        std::vector<MyMesh*> m_children;
        void SetBufferVertexView();
        void SetBufferIndexView();
        ID3D12DescriptorHeap* m_mainDescriptorHeap = nullptr;
        ID3D12Resource* m_textureBufferUploadHeap = nullptr;
        ID3D12Resource* m_textureBuffer = nullptr;
        PSO* m_pso = nullptr;
    public:




        std::vector<MyMesh*> getChildren();

        D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView();
        D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView();

        void SetObj(std::string a_obj);
        bool HaveATexture();
        bool HaveMesh();
        void SetPipeline(ID3D12GraphicsCommandList * a_commandList);
        void Draw(ID3D12GraphicsCommandList * a_commandList);
        void PushOnGPU(ID3D12Device * a_device, ID3D12GraphicsCommandList * a_commandList);
        void PushTextureOnGPU(ID3D12Device * a_device, ID3D12GraphicsCommandList * a_commandList);
        int GetCountIndex();
        MyMesh();
        MyMesh(objl::Mesh * a_mesh);
        ~MyMesh();
};

