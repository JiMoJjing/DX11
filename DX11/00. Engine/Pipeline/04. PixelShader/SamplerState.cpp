#include "pch.h"
#include "SamplerState.h"

SamplerState::SamplerState(ComPtr<ID3D11Device> device)
    : _device(device)
{
    
}

SamplerState::~SamplerState()
{
    
}

void SamplerState::Create()
{
    D3D11_SAMPLER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
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
    
    HRESULT hr = _device->CreateSamplerState(&desc, _samplerState.GetAddressOf());
    CHECK(hr);
}
