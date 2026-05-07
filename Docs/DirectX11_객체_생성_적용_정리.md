# DirectX11 객체 생성 / 적용 정리

DirectX11에서는 대부분의 객체가 비슷한 흐름을 가진다.

1. Desc 구조체에 "어떤 용도와 설정으로 만들지" 적는다.
2. 필요하면 초기 데이터나 컴파일 결과를 준비한다.
3. Device (`ID3D11Device`)로 GPU 리소스 또는 State 객체를 생성한다.
4. DeviceContext (`ID3D11DeviceContext`)로 렌더링 파이프라인에 적용한다.

핵심은 **생성은 Device, 적용은 DeviceContext**라는 점이다.

---

## 전체 흐름 요약

```cpp
// 생성 쪽
device->CreateBuffer(...);
device->CreateVertexShader(...);
device->CreateInputLayout(...);
device->CreateRenderTargetView(...);
device->CreateShaderResourceView(...);
device->CreateRasterizerState(...);

// 적용 쪽
context->IASetVertexBuffers(...);
context->IASetIndexBuffer(...);
context->IASetInputLayout(...);
context->VSSetShader(...);
context->PSSetShader(...);
context->RSSetViewports(...);
context->RSSetState(...);
context->OMSetRenderTargets(...);
```

---

# Device / DeviceContext / SwapChain

## 역할

- Device (`ID3D11Device`)
  - Buffer, Shader, View, State 같은 DirectX11 객체를 생성한다.
- DeviceContext (`ID3D11DeviceContext`)
  - 생성된 객체를 파이프라인에 적용하고 Draw 명령을 보낸다.
- SwapChain (`IDXGISwapChain`)
  - Back Buffer와 Front Buffer를 교체해서 최종 화면을 출력한다.

## 생성

사용 구조체:
- Swap Chain Desc (`DXGI_SWAP_CHAIN_DESC`)

주요 값:
- `BufferDesc.Width`, `BufferDesc.Height`
  - Back Buffer의 가로/세로 크기.
- `BufferDesc.Format`
  - Back Buffer의 픽셀 포맷.
  - 예: `DXGI_FORMAT_R8G8B8A8_UNORM`
- `SampleDesc.Count`
  - 멀티 샘플링 개수.
  - 기본은 `1`.
- `BufferUsage`
  - Back Buffer의 사용 목적.
  - 보통 `DXGI_USAGE_RENDER_TARGET_OUTPUT`.
- `BufferCount`
  - Back Buffer 개수.
- `OutputWindow`
  - 출력할 윈도우 핸들 (`HWND`).
- `Windowed`
  - 창 모드 여부.
- `SwapEffect`
  - 버퍼 교체 방식.

생성 함수:

```cpp
D3D11CreateDeviceAndSwapChain(
    nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    0,
    nullptr,
    0,
    D3D11_SDK_VERSION,
    &swapChainDesc,
    swapChain.GetAddressOf(),
    device.GetAddressOf(),
    nullptr,
    deviceContext.GetAddressOf()
);
```

## 적용 / 사용

SwapChain은 렌더링 끝에 화면을 갱신할 때 사용한다.

```cpp
swapChain->Present(1, 0);
```

## 헷갈리기 쉬운 부분

- Device는 "만드는 역할"이다.
- DeviceContext는 "GPU에 명령하는 역할"이다.
- SwapChain은 렌더링 결과를 화면에 보여주는 역할이다.

---

# Vertex Buffer

## 역할

Vertex Buffer (`ID3D11Buffer`)는 정점 데이터 배열을 GPU 메모리에 올려두는 버퍼다.

파이프라인 위치:
- IA(Input Assembler)

## 생성

사용 구조체:
- Buffer Desc (`D3D11_BUFFER_DESC`)
- Subresource Data (`D3D11_SUBRESOURCE_DATA`)

`D3D11_BUFFER_DESC` 주요 값:
- `ByteWidth`
  - 버퍼 전체 크기.
  - 예: `sizeof(Vertex) * vertexCount`
- `Usage`
  - CPU/GPU 접근 방식.
  - 예: `D3D11_USAGE_IMMUTABLE`, `D3D11_USAGE_DEFAULT`, `D3D11_USAGE_DYNAMIC`
