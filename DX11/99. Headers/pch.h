#pragma once

#include "Values.h"
#include "Struct.h"
#include "Types.h"

#include <memory>

// STL
#include <vector>
#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <string>

using namespace std;

//  WIN
#include <Windows.h>
#include <assert.h>

// DX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

#define CHECK(p) assert(SUCCEEDED(p))

// Engine
#include "00. Engine/Pipeline/Graphics.h"
#include "00. Engine/Pipeline/01. InputAssembler/VertexBuffer.h"
#include "00. Engine/Pipeline/01. InputAssembler/IndexBuffer.h"
#include "00. Engine/Pipeline/01. InputAssembler/InputLayout.h"

#include "00. Engine/Pipeline/00. Geometry/Geometry.h"
#include "00. Engine/Pipeline/00. Geometry/GeometryHelper.h"
#include "00. Engine/Pipeline/00. Geometry/VertexData.h"

#include "00. Engine/Pipeline/02. VertexShader/Shader.h"
#include "00. Engine/Pipeline/02. VertexShader/ConstantBuffer.h"
#include "00. Engine/Pipeline/04. PixelShader/Texture.h"

#include "00. Engine/Pipeline/03. Rasterizer/RasterizerState.h"
#include "00. Engine/Pipeline/04. PixelShader/SamplerState.h"
#include "00. Engine/Pipeline/05. OutputMerger/BlendState.h"

#include "00. Engine/Pipeline/Pipeline.h"
#include "00. Engine/GameObject/GameObject.h"

#include "00. Engine/Component/Component.h"
#include "00. Engine/Component/Transform.h"
#include "00. Engine/Component/MonoBehavior.h"