#pragma once
#include "00. Engine/Component/Transform.h"

class MonoBehavior;

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
    void Render(shared_ptr<Pipeline> pipeline); 
    
    shared_ptr<Component> GetFixedComponent(ComponentType type);
    shared_ptr<Transform> GetTransform();
    
    shared_ptr<Transform> GetOrAddTransform();
    void AddComponent(shared_ptr<Component> component);

private:
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _deviceContext;
    
    shared_ptr<Geometry<VertexTextureData>> _geometry;
    //shared_ptr<Geometry<VertexColorData>> _geometry;
    shared_ptr<VertexBuffer> _vertexBuffer;
    shared_ptr<IndexBuffer> _indexBuffer;
    shared_ptr<InputLayout> _inputLayout;
    
    shared_ptr<VertexShader> _vertexShader;
    shared_ptr<RasterizerState> _rasterizerState;
    shared_ptr<PixelShader> _pixelShader;
    
    shared_ptr<Texture> _texture;
    
    shared_ptr<SamplerState> _samplerState;
    shared_ptr<BlendState> _blendState;
    
private:
    TransformData _transformData;
    shared_ptr<ConstantBuffer<TransformData>> _constantBuffer;
    
protected:
    // [ Transform, MeshRenderer, Camera, Animator ]
    array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
    
    // [ Script ... ]
    vector<shared_ptr<MonoBehavior>> _scripts;
};