- `BindFlags`
  - 어떤 파이프라인 용도로 바인딩할지.
  - Vertex Buffer는 `D3D11_BIND_VERTEX_BUFFER`.
- `CPUAccessFlags`
  - CPU에서 접근할 수 있는지.
  - `D3D11_USAGE_DYNAMIC`이면 보통 `D3D11_CPU_ACCESS_WRITE`.
- `MiscFlags`
  - 특수 옵션. 기본 버퍼에서는 보통 `0`.
- `StructureByteStride`
  - 구조화 버퍼에서 사용. 일반 Vertex Buffer에서는 보통 `0`.

`D3D11_SUBRESOURCE_DATA` 주요 값:
- `pSysMem`
  - 초기 데이터 주소.
  - 예: `vertices.data()`
- `SysMemPitch`
  - 주로 Texture2D에서 한 줄 크기를 나타낼 때 사용.
- `SysMemSlicePitch`
  - 주로 Texture3D에서 사용.

생성 함수:

```cpp
D3D11_BUFFER_DESC desc = {};
desc.ByteWidth = sizeof(Vertex) * vertexCount;
desc.Usage = D3D11_USAGE_IMMUTABLE;
desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

D3D11_SUBRESOURCE_DATA data = {};
data.pSysMem = vertices.data();

device->CreateBuffer(&desc, &data, vertexBuffer.GetAddressOf());
```

## 적용

```cpp
UINT stride = sizeof(Vertex);
UINT offset = 0;

context->IASetVertexBuffers(
    0,
    1,
    vertexBuffer.GetAddressOf(),
    &stride,
    &offset
);
```

## 헷갈리기 쉬운 부분

- `BindFlags`에 `D3D11_BIND_VERTEX_BUFFER`가 있어야 Vertex Buffer로 사용할 수 있다.
- `stride`는 정점 하나의 크기다.
- `offset`은 버퍼 시작 위치에서 얼마나 건너뛸지다. 보통 `0`.

---

# Index Buffer

## 역할

Index Buffer (`ID3D11Buffer`)는 정점을 어떤 순서로 재사용해서 도형을 만들지 알려주는 버퍼다.

파이프라인 위치:
- IA(Input Assembler)

## 생성

사용 구조체:
- Buffer Desc (`D3D11_BUFFER_DESC`)
- Subresource Data (`D3D11_SUBRESOURCE_DATA`)

`D3D11_BUFFER_DESC` 주요 값:
- `ByteWidth`
  - 인덱스 전체 크기.
  - 예: `sizeof(uint32) * indexCount`
- `Usage`
  - 보통 변경하지 않으면 `D3D11_USAGE_IMMUTABLE`.
- `BindFlags`
  - Index Buffer는 `D3D11_BIND_INDEX_BUFFER`.

생성 함수:

```cpp
D3D11_BUFFER_DESC desc = {};
desc.ByteWidth = sizeof(uint32) * indexCount;
desc.Usage = D3D11_USAGE_IMMUTABLE;
desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

D3D11_SUBRESOURCE_DATA data = {};
data.pSysMem = indices.data();

device->CreateBuffer(&desc, &data, indexBuffer.GetAddressOf());
```

## 적용

```cpp
context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
context->DrawIndexed(indexCount, 0, 0);
```

## 헷갈리기 쉬운 부분

- `DXGI_FORMAT_R32_UINT`는 인덱스 하나가 32비트 unsigned int라는 뜻이다.
- Index Buffer를 사용하면 `Draw`가 아니라 `DrawIndexed`를 호출한다.

---

# Constant Buffer

## 역할

Constant Buffer (`ID3D11Buffer`)는 행렬, 시간, 색상, 옵션 값처럼 셰이더에 전달할 상수 데이터를 담는다.

파이프라인 위치:
- VS(Vertex Shader)
- PS(Pixel Shader)
- 필요한 셰이더 단계에 바인딩한다.

## 생성

사용 구조체:
- Buffer Desc (`D3D11_BUFFER_DESC`)

