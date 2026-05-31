#pragma once


#include "Component.h"

class Transform : public Component
{
    using Super = Component;
    
public:
    Transform();
    virtual ~Transform();
    
    virtual void Awake() override;
    virtual void Update() override;
    
    void UpdateTransform();
    
    // Local
    Vec3 GetLocalPosition() const { return _localPosition; }
    void SetLocalPosition(const Vec3& position) { _localPosition = position; UpdateTransform(); }
    
    Vec3 GetLocalRotation() const { return _localRotation; }
    void SetLocalRotation(const Vec3& rotation) { _localRotation = rotation; UpdateTransform(); }
    
    Vec3 GetLocalScale() const { return _localScale; }
    void SetLocalScale(const Vec3& scale) { _localScale = scale; UpdateTransform(); }

    // World
    Vec3 GetPosition() const { return _position; }
    void SetPosition(const Vec3& worldPosition);
    
    Vec3 GetRotation() const { return _rotation; }
    void SetRotation(const Vec3& worldRotation);
    
    Vec3 GetScale() const { return _scale; }
    void SetScale(const Vec3& worldScale);
    
    Vec3 GetRight() { return _matWorld.Right(); }
    Vec3 GetUp() { return _matWorld.Up(); }
    Vec3 GetLook() { return _matWorld.Backward(); }
    
    Matrix GetWorldMatrix() const { return _matWorld; }
    
    // Hierarchy
    bool HasParent() const { return _parent != nullptr; }
    shared_ptr<Transform> GetParent() const { return _parent; }
    void SetParent(shared_ptr<Transform> parent) { _parent = parent; };
    
    const vector<shared_ptr<Transform>>& GetChildren() const { return _children; }
    void AddChild(shared_ptr<Transform> child) { _children.push_back(child); }
    void RemoveChild(shared_ptr<Transform> child) { _children.erase(std::remove(std::begin(_children), std::end(_children), child), std::end(_children)); }
    
private:
    Vec3 _localPosition = {0.0f, 0.0f, 0.0f};
    Vec3 _localRotation = {0.0f, 0.0f, 0.0f};
    Vec3 _localScale = {1.0f, 1.0f, 1.0f};
    
    // Cache
    Matrix _matLocal = Matrix::Identity;
    Matrix _matWorld = Matrix::Identity;   
    
    Vec3 _position;
    Vec3 _rotation;
    Vec3 _scale;

private:
    shared_ptr<Transform> _parent;
    vector<shared_ptr<Transform>> _children;
};
