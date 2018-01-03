#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include "OBJ_Loader.h"
#include <vector>

class Camera;


struct Vertex {
    Vertex() : pos(0, 0, 0), color(0, 0, 0, 0) {};
    Vertex(float x, float y, float z, float r, float g, float b, float a) : pos(x, y, z), color(r, g, b, z) {}
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
};

struct ConstantBufferObject {
    DirectX::XMFLOAT4X4 wvpMat;
};

class GameObject
{

    public:
        objl::Mesh* m_mesh;
        ID3D12Resource** constantBufferUploadHeaps;
        UINT8** cbvGPUAddress;

        void Rotate(float a_x, float a_y, float a_z, float a_value);
        void Translate(float a_x, float a_y, float a_z);
        void Scale(float a_x, float a_y, float a_z);
        void SetParent(GameObject* a_parent);
        void AddChildren(GameObject* a_child);
        void SetObj(std::string a_obj);
        void Display(ID3D12Device * device, ID3D12GraphicsCommandList * commandList, int frameIndex, int numCubeIndices, Camera* camera);
      //  void Display(ID3D12Device * device, ID3D12GraphicsCommandList * commandList, ID3D12Resource ** constantBufferUploadHeaps, int frameIndex, int numCubeIndices, int ConstantBufferPerObjectAlignedSize);
      //  void Display(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

        void SetBufferVertexView(ID3D12Resource* vertexBuffer, int vBufferSize);
        void SetBufferIndexView(ID3D12Resource* vertexBuffer,int vBufferSize);
        void CreateCBUploadHeap(ID3D12Device * device, int frameBufferCount);

        int ConstantBufferObjectAlignedSize = (sizeof(ConstantBufferObject) + 255) & ~255;
        ConstantBufferObject cbObject; // this is the constant buffer data we will send to the gpu 
                                             // (which will be placed in the resource we created above)



        DirectX::XMMATRIX GetWorldMatrix();
      //  DirectX::XMMATRIX GetVPMatrix();


        D3D12_VERTEX_BUFFER_VIEW vertexBufferView; 
        D3D12_INDEX_BUFFER_VIEW indexBufferView; 

        std::vector<GameObject*> m_children;
        DirectX::XMFLOAT4 m_up;
        DirectX::XMFLOAT4 m_right;
        DirectX::XMFLOAT4 m_target;
        DirectX::XMFLOAT4 m_position;
        DirectX::XMFLOAT4X4 m_transformationMatrix;
        DirectX::XMFLOAT4X4 m_projectionMatrix;
        DirectX::XMFLOAT4X4 m_viewMatrix;


        GameObject();
        GameObject(DirectX::XMFLOAT4 a_position);
        GameObject(DirectX::XMFLOAT4 a_position, DirectX::XMFLOAT4 a_target, DirectX::XMFLOAT4 a_up, DirectX::XMFLOAT4 a_right);
        ~GameObject();
};