`D3D11_BUFFER_DESC` 주요 값:
- `ByteWidth`
  - 상수 버퍼 크기.
  - 16바이트 단위에 맞추는 것이 중요하다.
- `Usage`
  - 자주 갱신하면 보통 `D3D11_USAGE_DYNAMIC`.
- `BindFlags`
  - Constant Buffer는 `D3D11_BIND_CONSTANT_BUFFER`.
- `CPUAccessFlags`
  - CPU에서 갱신하려면 `D3D11_CPU_ACCESS_WRITE`.

생성 함수:

```cpp
D3D11_BUFFER_DESC desc = {};
desc.ByteWidth = sizeof(TransformData);
desc.Usage = D3D11_USAGE_DYNAMIC;
desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

device->CreateBuffer(&desc, nullptr, constantBuffer.GetAddressOf());
```

## 데이터 갱신

```cpp
D3D11_MAPPED_SUBRESOURCE subResource = {};

context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
memcpy(subResource.pData, &transformData, sizeof(TransformData));
context->Unmap(constantBuffer.Get(), 0);
```

## 적용

```cpp
context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
```

## 헷갈리기 쉬운 부분

- HLSL의 `cbuffer`와 C++ 구조체의 메모리 배치가 맞아야 한다.
- Constant Buffer 크기는 16바이트 정렬을 신경 써야 한다.
- 값 갱신은 Device가 아니라 DeviceContext의 `Map`, `Unmap`으로 한다.

---

# Shader Blob

## 역할

Shader Blob (`ID3DBlob`)은 HLSL 파일을 컴파일한 결과 바이트코드를 담는 객체다.

Shader Blob은 다음 생성에 사용된다.
- Vertex Shader (`ID3D11VertexShader`)
- Pixel Shader (`ID3D11PixelShader`)
- Input Layout (`ID3D11InputLayout`)

## 생성

사용 함수:
- `D3DCompileFromFile`

주요 인자:
- HLSL 파일 경로
- 진입 함수 이름
  - 예: `"VS"`, `"PS"`
- 셰이더 모델
  - 예: `"vs_5_0"`, `"ps_5_0"`
- 컴파일 결과 Blob
- 에러 Blob

```cpp
ComPtr<ID3DBlob> vertexShaderBlob;
ComPtr<ID3DBlob> errorBlob;

D3DCompileFromFile(
    L"Default.hlsl",
    nullptr,
    nullptr,
    "VS",
    "vs_5_0",
    0,
    0,
    vertexShaderBlob.GetAddressOf(),
    errorBlob.GetAddressOf()
);
```

## 헷갈리기 쉬운 부분

- Blob 자체가 셰이더는 아니다.
- Blob은 컴파일된 바이트코드이고, 이 Blob으로 Shader 객체를 따로 만든다.
- Input Layout을 만들 때도 Vertex Shader Blob이 필요하다.

---

# Vertex Shader / Pixel Shader

## 역할

- Vertex Shader (`ID3D11VertexShader`)
  - 정점 단위로 실행된다.
  - 위치 변환, 데이터 전달 등을 담당한다.
- Pixel Shader (`ID3D11PixelShader`)
  - 픽셀 단위로 실행된다.
  - 최종 색상, 텍스처 샘플링 등을 담당한다.

파이프라인 위치:
- VS(Vertex Shader)
- PS(Pixel Shader)

## 생성

생성에 필요한 것:
- Shader Blob (`ID3DBlob`)

생성 함수:

```cpp
device->CreateVertexShader(
    vertexShaderBlob->GetBufferPointer(),
    vertexShaderBlob->GetBufferSize(),
    nullptr,
    vertexShader.GetAddressOf()
);

device->CreatePixelShader(
    pixelShaderBlob->GetBufferPointer(),
    pixelShaderBlob->GetBufferSize(),
    nullptr,
    pixelShader.GetAddressOf()
);
```

## 적용

```cpp
context->VSSetShader(vertexShader.Get(), nullptr, 0);
context->PSSetShader(pixelShader.Get(), nullptr, 0);
```

## 헷갈리기 쉬운 부분

