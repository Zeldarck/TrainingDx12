#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <vector>
#include "MyMesh.h"

class Camera;



struct ConstantBufferObject {
    DirectX::XMFLOAT4X4 wvpMat;
};

class GameObject
{
    protected :
        ID3D12Device * m_device = nullptr;
        ID3D12Resource** constantBufferUploadHeaps = nullptr;
        UINT8** cbvGPUAddress = nullptr;
        MyMesh* m_mesh = nullptr;
        GameObject* m_parent = nullptr;
        int m_frameBufferCount = 0;
        ConstantBufferObject cbObject;
        std::vector<GameObject*> m_children;
        DirectX::XMFLOAT4 m_up;
        DirectX::XMFLOAT4 m_right;
        DirectX::XMFLOAT4 m_target;
        DirectX::XMFLOAT4 m_position;
        DirectX::XMFLOAT4X4 m_transformationMatrix;
        DirectX::XMFLOAT4X4 m_viewMatrix;

    public:

        void Rotate(float a_x, float a_y, float a_z, float a_value);
        void Translate(float a_x, float a_y, float a_z);
        void Scale(float a_x, float a_y, float a_z);
        void SetParent(GameObject* a_parent);
        void AddChildren(GameObject* a_child);
        void Draw(ID3D12GraphicsCommandList * a_commandList, int a_frameIndex, Camera * a_camera, DirectX::XMMATRIX a_transformationMatrix = DirectX::XMMatrixIdentity());

        void CreateCBUploadHeap(ID3D12Device * a_device, int a_frameBufferCount);

        void SetMesh(MyMesh* a_mesh);


        DirectX::XMMATRIX GetLocalWorldMatrix();

        DirectX::XMMATRIX GetWorldMatrix();


    


        GameObject();
        GameObject(DirectX::XMFLOAT4 a_position);
        GameObject(DirectX::XMFLOAT4 a_position, DirectX::XMFLOAT4 a_target, DirectX::XMFLOAT4 a_up, DirectX::XMFLOAT4 a_right);
        ~GameObject();
};





