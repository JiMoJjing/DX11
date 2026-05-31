#include "pch.h"
#include "Camera.h"

#include "00. Engine/Component/Transform.h"

Matrix Camera::S_MatView = Matrix::Identity;
Matrix Camera::S_MatProjection = Matrix::Identity;

Camera::Camera()
    : Super(ComponentType::Camera)
{
    
}

Camera::~Camera()
{
    
}

void Camera::Update()
{
    UpdateMatrix();
}

void Camera::UpdateMatrix()
{
    Vec3 eyePosition = GetTransform()->GetPosition();
    Vec3 focusPosition = eyePosition + GetTransform()->GetLook();
    Vec3 upDirection = GetTransform()->GetUp();    
    S_MatView = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
    
    
    if (_type == ProjectionType::Perspective)
    {
        S_MatProjection = ::XMMatrixPerspectiveFovLH(XM_PI / 4.f, 800.0f / 600.0f, 1.0f, 100.0f);
    }
    else
    {
        S_MatProjection = ::XMMatrixOrthographicLH(8.0f, 6.0f, 0.0f, 1.0f);
    }

}