- Shader 객체 생성은 Device가 한다.
- Shader 적용은 DeviceContext가 한다.
- Vertex Shader Blob은 Input Layout 생성에도 사용된다.

---

# Input Layout

## 역할

Input Layout (`ID3D11InputLayout`)은 C++의 Vertex 구조체와 HLSL Vertex Shader 입력 구조를 연결한다.

파이프라인 위치:
- IA(Input Assembler)

## 생성

사용 구조체:
- Input Element Desc (`D3D11_INPUT_ELEMENT_DESC`)

`D3D11_INPUT_ELEMENT_DESC` 주요 값:
- `SemanticName`
  - HLSL 입력 이름.
  - 예: `"POSITION"`, `"COLOR"`, `"TEXCOORD"`
- `SemanticIndex`
  - 같은 Semantic이 여러 개일 때 구분하는 번호.
- `Format`
  - 데이터 포맷.
  - 예: `DXGI_FORMAT_R32G32B32_FLOAT`
- `InputSlot`
  - 어느 Vertex Buffer 슬롯에서 읽을지.
- `AlignedByteOffset`
  - Vertex 구조체 안에서 해당 값이 시작되는 위치.
  - 예: `offsetof(Vertex, position)`
- `InputSlotClass`
  - 정점 단위 데이터인지, 인스턴스 단위 데이터인지.
  - 보통 `D3D11_INPUT_PER_VERTEX_DATA`.
- `InstanceDataStepRate`
  - 인스턴싱에서 사용. 일반 정점 데이터는 `0`.

생성 함수:

```cpp
D3D11_INPUT_ELEMENT_DESC layout[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

device->CreateInputLayout(
    layout,
    ARRAYSIZE(layout),
    vertexShaderBlob->GetBufferPointer(),
    vertexShaderBlob->GetBufferSize(),
    inputLayout.GetAddressOf()
);
```

## 적용

```cpp
context->IASetInputLayout(inputLayout.Get());
```

## 헷갈리기 쉬운 부분

- Input Layout은 Vertex Shader 입력과 맞춰서 만든다.
- 그래서 Pixel Shader Blob이 아니라 Vertex Shader Blob이 필요하다.
- `SemanticName`, `Format`, `Offset`이 C++ Vertex 구조체와 HLSL 입력 구조에 맞아야 한다.

---

# Primitive Topology

## 역할

Primitive Topology (`D3D11_PRIMITIVE_TOPOLOGY`)는 정점들을 어떤 도형으로 해석할지 정한다.

파이프라인 위치:
- IA(Input Assembler)

## 적용

생성 객체는 따로 없다. 값만 DeviceContext에 설정한다.

```cpp
context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
```

자주 쓰는 값:
- `D3D11_PRIMITIVE_TOPOLOGY_POINTLIST`
  - 점 목록.
- `D3D11_PRIMITIVE_TOPOLOGY_LINELIST`
  - 선 목록.
- `D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST`
  - 삼각형 목록.

## 헷갈리기 쉬운 부분

- Vertex Buffer만 연결한다고 삼각형이 되는 것은 아니다.
- Topology가 있어야 정점들을 점, 선, 삼각형 중 무엇으로 해석할지 결정된다.

---

# Viewport

## 역할

Viewport (`D3D11_VIEWPORT`)는 렌더링 결과를 화면의 어느 영역에 그릴지 정한다.

파이프라인 위치:
- RS(Rasterizer)

## 생성

Viewport는 DirectX 객체로 생성하지 않고 구조체 값을 만든 뒤 바로 적용한다.

`D3D11_VIEWPORT` 주요 값:
- `TopLeftX`, `TopLeftY`
  - Viewport 시작 위치.
- `Width`, `Height`
  - Viewport 크기.
- `MinDepth`, `MaxDepth`
  - 깊이 범위.
  - 보통 `0.0f`, `1.0f`.

```cpp
D3D11_VIEWPORT viewport = {};
viewport.TopLeftX = 0.0f;
viewport.TopLeftY = 0.0f;
viewport.Width = static_cast<float>(width);
viewport.Height = static_cast<float>(height);
viewport.MinDepth = 0.0f;
viewport.MaxDepth = 1.0f;
```

