#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include "GameObject.h"
#include "Camera.h"
#include "Util.h"

void GameObject::Rotate(float a_x, float a_y, float a_z, float a_value)
{
    DirectX::XMStoreFloat4x4(&m_transformationMatrix, DirectX::XMLoadFloat4x4(&m_transformationMatrix) *= DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(a_x,a_y,a_z,0.0f)),a_value));
}

void GameObject::Translate(float a_x, float a_y, float a_z)
{
    DirectX::XMStoreFloat4x4(&m_transformationMatrix, DirectX::XMLoadFloat4x4(&m_transformationMatrix) *= DirectX::XMMatrixTranslation(a_x, a_y, a_z));
}

void GameObject::Scale(float a_x, float a_y, float a_z)
{
    DirectX::XMStoreFloat4x4(&m_transformationMatrix, DirectX::XMLoadFloat4x4(&m_transformationMatrix) *= DirectX::XMMatrixScaling(a_x, a_y, a_z));
}

void GameObject::SetParent(GameObject * a_parent)
{
    a_parent->AddChildren(this);
}

void GameObject::AddChildren(GameObject * a_child)
{
    m_children.push_back(a_child);
    a_child->m_parent = this;
}




void GameObject::Draw(ID3D12GraphicsCommandList * a_commandList, int a_frameIndex, Camera* a_camera, DirectX::XMMATRIX a_transformationMatrix /* = DirectX::XMMatrixIdentity() */)
{
    DirectX::XMMATRIX worldMatrix = GetLocalWorldMatrix() * a_transformationMatrix;

    //If we have a mesh
    if (m_mesh && m_mesh->HaveMesh()) {
        m_mesh->SetPipeline(a_commandList);

        DirectX::XMMATRIX wvpMat = worldMatrix *  a_camera->GetVPMatrix(); // create wvp matrix

        DirectX::XMMATRIX transposed = XMMatrixTranspose(wvpMat); // must transpose wvp matrix for the gpu
        XMStoreFloat4x4(&cbObject.wvpMat, transposed); // store transposed wvp matrix in constant buffer

        memcpy(cbvGPUAddress[a_frameIndex], &cbObject, sizeof(cbObject));



        a_commandList->SetGraphicsRootConstantBufferView(0, constantBufferUploadHeaps[a_frameIndex]->GetGPUVirtualAddress());

        m_mesh->Draw(a_commandList);

    }

    for (GameObject* go : m_children) {
        go->Draw(a_commandList, a_frameIndex, a_camera, worldMatrix);
    }

}


void GameObject::CreateCBUploadHeap(ID3D12Device * a_device, int a_frameBufferCount)
{
    constantBufferUploadHeaps = new ID3D12Resource*[a_frameBufferCount];
    cbvGPUAddress = new UINT8*[a_frameBufferCount];
    for (int i = 0; i < a_frameBufferCount; ++i)
    {
        HRESULT hr = a_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 
            D3D12_HEAP_FLAG_NONE, 
            &CD3DX12_RESOURCE_DESC::Buffer(1024 * 64), 
            D3D12_RESOURCE_STATE_GENERIC_READ, 
            nullptr,
            IID_PPV_ARGS(&constantBufferUploadHeaps[i]));
        constantBufferUploadHeaps[i]->SetName(L"Constant Buffer Upload Resource Heap");

        ZeroMemory(&cbObject, sizeof(cbObject));

        CD3DX12_RANGE readRange(0, 0);	

        hr = constantBufferUploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&cbvGPUAddress[i]));

        memcpy(cbvGPUAddress[i], &cbObject, sizeof(cbObject)); 
    }
    m_device = a_device;
    m_frameBufferCount = a_frameBufferCount;

}

void GameObject::SetMesh(MyMesh* a_mesh)
{
    m_mesh = a_mesh;
    for (MyMesh* mesh : a_mesh->getChildren()) {
        GameObject* go = new GameObject();
        go->SetMesh(mesh);
        go->CreateCBUploadHeap(m_device, m_frameBufferCount);
        this->AddChildren(go);
    }
}


DirectX::XMMATRIX GameObject::GetLocalWorldMatrix() {
    return XMLoadFloat4x4(&m_transformationMatrix) * DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}

DirectX::XMMATRIX GameObject::GetWorldMatrix() {
    DirectX::XMMATRIX parentWorld = DirectX::XMMatrixIdentity();
    if (m_parent) {
        parentWorld *= m_parent->GetWorldMatrix();
    }

    return GetLocalWorldMatrix() * parentWorld;
}


GameObject::GameObject() : GameObject::GameObject(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f))
{
}

GameObject::GameObject(DirectX::XMFLOAT4 a_position) : GameObject(a_position, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f))
{
}

GameObject::GameObject(DirectX::XMFLOAT4 a_position, DirectX::XMFLOAT4 a_target, DirectX::XMFLOAT4 a_up, DirectX::XMFLOAT4 a_right) 
    : m_position(a_position),m_target(a_target), m_up(a_up), m_right(a_right)
{
    DirectX::XMStoreFloat4x4(&m_transformationMatrix, DirectX::XMMatrixIdentity());
}


GameObject::~GameObject()
{
    for (int i = 0; i < m_frameBufferCount; ++i) {
        if (constantBufferUploadHeaps) {
            SAFE_RELEASE(constantBufferUploadHeaps[i]);
        }
    }
    if(constantBufferUploadHeaps)
        delete constantBufferUploadHeaps;
    if (cbvGPUAddress)
        delete cbvGPUAddress;

}
