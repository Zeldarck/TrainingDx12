#pragma once
#include "GameObject.h"


class Camera : public GameObject
{
    protected :
        bool m_isOrthographic;
        float m_with;
        float m_heigth;
        float m_nearZ;
        float m_farZ;
        float m_fov;
    public :
        void SetIsOrthographic(bool a_isOrthographic);
        void SetView(float a_width, float a_heigth);
        void SetNearZ(float a_nearZ);
        void SetFarZ(float a_farZ);
        void SetFOV(float a_fov);
        DirectX::XMMATRIX GetVPMatrix();
        DirectX::XMMATRIX GetProjectionMatrix();
        Camera();
        Camera(DirectX::XMFLOAT4 a_position, DirectX::XMFLOAT4 a_target, DirectX::XMFLOAT4 a_up, DirectX::XMFLOAT4 a_right);
        ~Camera();
};

