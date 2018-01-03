#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include "OBJ_Loader.h"
#include <vector>


class GameObject
{

    public:
        objl::Mesh* m_mesh;

        void Rotate(float a_x, float a_y, float a_z, float a_value);
        void Translate(float a_x, float a_y, float a_z);
        void Scale(float a_x, float a_y, float a_z);
        void SetParent(GameObject* a_parent);
        void AddChildren(GameObject* a_child);
        void SetObj(std::string a_obj);
        DirectX::XMMATRIX GetWorldMatrix();
        DirectX::XMMATRIX GetVPMatrix();
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