## 적용

```cpp
context->RSSetViewports(1, &viewport);
```

## 헷갈리기 쉬운 부분

- Viewport는 Buffer나 Shader처럼 `Create...`로 만들지 않는다.
- Rasterizer 단계에서 NDC 좌표를 실제 화면 영역으로 옮기는 데 관여한다.

---

# Render Target View

## 역할

Render Target View (`ID3D11RenderTargetView`)는 Back Buffer를 렌더링 결과가 기록될 대상으로 사용할 수 있게 해주는 View다.

파이프라인 위치:
- OM(Output Merger)

## 생성

생성에 필요한 것:
- Back Buffer (`ID3D11Texture2D`)

Back Buffer 가져오기:

```cpp
ComPtr<ID3D11Texture2D> backBuffer;

swapChain->GetBuffer(
    0,
    __uuidof(ID3D11Texture2D),
    reinterpret_cast<void**>(backBuffer.GetAddressOf())
);
```

RTV 생성:

```cpp
device->CreateRenderTargetView(
    backBuffer.Get(),
    nullptr,
    renderTargetView.GetAddressOf()
);
```

## 적용

```cpp
context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);
```

## Clear

```cpp
float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
context->ClearRenderTargetView(renderTargetView.Get(), clearColor);
```

## 헷갈리기 쉬운 부분

- Back Buffer 자체에 바로 그리는 것이 아니라 RTV를 통해 렌더링 대상으로 묶는다.
- RTV는 Output Merger 단계에 적용한다.

---

# Texture2D / Shader Resource View

## 역할

- Texture2D (`ID3D11Texture2D`)
  - 이미지 데이터를 GPU 리소스로 가진다.
- Shader Resource View (`ID3D11ShaderResourceView`)
  - 셰이더가 Texture2D를 읽을 수 있게 해주는 View다.

파이프라인 위치:
- 주로 PS(Pixel Shader)

## Texture2D 생성

사용 구조체:
- Texture2D Desc (`D3D11_TEXTURE2D_DESC`)
- Subresource Data (`D3D11_SUBRESOURCE_DATA`)

`D3D11_TEXTURE2D_DESC` 주요 값:
- `Width`, `Height`
  - 텍스처 크기.
- `MipLevels`
  - MipMap 레벨 수.
- `ArraySize`
  - 텍스처 배열 개수. 일반 2D 텍스처는 `1`.
- `Format`
  - 픽셀 포맷.
- `SampleDesc.Count`
  - 멀티 샘플링 개수. 일반 텍스처는 보통 `1`.
- `Usage`
  - 사용 방식.
- `BindFlags`
  - 셰이더에서 읽으려면 `D3D11_BIND_SHADER_RESOURCE`.

`D3D11_SUBRESOURCE_DATA` 주요 값:
- `pSysMem`
  - 이미지 픽셀 데이터 시작 주소.
- `SysMemPitch`
  - 한 줄의 바이트 크기.
  - 예: `width * bytesPerPixel`
- `SysMemSlicePitch`
  - 3D 텍스처 등에서 사용.

생성 함수:

```cpp
device->CreateTexture2D(&textureDesc, &data, texture.GetAddressOf());
```

## SRV 생성

사용 구조체:
- Shader Resource View Desc (`D3D11_SHADER_RESOURCE_VIEW_DESC`)

`D3D11_SHADER_RESOURCE_VIEW_DESC` 주요 값:
- `Format`
  - 리소스를 어떤 포맷으로 읽을지.
- `ViewDimension`
  - 어떤 형태의 리소스인지.
  - 예: `D3D11_SRV_DIMENSION_TEXTURE2D`
- `Texture2D.MipLevels`
  - 사용할 MipMap 개수.
- `Texture2D.MostDetailedMip`
  - 시작 MipMap 레벨.

생성 함수:

```cpp
device->CreateShaderResourceView(
    texture.Get(),
    &srvDesc,
    shaderResourceView.GetAddressOf()
);
```

## 적용

```cpp
context->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
```

## 헷갈리기 쉬운 부분

