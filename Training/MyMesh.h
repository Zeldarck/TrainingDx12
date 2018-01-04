#pragma once
#include "OBJ_Loader.h"
#include "d3dx12.h"
#include <DirectXMath.h>
#include <wincodec.h>


struct Vertex {
    Vertex() : pos(0, 0, 0), color(0, 0, 0, 0),texCoord(0,0) {};
    Vertex(float x, float y, float z, float r, float g, float b, float a, float u, float v) : pos(x, y, z), color(r, g, b, z), texCoord(u,v) {}
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT2 texCoord;

};

class MyMesh
{
    protected:
        ID3D12Resource* vertexBuffer;
        ID3D12Resource* indexBuffer;
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        int vBufferSize;
        int vIndexSize;
        objl::Mesh* m_mesh = nullptr;
        std::vector<MyMesh*> m_children;
        void SetBufferVertexView();
        void SetBufferIndexView();
        ID3D12DescriptorHeap* mainDescriptorHeap;
        ID3D12Resource* textureBufferUploadHeap;
        ID3D12Resource* textureBuffer;

    public:

        int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int &bytesPerRow);

        DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
        WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);
        int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);



        std::vector<MyMesh*> getChildren();

        D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView();
        D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView();

        void SetObj(std::string a_obj);
        bool HaveATexture();
        void Draw(ID3D12GraphicsCommandList * a_commandList);
        void PushOnGPU(ID3D12Device * a_device, ID3D12GraphicsCommandList * a_commandList);
        void PushTextureOnGPU(ID3D12Device * a_device, ID3D12GraphicsCommandList * a_commandList);
        int GetCountIndex();
        MyMesh();
        MyMesh(objl::Mesh * a_mesh);
        ~MyMesh();
};

