#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include "GameObject.h"

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

DirectX::XMMATRIX GameObject::GetVPMatrix()
{
    return   DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat4(&m_position), DirectX::XMLoadFloat4(&m_target), DirectX::XMLoadFloat4(&m_up)) * XMLoadFloat4x4(&m_transformationMatrix) * XMLoadFloat4x4(&m_projectionMatrix);
}

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
