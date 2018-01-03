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

    public:
        ID3D12Resource** constantBufferUploadHeaps;
        UINT8** cbvGPUAddress;
        MyMesh* m_mesh;

        void Rotate(float a_x, float a_y, float a_z, float a_value);
        void Translate(float a_x, float a_y, float a_z);
        void Scale(float a_x, float a_y, float a_z);
        void SetParent(GameObject* a_parent);
        void AddChildren(GameObject* a_child);
        void Display(ID3D12Device * device, ID3D12GraphicsCommandList * commandList, int frameIndex, Camera * camera);

        void CreateCBUploadHeap(ID3D12Device * device, int frameBufferCount);

        void SetMesh(MyMesh* a_mesh);

        int ConstantBufferObjectAlignedSize = (sizeof(ConstantBufferObject) + 255) & ~255;
        ConstantBufferObject cbObject; // this is the constant buffer data we will send to the gpu 
                                             // (which will be placed in the resource we created above)



        DirectX::XMMATRIX GetWorldMatrix();
      //  DirectX::XMMATRIX GetVPMatrix();


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





