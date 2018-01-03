#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include "Camera.h"

void Camera::SetIsOrthographic(bool a_isOrthographic)
{
    m_isOrthographic = a_isOrthographic;
}

void Camera::SetView(float a_width, float a_heigth)
{
    m_with = a_width;
    m_heigth = a_heigth;
}

void Camera::SetNearZ(float a_nearZ)
{
    m_nearZ = a_nearZ;
}

void Camera::SetFarZ(float a_farZ)
{
    m_farZ = a_farZ;
}

void Camera::SetFOV(float a_fov)
{
    m_fov = a_fov;
}

DirectX::XMMATRIX Camera::GetVPMatrix()
{
    return   DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat4(&m_position), DirectX::XMLoadFloat4(&m_target), DirectX::XMLoadFloat4(&m_up)) * DirectX::XMLoadFloat4x4(&m_transformationMatrix) * GetProjectionMatrix();
}

DirectX::XMMATRIX Camera::GetProjectionMatrix()
{
    return m_isOrthographic ? DirectX::XMMatrixOrthographicLH(m_with, m_heigth, m_nearZ, m_farZ) : DirectX::XMMatrixPerspectiveFovLH(m_fov*(3.14f / 180.0f), m_with / m_heigth, m_nearZ, m_farZ);
}

Camera::Camera()
{
}


Camera::Camera(DirectX::XMFLOAT4 a_position, DirectX::XMFLOAT4 a_target, DirectX::XMFLOAT4 a_up, DirectX::XMFLOAT4 a_right) : GameObject(a_position, a_target, a_up, a_right)
{
    
}



Camera::~Camera()
{
}
