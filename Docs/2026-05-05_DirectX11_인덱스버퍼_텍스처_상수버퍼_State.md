# DirectX11 학습 기록 - 인덱스 버퍼, 텍스처, 상수 버퍼, State 맛보기

## 수강 범위

- 강의: 인프런 게임 프로그래머 취업반 DirectX11 입문
- 섹션: 2. DirectX11 입문
  - 6. 텍스처와 UV
  - 7. ConstantBuffer
  - 8. RasterizerState, SamplerState, BlendState
  - 9. 복습
- 강의:
  - 인덱스 버퍼 생성과 `DrawIndexed`
  - 텍스처 좌표와 Shader Resource View
  - Constant Buffer를 통한 셰이더 데이터 전달
  - 렌더링 파이프라인 State 객체 맛보기

## 오늘 만든 것

- 기존 삼각형 렌더링을 사각형 렌더링으로 확장했다.
- Index Buffer (`ID3D11Buffer`)를 만들어 정점 재사용 방식으로 사각형을 그렸다.
- Vertex (`Vertex`) 구조에서 색상(`Color`) 대신 텍스처 좌표(`Vec2 uv`)를 사용하도록 바꿨다.
- Texture2D (`Texture2D`)와 SamplerState (`SamplerState`)를 HLSL에 등록하고, Pixel Shader에서 텍스처를 샘플링했다.
- Shader Resource View (`ID3D11ShaderResourceView`)를 만들어 이미지 파일을 Pixel Shader에 넘겼다.
- Constant Buffer (`ID3D11Buffer`)를 만들어 C++의 `TransformData` 값을 Vertex Shader에 전달했다.
- Rasterizer State (`ID3D11RasterizerState`), Sampler State (`ID3D11SamplerState`), Blend State (`ID3D11BlendState`)를 생성하고 각 파이프라인 단계에 바인딩했다.

---

# 핵심 흐름 정리

## 인덱스 버퍼

### 인덱스 버퍼를 쓰는 이유

- 사각형은 삼각형 2개로 만들 수 있다.
- 삼각형 2개를 정점만으로 만들면 겹치는 정점을 중복해서 넣어야 한다.
- Index Buffer (`ID3D11Buffer`)를 사용하면 정점은 4개만 만들고, 인덱스 6개로 삼각형 2개를 구성할 수 있다.

```cpp
_indices = { 0, 1, 2, 2, 1, 3 };
```

### 인덱스 버퍼 생성

- `D3D11_BUFFER_DESC`의 `BindFlags`를 `D3D11_BIND_INDEX_BUFFER`로 설정한다.
- `D3D11_SUBRESOURCE_DATA`의 `pSysMem`에 인덱스 배열 주소를 넣는다.
- Device (`ID3D11Device`)의 `CreateBuffer`로 GPU Buffer를 만든다.

```cpp
desc.Usage = D3D11_USAGE_IMMUTABLE;
desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
desc.ByteWidth = sizeof(uint32) * _indices.size();

data.pSysMem = _indices.data();

_device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
```

### Render에서 사용

- IA(Input Assembler) 단계에 Index Buffer를 연결한다.
- 기존 `Draw` 대신 `DrawIndexed`를 호출한다.

```cpp
_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
_deviceContext->DrawIndexed((uint32)_indices.size(), 0, 0);
```

---

## 텍스처

### Vertex 구조 변경

- 기존에는 정점마다 색상(`Color`)을 가지고 있었다.
- 텍스처를 입히기 위해 색상 대신 UV 좌표(`Vec2 uv`)를 넣었다.
- UV는 텍스처 이미지의 어느 위치를 읽을지 나타내는 좌표다.

```cpp
struct Vertex
{
    Vec3 position;
    Vec2 uv;
};
```

### Input Layout 수정

- C++의 `Vertex` 구조와 HLSL의 입력 구조를 맞추기 위해 Input Layout (`ID3D11InputLayout`)도 수정했다.
- `COLOR` Semantic 대신 `TEXCOORD` Semantic을 사용한다.

