#include "pch.h"
#include "Transform.h"

Transform::Transform()
    : Super(ComponentType::Transform)
{
    
}

Transform::~Transform()
{
    
}

void Transform::Awake()
{
    
}

void Transform::Update()
{
    
}

Vec3 ToEulerAngles(Quaternion q)
{
    Vec3 angles;
    
    // roll (x-axis rotation)
    double sinr_cosp = 2.0 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.x = atan2(sinr_cosp, cosr_cosp);
    
    double sinp = sqrt(1 + 2 * (q.w * q.y - q.z * q.z));
    double cosp = 2 * (q.w * q.y + q.z * q.z);
    angles.y = atan2(sinp, cosp);
    
    double siny_cosp = 2.0 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.z = atan2(siny_cosp, cosy_cosp);
    
    return angles;
}

void Transform::UpdateTransform()
{
    Matrix matScale = Matrix::CreateScale(_localScale);
    Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
    matRotation *= Matrix::CreateRotationY(_localRotation.y);
    matRotation *= Matrix::CreateRotationZ(_localRotation.z);
    Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

    _matLocal = matScale * matRotation * matTranslation;

    if (HasParent())
    {
        _matWorld = _matLocal * _parent->GetWorldMatrix();
    }
    else
    {
        _matWorld = _matLocal;
    }

    Quaternion quat;
    _matWorld.Decompose(_scale, quat, _position);
    _rotation = ToEulerAngles(quat);

    // Children
    for (const shared_ptr<Transform>& child : _children)
    {
        child->UpdateTransform();
    }
}

void Transform::SetPosition(const Vec3& worldPosition)
{
    if (HasParent())
    {
        Matrix worldToParentLocalMatrix = _parent->GetWorldMatrix().Invert();
        
        Vec3 position;
        position.Transform(worldPosition, worldToParentLocalMatrix);
        
        SetLocalPosition(position);
    }
    else
    {
        SetLocalPosition(worldPosition);
    }
}

void Transform::SetRotation(const Vec3& worldRotation)
{
    if (HasParent())
    {
        Matrix worldToParentLocalMatrix = _parent->GetWorldMatrix().Invert();
        
        Vec3 rotation;
        rotation.Transform(worldRotation, worldToParentLocalMatrix);
        
        SetLocalRotation(rotation);
    }
    else
    {
        SetLocalRotation(worldRotation);
    }
}

void Transform::SetScale(const Vec3& worldScale)
{
    if (HasParent())
    {
        Vec3 parentScale = _parent->GetScale();
        Vec3 scale = worldScale;
        scale.x /= parentScale.x;
        scale.y /= parentScale.y;
        scale.z /= parentScale.z;
        
        SetLocalScale(scale);
    }
    else
    {
        SetLocalScale(worldScale);
    }
}
