#include "pch.h"
#include "GameObject.h"



GameObject::GameObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
    : _device(device), _deviceContext(deviceContext)
{
    _geometry = make_shared<Geometry<VertexTextureData>>();
    //_geometry = make_shared<Geometry<VertexColorData>>();
    GeometryHelper::CreateRectangle(_geometry);
    //GeometryHelper::CreateRectangle(_geometry, Color{1.0f, 0.0f, 0.0f, 1.0f});
    
    _vertexBuffer = make_shared<VertexBuffer>(_device);
    _vertexBuffer->Create(_geometry->GetVertices());
    
    _indexBuffer = make_shared<IndexBuffer>(_device);
    _indexBuffer->Create(_geometry->GetIndices());
    
    _vertexShader = make_shared<VertexShader>(_device);
    _vertexShader->Create(L"98. Shaders/Default.hlsl", "VS", "vs_5_0");
    //_vertexShader->Create(L"98. Shaders/Color.hlsl", "VS", "vs_5_0");

    _pixelShader = make_shared<PixelShader>(_device);
    _pixelShader->Create(L"98. Shaders/Default.hlsl", "PS", "ps_5_0");
    //_pixelShader->Create(L"98. Shaders/Color.hlsl", "PS", "ps_5_0");
    
    _inputLayout = make_shared<InputLayout>(_device);
    _inputLayout->Create(VertexTextureData::descs, _vertexShader->GetBlob());
    //_inputLayout->Create(VertexColorData::descs, _vertexShader->GetBlob());
    
    _constantBuffer = make_shared<ConstantBuffer<TransformData>>(_device, _deviceContext);
    _constantBuffer->Create();

    _texture = make_shared<Texture>(_device);
    _texture->Create(L"BusilPotato.png");
    
    _rasterizerState = make_shared<RasterizerState>(_device);
    _rasterizerState->Create();
    
    _samplerState = make_shared<SamplerState>(_device);
    _samplerState->Create();
    
    _blendState = make_shared<BlendState>(_device);
    _blendState->Create();
    
}

GameObject::~GameObject()
{
}

void GameObject::Awake()
{
    for (shared_ptr<Component>& component : _components)
    {
        if (component != nullptr)
        {
            component->Awake();
        }
    }
    
    for (shared_ptr<MonoBehavior>& script : _scripts)
    {
        script->Awake();
    }
}

void GameObject::Start()
{
    for (shared_ptr<Component>& component : _components)
    {
        if (component != nullptr)
        {
            component->Start();
        }
    }
    
    for (shared_ptr<MonoBehavior>& script : _scripts)
    {
        script->Start();
    }
}

void GameObject::Update()
{
    for (shared_ptr<Component>& component : _components)
    {
        if (component != nullptr)
        {
            component->Update();
        }
    }
    
    for (shared_ptr<MonoBehavior>& script : _scripts)
    {
        script->Update();
    }
    
    _transformData.matWorld = GetOrAddTransform()->GetWorldMatrix();
    _constantBuffer->CopyData(_transformData);
}

void GameObject::LateUpdate()
{
    for (shared_ptr<Component>& component : _components)
    {
        if (component != nullptr)
        {
            component->LateUpdate();
        }
    }
    
    for (shared_ptr<MonoBehavior>& script : _scripts)
    {
        script->LateUpdate();
    }
}

void GameObject::FixedUpdate()
{
    for (shared_ptr<Component>& component : _components)
    {
        if (component != nullptr)
        {
            component->FixedUpdate();
        }
    }
    
    for (shared_ptr<MonoBehavior>& script : _scripts)
    {
        script->FixedUpdate();
    }   
}

shared_ptr<Component> GameObject::GetFixedComponent(ComponentType type)
{
    uint8 index = static_cast<uint8>(type);
    assert(index < FIXED_COMPONENT_COUNT);
    return _components[index];
}

shared_ptr<Transform> GameObject::GetTransform()
{
    shared_ptr<Component> component = GetFixedComponent(ComponentType::Transform);
    return static_pointer_cast<Transform>(component);
}

shared_ptr<Transform> GameObject::GetOrAddTransform()
{
    if (GetTransform() == nullptr)
    {
        shared_ptr<Transform> transform = make_shared<Transform>();
        AddComponent(transform);
        return transform;
    }
    return GetTransform();
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
    component->SetGameObject(shared_from_this());
    
    uint8 index = static_cast<uint8>(component->GetType());
    if (index < FIXED_COMPONENT_COUNT)
    {
        _components[index] = component;
    }
    else
    {
        _scripts.push_back(dynamic_pointer_cast<MonoBehavior>(component));
    }
}

void GameObject::Render(shared_ptr<Pipeline> pipeline)
{
    PipelineInfo info;
    info.inputLayout = _inputLayout;
    info.vertexShader = _vertexShader;
    info.pixelShader = _pixelShader;
    info.rasterizerState = _rasterizerState;
    info.blendState = _blendState;
    pipeline->UpdatePipeline(info);
    
    pipeline->SetVertexBuffer(_vertexBuffer);
    pipeline->SetIndexBuffer(_indexBuffer);
    pipeline->SetConstantBuffer(0, SS_VertexShader, _constantBuffer);
    pipeline->SetTexture(0, SS_PixelShader, _texture);
    pipeline->SetSamplerState(0, SS_PixelShader, _samplerState);
    
    pipeline->DrawIndexed(_geometry->GetIndexCount(), 0, 0);
}