```cpp
D3D11_INPUT_ELEMENT_DESC layout[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
```

### 지오메트리 수정

- 사각형 정점 4개에 각각 UV 좌표를 넣었다.
- UV를 `0.0f ~ 5.0f` 범위로 줘서 텍스처가 반복되는 효과도 확인했다.

```cpp
_vertices[0].uv = { 0.0f, 5.0f };
_vertices[1].uv = { 0.0f, 0.0f };
_vertices[2].uv = { 5.0f, 5.0f };
_vertices[3].uv = { 5.0f, 0.0f };
```

### 셰이더 수정

- Vertex Shader 입력과 출력에 `uv : TEXCOORD`를 추가했다.
- Pixel Shader에서 Texture2D (`Texture2D`)와 SamplerState (`SamplerState`)를 사용해 텍스처 색상을 읽었다.

```hlsl
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = texture0.Sample(sampler0, input.uv);
    return color;
}
```

### Shader Resource View 생성

- 이미지 파일을 GPU에서 읽을 수 있게 Shader Resource View (`ID3D11ShaderResourceView`)를 만들었다.
- 현재는 `BusilPotato.png`를 WIC로 로드한 뒤 SRV를 생성한다.

```cpp
DirectX::TexMetadata md;
DirectX::ScratchImage image;

LoadFromWICFile(L"BusilPotato.png", WIC_FLAGS_NONE, &md, image);
CreateShaderResourceView(_device.Get(), image.GetImages(), image.GetImageCount(), md,
                         _shaderResourceView.GetAddressOf());
```

### Render에서 Pixel Shader에 전달

- SRV는 Pixel Shader 단계에 연결한다.
- 실제 코드에서는 `PSSetShaderResources`를 사용한다.

```cpp
_deviceContext->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());
```

---

## 상수 버퍼

### TransformData 구조체 생성

- C++에서 셰이더로 넘길 데이터를 담기 위해 `TransformData` 구조체를 만들었다.
- 지금은 위치 이동용 `offset`만 있고, 16바이트 정렬을 맞추기 위해 `float dummy`를 추가했다.

```cpp
struct TransformData
{
    Vec3 offset;
    float dummy;
};
```

### Constant Buffer 생성

- Constant Buffer (`ID3D11Buffer`)는 셰이더에 자주 전달되는 작은 데이터를 담는 버퍼다.
- CPU에서 매 프레임 값을 갱신할 수 있도록 `D3D11_USAGE_DYNAMIC`, `D3D11_CPU_ACCESS_WRITE`를 사용했다.

```cpp
desc.Usage = D3D11_USAGE_DYNAMIC;
desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
desc.ByteWidth = sizeof(TransformData);
desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

_device->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
```

### Update에서 값 복사

- `Map`으로 GPU Buffer에 쓸 수 있는 CPU 접근 주소를 얻는다.
- `memcpy`로 `TransformData` 값을 복사한다.
- `Unmap`으로 쓰기 작업이 끝났다고 알려준다.

```cpp
D3D11_MAPPED_SUBRESOURCE subResource;

_deviceContext->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
memcpy(subResource.pData, &_transformData, sizeof(TransformData));
_deviceContext->Unmap(_constantBuffer.Get(), 0);
```

### Unmap을 안 하면 어떻게 되나?

- `Map`은 "이 리소스를 CPU가 쓰는 중"이라고 잡아두는 작업이다.
- `Unmap`을 호출해야 GPU가 다시 해당 리소스를 정상적으로 읽을 수 있다.
- `Unmap`을 하지 않으면 GPU가 갱신된 데이터를 사용하지 못하거나, 다음 `Map`/렌더링 호출에서 문제가 생길 수 있다.
- 즉 `Map`과 `Unmap`은 반드시 한 쌍으로 써야 한다.

### 셰이더에 상수 버퍼 등록

- HLSL에서는 `cbuffer`를 `register(b0)`에 등록했다.
- C++의 `VSSetConstantBuffers(0, ...)`와 HLSL의 `register(b0)`가 대응된다.