- Texture2D는 실제 이미지 리소스다.
- SRV는 셰이더가 그 리소스를 읽기 위한 View다.
- Pixel Shader에서 `Texture2D.Sample(...)`을 하려면 SRV와 Sampler State가 같이 필요하다.

---

# Sampler State

## 역할

Sampler State (`ID3D11SamplerState`)는 텍스처를 샘플링할 때 필터링과 주소 모드를 정한다.

파이프라인 위치:
- 주로 PS(Pixel Shader)

## 생성

사용 구조체:
- Sampler Desc (`D3D11_SAMPLER_DESC`)

`D3D11_SAMPLER_DESC` 주요 값:
- `Filter`
  - 확대/축소 시 보간 방식.
  - 예: `D3D11_FILTER_MIN_MAG_MIP_LINEAR`
- `AddressU`, `AddressV`, `AddressW`
  - UV 범위를 벗어났을 때 처리 방식.
  - 예: `D3D11_TEXTURE_ADDRESS_WRAP`, `D3D11_TEXTURE_ADDRESS_CLAMP`
- `ComparisonFunc`
  - 비교 샘플링에서 사용.
- `MinLOD`, `MaxLOD`
  - 사용할 MipMap 레벨 범위.

생성 함수:

```cpp
D3D11_SAMPLER_DESC desc = {};
desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
desc.MinLOD = 0;
desc.MaxLOD = D3D11_FLOAT32_MAX;

device->CreateSamplerState(&desc, samplerState.GetAddressOf());
```

## 적용

```cpp
context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
```

## 헷갈리기 쉬운 부분

- SRV는 어떤 텍스처를 읽을지 정한다.
- Sampler State는 그 텍스처를 어떻게 읽을지 정한다.

---

# Rasterizer State

## 역할

Rasterizer State (`ID3D11RasterizerState`)는 삼각형을 픽셀 후보로 바꾸는 Rasterizer 단계의 규칙을 정한다.

파이프라인 위치:
- RS(Rasterizer)

## 생성

사용 구조체:
- Rasterizer Desc (`D3D11_RASTERIZER_DESC`)

`D3D11_RASTERIZER_DESC` 주요 값:
- `FillMode`
  - 면을 채울지, 와이어프레임으로 볼지.
  - 예: `D3D11_FILL_SOLID`, `D3D11_FILL_WIREFRAME`
- `CullMode`
  - 어떤 면을 그리지 않을지.
  - 예: `D3D11_CULL_BACK`, `D3D11_CULL_FRONT`, `D3D11_CULL_NONE`
- `FrontCounterClockwise`
  - 반시계 방향 정점을 앞면으로 볼지.
- `DepthClipEnable`
  - 깊이 클리핑 사용 여부.
- `ScissorEnable`
  - Scissor Rect 사용 여부.
- `MultisampleEnable`
  - 멀티 샘플링 사용 여부.

생성 함수:

```cpp
D3D11_RASTERIZER_DESC desc = {};
desc.FillMode = D3D11_FILL_SOLID;
desc.CullMode = D3D11_CULL_BACK;
desc.FrontCounterClockwise = false;
desc.DepthClipEnable = true;

device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
```

## 적용

```cpp
context->RSSetState(rasterizerState.Get());
```

## 헷갈리기 쉬운 부분

- Rasterizer State는 Viewport와 같은 RS 단계에 적용된다.
- CullMode 때문에 도형이 안 보이는 경우가 많다.
- 정점 순서가 시계/반시계인지에 따라 앞면 판정이 바뀐다.

---

# Blend State

## 역할

Blend State (`ID3D11BlendState`)는 새로 그리는 픽셀과 이미 Render Target에 있는 픽셀을 어떻게 섞을지 정한다.

파이프라인 위치:
- OM(Output Merger)

## 생성

사용 구조체:
- Blend Desc (`D3D11_BLEND_DESC`)

`D3D11_BLEND_DESC` 주요 값:
- `AlphaToCoverageEnable`
  - MSAA에서 Alpha To Coverage 사용 여부.
- `IndependentBlendEnable`
  - Render Target마다 다른 Blend 설정을 쓸지.
- `RenderTarget[0].BlendEnable`
  - Blend 사용 여부.
