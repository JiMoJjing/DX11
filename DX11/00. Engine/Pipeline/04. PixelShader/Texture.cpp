#include "pch.h"
#include "Texture.h"

Texture::Texture(ComPtr<ID3D11Device> device)
    : _device(device)
{
}

Texture::~Texture()
{
}

void Texture::Create(const wstring& path)
{
    DirectX::TexMetadata md;
    DirectX::ScratchImage image;
    
    HRESULT hr = ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &md, image);
    CHECK(hr);
    
    hr = ::CreateShaderResourceView(_device.Get(), image.GetImages(), image.GetImageCount(), md, _shaderResourceView.GetAddressOf());
    CHECK(hr);
}