```hlsl
cbuffer TransformData : register(b0)
{
    float4 offset;
}
```

### Render에서 Vertex Shader에 전달

- 현재 `offset`은 Vertex Shader에서 정점 위치에 더해진다.
- 그래서 나중에 `_transformData.offset` 값을 바꾸면 물체 위치를 이동시킬 수 있다.

```cpp
_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
```

```hlsl
output.position = input.position + offset;
```

---

## RasterizerState, SamplerState, BlendState

### Rasterizer State

- Rasterizer State (`ID3D11RasterizerState`)는 RS(Rasterizer) 단계의 동작을 정한다.
- 예를 들어 삼각형을 채워서 그릴지, 선만 보이게 와이어프레임으로 그릴지 정할 수 있다.
- Back-face Culling도 여기서 설정한다.

```cpp
desc.FillMode = D3D11_FILL_SOLID;
desc.CullMode = D3D11_CULL_BACK;

_device->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());
_deviceContext->RSSetState(_rasterizerState.Get());
```

### Sampler State

- Sampler State (`ID3D11SamplerState`)는 텍스처를 어떻게 읽을지 정한다.
- UV가 0~1 범위를 벗어났을 때 반복할지(`WRAP`), 가장자리 색을 늘릴지(`CLAMP`) 같은 설정이 들어간다.
- 필터링 방식도 들어가며, MipMap을 사용할 때 어떤 방식으로 샘플링할지도 연관된다.

```cpp
desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

_device->CreateSamplerState(&desc, _samplerState.GetAddressOf());
_deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());
```

### Blend State

- Blend State (`ID3D11BlendState`)는 OM(Output Merger) 단계에서 최종 색을 Render Target에 합치는 방식을 정한다.
- 알파 블렌딩은 이미 그려진 색과 새로 그릴 색을 알파값 기준으로 섞는 기능이다.

```cpp
desc.RenderTarget[0].BlendEnable = true;
desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

_device->CreateBlendState(&desc, _blendState.GetAddressOf());
_deviceContext->OMSetBlendState(_blendState.Get(), nullptr, 0xFFFFFFFF);
```

---

## DirectX11 렌더링 파이프라인 메모

### IA - Input Assembler

- Vertex Buffer (`ID3D11Buffer`)와 Index Buffer (`ID3D11Buffer`)를 입력으로 받는다.
- Input Layout (`ID3D11InputLayout`)을 통해 C++ 정점 구조체와 HLSL 입력 구조를 연결한다.
- Primitive Topology (`D3D11_PRIMITIVE_TOPOLOGY`)로 정점을 어떤 도형으로 해석할지 정한다.

```cpp
_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
_deviceContext->IASetInputLayout(_inputLayout.Get());
_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
```

### VS - Vertex Shader

- 정점 단위로 실행된다.
- Constant Buffer (`ID3D11Buffer`)에서 받은 `offset`을 정점 위치에 더한다.

```cpp
_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
```

### RS - Rasterizer

- 삼각형을 픽셀 후보로 변환한다.
- Rasterizer State (`ID3D11RasterizerState`)로 채우기 방식, 컬링 방식 등을 제어한다.

```cpp
_deviceContext->RSSetState(_rasterizerState.Get());
```

### PS - Pixel Shader

- 픽셀 단위로 실행된다.
- Shader Resource View (`ID3D11ShaderResourceView`)와 Sampler State (`ID3D11SamplerState`)를 이용해 텍스처 색을 읽는다.

```cpp
_deviceContext->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());
_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
_deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());
```

### OM - Output Merger

- Pixel Shader의 결과를 Render Target에 최종 기록한다.
- Blend State (`ID3D11BlendState`)로 기존 색과 새 색을 섞는 방식을 정한다.

```cpp
_deviceContext->OMSetBlendState(_blendState.Get(), nullptr, 0xFFFFFFFF);
_deviceContext->DrawIndexed((uint32)_indices.size(), 0, 0);
```

---