- `RenderTarget[0].SrcBlend`
  - 새로 그리는 색상의 비율.
- `RenderTarget[0].DestBlend`
  - 기존 색상의 비율.
- `RenderTarget[0].BlendOp`
  - 색상 Blend 연산.
- `RenderTarget[0].SrcBlendAlpha`
  - Alpha에 대한 Source Blend.
- `RenderTarget[0].DestBlendAlpha`
  - Alpha에 대한 Destination Blend.
- `RenderTarget[0].BlendOpAlpha`
  - Alpha Blend 연산.
- `RenderTarget[0].RenderTargetWriteMask`
  - 어떤 색상 채널에 쓸지.

생성 함수:

```cpp
D3D11_BLEND_DESC desc = {};
desc.RenderTarget[0].BlendEnable = true;
desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

device->CreateBlendState(&desc, blendState.GetAddressOf());
```

## 적용

```cpp
float blendFactor[4] = { 0, 0, 0, 0 };
UINT sampleMask = 0xffffffff;

context->OMSetBlendState(blendState.Get(), blendFactor, sampleMask);
```

## 헷갈리기 쉬운 부분

- Blend State는 투명도 표현에 자주 사용된다.
- Pixel Shader에서 alpha를 내보내도 Blend State가 꺼져 있으면 섞이지 않는다.
- Blend는 Output Merger 단계에서 일어난다.

---

# Depth Stencil Buffer / View / State

## 역할

- Depth Stencil Buffer (`ID3D11Texture2D`)
  - 픽셀의 깊이값과 스텐실 값을 저장하는 Texture2D 리소스다.
- Depth Stencil View (`ID3D11DepthStencilView`)
  - Depth Stencil Buffer를 OM 단계에서 사용할 수 있게 해주는 View다.
- Depth Stencil State (`ID3D11DepthStencilState`)
  - 깊이 테스트와 스텐실 테스트 규칙을 정한다.

파이프라인 위치:
- OM(Output Merger)

## Depth Stencil Buffer 생성

사용 구조체:
- Texture2D Desc (`D3D11_TEXTURE2D_DESC`)

주요 값:
- `Width`, `Height`
  - Render Target과 같은 크기로 맞춘다.
- `MipLevels`
  - 보통 `1`.
- `ArraySize`
  - 보통 `1`.
- `Format`
  - 예: `DXGI_FORMAT_D24_UNORM_S8_UINT`
- `BindFlags`
  - `D3D11_BIND_DEPTH_STENCIL`

```cpp
D3D11_TEXTURE2D_DESC desc = {};
desc.Width = width;
desc.Height = height;
desc.MipLevels = 1;
desc.ArraySize = 1;
desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
desc.SampleDesc.Count = 1;
desc.Usage = D3D11_USAGE_DEFAULT;
desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

device->CreateTexture2D(&desc, nullptr, depthStencilBuffer.GetAddressOf());
```

## Depth Stencil View 생성

```cpp
device->CreateDepthStencilView(
    depthStencilBuffer.Get(),
    nullptr,
    depthStencilView.GetAddressOf()
);
```

## Depth Stencil State 생성

사용 구조체:
- Depth Stencil Desc (`D3D11_DEPTH_STENCIL_DESC`)

주요 값:
- `DepthEnable`
  - 깊이 테스트 사용 여부.
- `DepthWriteMask`
  - 깊이값 기록 여부.
  - 예: `D3D11_DEPTH_WRITE_MASK_ALL`
- `DepthFunc`
  - 깊이 비교 방식.
  - 예: `D3D11_COMPARISON_LESS`
- `StencilEnable`
  - 스텐실 테스트 사용 여부.

```cpp
D3D11_DEPTH_STENCIL_DESC desc = {};
desc.DepthEnable = true;
desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
desc.DepthFunc = D3D11_COMPARISON_LESS;
desc.StencilEnable = false;

device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
```

## 적용

RTV와 DSV를 함께 적용:

```cpp
context->OMSetRenderTargets(
    1,
    renderTargetView.GetAddressOf(),
    depthStencilView.Get()
);
```

Depth Stencil State 적용:

