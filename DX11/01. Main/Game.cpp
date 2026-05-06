#include "pch.h"
#include "Game.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hwnd)
{
    _hwnd = hwnd;
    _width = GWinSizeX;
    _height = GWinSizeY;
    
    // TODO
    CreateDeviceAndSwapChain();
    CreateRenderTargetView();
    SetViewport();
    
    CreateGeometry();
    CreateVS();
    CreateInputLayout();
    CreatePS();
    
    CreateRasterizerState();
    CreateSamplerState();
    CreateBlendState();
    
    CreateSRV();
    CreateConstantBuffer();
}

void Game::Update()
{
    D3D11_MAPPED_SUBRESOURCE subResource;
    ZeroMemory(&subResource, sizeof(subResource));
    
    // _transformData.offset.x += 0.0003f;
    // _transformData.offset.y += 0.0003f;
     
    _deviceContext->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
    ::memcpy(subResource.pData, &_transformData, sizeof(TransformData));
    _deviceContext->Unmap(_constantBuffer.Get(), 0);
}

void Game::Render()
{
    // 그리기 전 세팅.
    RenderBegin();
    
    // IA - VS - RS - PS - OM
    {
        uint32 stride = sizeof(Vertex);
        uint32 offset = 0;
        
        // IA
        _deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        _deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
        _deviceContext->IASetInputLayout(_inputLayout.Get());
        _deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
        // VS
        _deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
        _deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
        
        // RS
        _deviceContext->RSSetState(_rasterizerState.Get());
        
        // PS
        _deviceContext->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());
        _deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
        _deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());
        
        // OM
        _deviceContext->OMSetBlendState(_blendState.Get(), nullptr, 0xFFFFFFFF);
        _deviceContext->DrawIndexed((uint32)_indices.size(), 0, 0);
    }
    
    // 다 그렸으면 Present.
    RenderEnd();
}

void Game::RenderBegin()
{
    _deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
    _deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
    _deviceContext->RSSetViewports(1, &_viewport);
}

void Game::RenderEnd()
{
    HRESULT hr = _swapChain->Present(1, 0);
    CHECK(hr);
}

void Game::CreateDeviceAndSwapChain()
{
    DXGI_SWAP_CHAIN_DESC desc = {};
    ZeroMemory(&desc, sizeof(desc));
    {
        desc.BufferDesc.Width = _width;
        desc.BufferDesc.Height = _height;
        desc.BufferDesc.RefreshRate.Numerator = 60;
        desc.BufferDesc.RefreshRate.Denominator = 1;
        desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        desc.SampleDesc.Count = 1;                                                  // 계단 현상 관련.
        desc.SampleDesc.Quality = 0;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                         // 버퍼의 최종 역할.
        desc.BufferCount = 1;                                                       // 후면 버퍼 개수.
        desc.OutputWindow = _hwnd; 
        desc.Windowed = TRUE;
        desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    }
    
    // ID3D11Device* 
    // _device.Get();
    
    // ID3D11Device**
    // _device.GetAddressOf();
    
    HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &desc,
        _swapChain.GetAddressOf(),
        _device.GetAddressOf(),
        nullptr,
        _deviceContext.GetAddressOf()
        );
    
    CHECK(hr);
}

void Game::CreateRenderTargetView()
{
    HRESULT hr;
    
    ComPtr<ID3D11Texture2D> backBuffer = nullptr;
    hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
    CHECK(hr);
    
    _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
}

void Game::SetViewport()
{
    _viewport.TopLeftX = 0.f;
    _viewport.TopLeftY = 0.f;
    _viewport.Width = static_cast<float>(_width);
    _viewport.Height = static_cast<float>(_height);
    _viewport.MinDepth = 0.f;
    _viewport.MaxDepth = 1.f;
}