## 오늘 이해한 개념

- Index Buffer (`ID3D11Buffer`)는 정점을 재사용해서 같은 도형을 더 효율적으로 그리게 해준다.
- `Draw`는 정점 순서대로 그리는 방식이고, `DrawIndexed`는 인덱스 배열을 보고 정점을 조합해서 그리는 방식이다.
- 텍스처를 입히려면 정점에 색상 대신 UV 좌표가 필요하다.
- Input Layout (`ID3D11InputLayout`)은 C++의 정점 메모리 구조와 HLSL 입력 Semantic을 맞춰주는 약속이다.
- Shader Resource View (`ID3D11ShaderResourceView`)는 텍스처 같은 GPU 리소스를 셰이더에서 읽을 수 있게 만든 View다.
- Sampler State (`ID3D11SamplerState`)는 텍스처를 읽는 방법을 정한다.
- Constant Buffer (`ID3D11Buffer`)는 C++에서 셰이더로 매 프레임 바뀔 수 있는 값을 전달할 때 사용한다.
- `Map`과 `Unmap`은 CPU가 GPU 리소스에 데이터를 쓰기 위해 사용하는 연결과 해제 과정이다.
- Rasterizer State, Sampler State, Blend State는 각각 RS, PS, OM 단계의 세부 동작을 정하는 객체다.

## 헷갈렸던 부분

- `PSSetShaderResources` 이름이 길어서 "ResourceView를 넣는다"라고 생각하기 쉬운데, 실제 API 이름은 `PSSetShaderResources`다.
- `Texture2D texture0 : register(t0)`와 C++의 `PSSetShaderResources(0, ...)`가 서로 대응된다.
- `SamplerState sampler0 : register(s0)`와 C++의 `PSSetSamplers(0, ...)`가 서로 대응된다.
- `cbuffer TransformData : register(b0)`와 C++의 `VSSetConstantBuffers(0, ...)`가 서로 대응된다.
- Constant Buffer는 16바이트 단위 정렬을 신경 써야 해서 `Vec3`만 넣기보다 패딩용 `float`을 추가했다.
- `Unmap`을 안 하면 CPU가 리소스를 계속 잡고 있는 상태가 되어 GPU가 정상적으로 읽거나 다음 갱신을 진행하기 어렵다.

## 다시 볼 것

- Index Buffer에서 정점 순서가 바뀌면 삼각형의 앞면/뒷면 판정이 어떻게 바뀌는지 다시 보기
- `D3D11_CULL_BACK`, `D3D11_CULL_FRONT`, `D3D11_CULL_NONE` 차이 확인
- Sampler State의 `WRAP`, `CLAMP`, `BORDER`, `MIRROR` 차이 확인
- MipMap이 왜 필요한지, `D3D11_FILTER_MIN_MAG_MIP_LINEAR`가 어떤 의미인지 다시 보기
- Blend State에서 `SrcBlend`, `DestBlend`, `BlendOp`가 실제 색 계산에 어떻게 들어가는지 다시 보기
- Constant Buffer의 16바이트 정렬 규칙 자세히 보기

## 실행 결과

- 삼각형 대신 사각형을 그리는 구조로 변경했다.
- 사각형에 `BusilPotato.png` 텍스처를 입히는 흐름을 만들었다.
- UV를 0~5 범위로 줘서 Sampler State의 `WRAP` 설정에 따른 타일링 효과를 확인할 수 있는 상태가 되었다.
- Constant Buffer를 통해 Vertex Shader에서 위치 이동 값을 받을 수 있는 구조가 되었다.
- Rasterizer State, Sampler State, Blend State를 생성하고 렌더링 파이프라인의 알맞은 단계에 바인딩했다.

## 한 줄 회고

- 오늘은 단순히 정점 색으로 삼각형을 그리던 단계에서 벗어나, 인덱스, 텍스처, 상수 버퍼, State 객체를 연결하면서 DirectX11 렌더링 파이프라인의 각 단계가 조금씩 더 구체적으로 보이기 시작했다.