```cpp
context->OMSetDepthStencilState(depthStencilState.Get(), 0);
```

Clear:

```cpp
context->ClearDepthStencilView(
    depthStencilView.Get(),
    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
    1.0f,
    0
);
```

## 헷갈리기 쉬운 부분

- 깊이 테스트는 가까운 물체가 먼 물체를 가리게 만드는 데 필요하다.
- Depth Stencil Buffer는 Texture2D 리소스이고, DSV는 그 리소스를 OM 단계에 연결하기 위한 View다.
- RTV와 DSV는 보통 `OMSetRenderTargets`에서 함께 묶는다.

---

# State 객체 공통 정리

## 역할

State 객체는 파이프라인의 특정 단계가 동작하는 규칙을 묶어둔 객체다.

대표 State:
- Rasterizer State (`ID3D11RasterizerState`)
- Sampler State (`ID3D11SamplerState`)
- Blend State (`ID3D11BlendState`)
- Depth Stencil State (`ID3D11DepthStencilState`)

## 공통 흐름

```cpp
// 1. Desc 작성
D3D11_XXX_DESC desc = {};

// 2. Device로 State 생성
device->CreateXXXState(&desc, state.GetAddressOf());

// 3. DeviceContext로 해당 파이프라인 단계에 적용
context->XXSetXXXState(...);
```

## 적용 위치

- Rasterizer State
  - `context->RSSetState(...)`
- Sampler State
  - `context->PSSetSamplers(...)`
- Blend State
  - `context->OMSetBlendState(...)`
- Depth Stencil State
  - `context->OMSetDepthStencilState(...)`

## 헷갈리기 쉬운 부분

- State 객체는 데이터를 담는 버퍼가 아니라, 파이프라인 동작 규칙이다.
- State를 만들었다고 자동 적용되는 것은 아니다.
- 생성 후 반드시 알맞은 `Set...` 함수로 적용해야 한다.

---

# 빠른 체크리스트

## 초기화 단계에서 자주 생성하는 것

- Device (`ID3D11Device`)
- DeviceContext (`ID3D11DeviceContext`)
- SwapChain (`IDXGISwapChain`)
- Render Target View (`ID3D11RenderTargetView`)
- Depth Stencil Buffer (`ID3D11Texture2D`)
- Depth Stencil View (`ID3D11DepthStencilView`)
- Viewport (`D3D11_VIEWPORT`)
- Vertex Buffer (`ID3D11Buffer`)
- Index Buffer (`ID3D11Buffer`)
- Constant Buffer (`ID3D11Buffer`)
- Shader Blob (`ID3DBlob`)
- Vertex Shader (`ID3D11VertexShader`)
- Pixel Shader (`ID3D11PixelShader`)
- Input Layout (`ID3D11InputLayout`)
- Texture2D (`ID3D11Texture2D`)
- Shader Resource View (`ID3D11ShaderResourceView`)
- Sampler State (`ID3D11SamplerState`)
- Rasterizer State (`ID3D11RasterizerState`)
- Blend State (`ID3D11BlendState`)
- Depth Stencil State (`ID3D11DepthStencilState`)

## Render 단계에서 자주 적용하는 것

```cpp
context->OMSetRenderTargets(...);
context->ClearRenderTargetView(...);
context->ClearDepthStencilView(...);

context->RSSetViewports(...);
context->RSSetState(...);

context->IASetInputLayout(...);
context->IASetVertexBuffers(...);
context->IASetIndexBuffer(...);
context->IASetPrimitiveTopology(...);

context->VSSetShader(...);
context->VSSetConstantBuffers(...);

context->PSSetShader(...);
context->PSSetShaderResources(...);
context->PSSetSamplers(...);
context->PSSetConstantBuffers(...);

context->OMSetBlendState(...);
context->OMSetDepthStencilState(...);

context->Draw(...);
context->DrawIndexed(...);

swapChain->Present(...);
```

---

# 한 줄 정리

DirectX11 객체 정리는 **Desc로 설정을 적고, Device로 만들고, DeviceContext로 파이프라인에 꽂는다**는 흐름으로 보면 된다.
