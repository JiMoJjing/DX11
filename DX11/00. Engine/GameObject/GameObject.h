#pragma once

#include "00. Engine/Component/Component.h"

class Animator;
class MeshRenderer;
class MonoBehavior;
class Camera;

class GameObject : public enable_shared_from_this<GameObject>
{
public:
    GameObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext);
    ~GameObject();
    
    
    void Awake();
    void Start();
    void Update();
    void LateUpdate();
    void FixedUpdate();
    
    shared_ptr<Component> GetFixedComponent(ComponentType type);
    shared_ptr<Transform> GetTransform();
    shared_ptr<Camera> GetCamera();
    shared_ptr<MeshRenderer> GetMeshRenderer();
    shared_ptr<Animator> GetAnimator();
    
    shared_ptr<Transform> GetOrAddTransform();
    void AddComponent(shared_ptr<Component> component);

private:
    ComPtr<ID3D11Device> _device;
    
protected:
    // [ Transform, MeshRenderer, Camera, Animator ]
    array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
    
    // [ Script ... ]
    vector<shared_ptr<MonoBehavior>> _scripts;
};
