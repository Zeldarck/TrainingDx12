#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include "GameObject.h"
#include "Camera.h"

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
}

void GameObject::SetObj(std::string a_obj)
{
    objl::Loader Loader;

    bool loadout = Loader.LoadFile(a_obj);
    m_mesh = &Loader.LoadedMeshes[0];
}

void GameObject::Display(ID3D12Device * device, ID3D12GraphicsCommandList * commandList,int frameIndex,int numCubeIndices, Camera* camera)
{

    DirectX::XMMATRIX wvpMat = GetWorldMatrix() *  camera->GetVPMatrix(); // create wvp matrix
    DirectX::XMMATRIX transposed = XMMatrixTranspose(wvpMat); // must transpose wvp matrix for the gpu
    XMStoreFloat4x4(&cbObject.wvpMat, transposed); // store transposed wvp matrix in constant buffer

    memcpy(cbvGPUAddress[frameIndex], &cbObject, sizeof(cbObject));



    commandList->IASetVertexBuffers(0, 1, &vertexBufferView); // set the vertex buffer (using the vertex buffer view)
    commandList->IASetIndexBuffer(&indexBufferView); // set the vertex buffer (using the vertex buffer view)

                                                     // set cube1's constant buffer
    commandList->SetGraphicsRootConstantBufferView(0, constantBufferUploadHeaps[frameIndex]->GetGPUVirtualAddress());

    commandList->DrawIndexedInstanced(numCubeIndices, 1, 0, 0, 0);

}

void GameObject::SetBufferVertexView(ID3D12Resource * vertexBuffer, int vBufferSize)
{
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(Vertex);
    vertexBufferView.SizeInBytes = vBufferSize;
}

void GameObject::SetBufferIndexView(ID3D12Resource * indexBuffer, int vIndexSize)
{
    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = vIndexSize;
    indexBufferView.Format = DXGI_FORMAT_R32_UINT; // 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
}

void GameObject::CreateCBUploadHeap(ID3D12Device * device, int frameBufferCount)
{
    constantBufferUploadHeaps = new ID3D12Resource*[frameBufferCount];
    cbvGPUAddress = new UINT8*[frameBufferCount];
    for (int i = 0; i < frameBufferCount; ++i)
    {
        HRESULT hr = device->CreateCommittedResource(
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

        // Because of the constant read alignment requirements, constant buffer views must be 256 bit aligned. Our buffers are smaller than 256 bits,
        // so we need to add spacing between the two buffers, so that the second buffer starts at 256 bits from the beginning of the resource heap.
        memcpy(cbvGPUAddress[i], &cbObject, sizeof(cbObject)); // cube1's constant buffer data
       // memcpy(cbvGPUAddress[i] + ConstantBufferObjectAlignedSize, &cbPerObject, sizeof(cbPerObject)); // cube2's constant buffer data
    }

}

/*DirectX::XMMATRIX GameObject::GetVPMatrix()
{
    return   DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat4(&m_position), DirectX::XMLoadFloat4(&m_target), DirectX::XMLoadFloat4(&m_up)) * XMLoadFloat4x4(&m_transformationMatrix) * XMLoadFloat4x4(&m_projectionMatrix);
}*/

DirectX::XMMATRIX GameObject::GetWorldMatrix() {
    DirectX::XMMATRIX parentWorld = DirectX::XMMatrixIdentity();
    return XMLoadFloat4x4(&m_transformationMatrix) * DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z) * parentWorld;
}


GameObject::GameObject()
{
    DirectX::XMStoreFloat4x4(&m_transformationMatrix, DirectX::XMMatrixIdentity());
}

GameObject::GameObject(DirectX::XMFLOAT4 a_position) : GameObject(a_position, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f))
{
    DirectX::XMStoreFloat4x4(&m_transformationMatrix, DirectX::XMMatrixIdentity());
}

GameObject::GameObject(DirectX::XMFLOAT4 a_position, DirectX::XMFLOAT4 a_target, DirectX::XMFLOAT4 a_up, DirectX::XMFLOAT4 a_right) 
    : m_position(a_position),m_target(a_target), m_up(a_up), m_right(a_right)
{
    DirectX::XMStoreFloat4x4(&m_transformationMatrix, DirectX::XMMatrixIdentity());
}


GameObject::~GameObject()
{
//delete m_parent;
}