void Game::CreateGeometry()
{
    // VertexData
    {
        _vertices.resize(4);
        _vertices[0].position = { -0.5f, -0.5f, 0.0f };
        _vertices[1].position = { -0.5f,  0.5f, 0.0f };
        _vertices[2].position = {  0.5f, -0.5f, 0.0f };
        _vertices[3].position = {  0.5f,  0.5f, 0.0f };
        
        _vertices[0].uv = { 0.0f, 5.0f };
        _vertices[1].uv = { 0.0f, 0.0f };
        _vertices[2].uv = { 5.0f, 5.0f };
        _vertices[3].uv = { 5.0f, 0.0f };
    
        //_vertices[0].color = { 1.0f, 0.0f, 0.0f, 1.0f };
        //_vertices[1].color = { 0.0f, 1.0f, 0.0f, 1.0f };
        //_vertices[2].color = { 0.0f, 0.0f, 1.0f, 1.0f };
        //_vertices[3].color = { 0.0f, 1.0f, 1.0f, 1.0f };
    }
    
    // VertexBuffer
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Usage = D3D11_USAGE_IMMUTABLE;                     // GPU의 접근 제한(Read Only, Write X).
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;              // 이 버퍼를 어떠한 용도로 사용? (지금 여기선 정점 버퍼로).
        desc.ByteWidth = (uint32)(sizeof(Vertex) * _vertices.size());
        
        
        D3D11_SUBRESOURCE_DATA data;
        ZeroMemory(&data, sizeof(data));
        data.pSysMem = _vertices.data();                        // 첫 번째 데이터의 주소.
        
        _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
    }
    
    // IndexData
    {
        _indices = {0, 1, 2, 2, 1, 3};
    }
    
    // IndexBuffer
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.ByteWidth = (uint32)(sizeof(uint32) * _indices.size());
        
        D3D11_SUBRESOURCE_DATA data;
        ZeroMemory(&data, sizeof(data));
        data.pSysMem = _indices.data();
        
        _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());       
    }
}

void Game::CreateInputLayout()
{
    D3D11_INPUT_ELEMENT_DESC  layout[] = 
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    
    const int32 count = (int32)(sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC));
    
    _device->CreateInputLayout(layout, count, _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
}

void Game::CreateVS()
{
    LoadShaderFromFile(L"98. Shaders/Default.hlsl", "VS", "vs_5_0", _vsBlob);
    
    HRESULT hr = _device->CreateVertexShader(_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());
    CHECK(hr);
}

void Game::CreatePS()
{
    LoadShaderFromFile(L"98. Shaders/Default.hlsl", "PS", "ps_5_0", _psBlob);
    
    HRESULT hr = _device->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());
    CHECK(hr);
}

void Game::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    {
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = FALSE;
    }
    
    HRESULT hr = _device->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());
    CHECK(hr);
}

void Game::CreateSamplerState()
{
    D3D11_SAMPLER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    {
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.BorderColor[0] = 1.0f;
        desc.BorderColor[1] = 0.0f;
        desc.BorderColor[2] = 0.0f;
        desc.BorderColor[3] = 1.0f;
        desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.MaxAnisotropy = 16;
        desc.MinLOD = FLT_MIN;
        desc.MaxLOD = FLT_MAX;
        desc.MipLODBias = 0.0f;        
    }
    
    HRESULT hr = _device->CreateSamplerState(&desc, _samplerState.GetAddressOf());
    CHECK(hr);
}

void Game::CreateBlendState()
{
    D3D11_BLEND_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    {
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;       
    }
    
    HRESULT hr = _device->CreateBlendState(&desc, _blendState.GetAddressOf());
    CHECK(hr);
}

void Game::CreateSRV()
{
    DirectX::TexMetadata md;
    DirectX::ScratchImage image;
    
    HRESULT hr = ::LoadFromWICFile(L"BusilPotato.png", WIC_FLAGS_NONE, &md, image);
    CHECK(hr);
    
    hr = ::CreateShaderResourceView(_device.Get(), image.GetImages(), image.GetImageCount(), md, _shaderResourceView.GetAddressOf());
    CHECK(hr);
}

void Game::CreateConstantBuffer()
{
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    {
        desc.Usage = D3D11_USAGE_DYNAMIC;               // CPU_Write + GPU_Read
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = sizeof(TransformData);
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        
        HRESULT hr = _device->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
        CHECK(hr);
    }
}

void Game::LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob)
{
    const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    
    // d3dcompiler.lib
    HRESULT hr = ::D3DCompileFromFile(
        path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        name.c_str(),
        version.c_str(),
        compileFlag,
        0,
        blob.GetAddressOf(),
        nullptr
        );
    
    CHECK(hr);
}
