#include "pch.h"
#include "MeshRenderer.h"
#include "00. Engine/Component/Camera/Camera.h"
#include "00. Engine/Component/Transform.h"

MeshRenderer::MeshRenderer(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
    : Super(ComponentType::MeshRenderer), _device(device)
{
    
}

MeshRenderer::~MeshRenderer()
{
}
