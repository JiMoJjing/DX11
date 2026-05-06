# DirectX11 입문 학습 기록 - 제목

## 수강 범위

- 강의: 인프런 [게임 프로그래머 도약반] DirectX11 입문
- 섹션:
- 강의:
   - N강:
   - N강:
   - N강:

## 오늘 만든 것

- 
- 
- 

---

# 핵심 흐름 정리

## 초기화

### 장치 초기화

- Device (`ID3D11Device`), DeviceContext (`ID3D11DeviceContext`), SwapChain (`IDXGISwapChain`) 생성
  - 핵심 API: `D3D11CreateDeviceAndSwapChain`

```cpp
// 핵심 코드만 붙여넣기
```

- SwapChain (`IDXGISwapChain`)의 Back Buffer (`ID3D11Texture2D`)를 이용해 Render Target View (`ID3D11RenderTargetView`) 생성
  - 핵심 API: `GetBuffer`, `CreateRenderTargetView`

```cpp
// 핵심 코드만 붙여넣기
```

- Viewport (`D3D11_VIEWPORT`) 설정

```cpp
// 핵심 코드만 붙여넣기
```

---

### 지오메트리 생성

- 모델링 정점 정보 (`Vertex`)를 기반으로 Vertex Buffer (`ID3D11Buffer`) 생성
  - 핵심 구조체: `D3D11_BUFFER_DESC`, `D3D11_SUBRESOURCE_DATA`
  - 핵심 API: `CreateBuffer`

```cpp
// 핵심 코드만 붙여넣기
```

---

### 셰이더 생성

- 셰이더 파일 (`.hlsl`)을 컴파일해서 Shader Blob (`ID3DBlob`) 생성
  - 핵심 API: `D3DCompileFromFile`

```cpp
// 핵심 코드만 붙여넣기
```

- Shader Blob (`ID3DBlob`)을 이용해 Vertex Shader (`ID3D11VertexShader`), Pixel Shader (`ID3D11PixelShader`) 생성
  - 핵심 API: `CreateVertexShader`, `CreatePixelShader`

```cpp
// 핵심 코드만 붙여넣기
```

---

### 인풋 레이아웃 생성

- 입력 레이아웃 정보 (`D3D11_INPUT_ELEMENT_DESC`) 정의
- Vertex Shader Blob (`ID3DBlob`)을 이용하여 Input Layout (`ID3D11InputLayout`) 생성
  - 핵심 API: `CreateInputLayout`

```cpp
// 핵심 코드만 붙여넣기
```

---

## 렌더링 반복

### 그리기 전 세팅

- Render Target View (`ID3D11RenderTargetView`)를 OM 단계에 설정
  - 핵심 API: `OMSetRenderTargets`
- Render Target View 비우기
  - 핵심 API: `ClearRenderTargetView`
- Viewport (`D3D11_VIEWPORT`) 설정
  - 핵심 API: `RSSetViewports`

```cpp
// 핵심 코드만 붙여넣기
```

### 그리는 과정

- Vertex Buffer (`ID3D11Buffer`) 설정
  - 핵심 API: `IASetVertexBuffers`
- Input Layout (`ID3D11InputLayout`) 설정
  - 핵심 API: `IASetInputLayout`
- 그리는 모양 설정
  - Primitive Topology (`D3D11_PRIMITIVE_TOPOLOGY`)
  - 핵심 API: `IASetPrimitiveTopology`
- Vertex Shader (`ID3D11VertexShader`) 설정
  - 핵심 API: `VSSetShader`
- Pixel Shader (`ID3D11PixelShader`) 설정
  - 핵심 API: `PSSetShader`
- `Draw` 호출

```cpp
// 핵심 코드만 붙여넣기
```

### 다 그렸으면

- SwapChain (`IDXGISwapChain`)의 `Present` 호출

```cpp
// 핵심 코드만 붙여넣기
```

---

## DirectX11 렌더링 파이프라인 메모

### IA - Input Assembler

- Vertex Buffer를 GPU 파이프라인에 공급한다.
- Input Layout을 통해 C++ 정점 구조체와 HLSL 입력 구조체를 연결한다.
- Primitive Topology로 정점을 어떤 도형으로 해석할지 정한다.

### VS - Vertex Shader

- 정점 단위로 실행된다.
- 

### RS - Rasterizer

- 정점으로 구성된 도형을 픽셀 후보로 변환한다.
- 

### PS - Pixel Shader

- 픽셀 단위로 실행된다.
- 

### OM - Output Merger

- 최종 결과를 Render Target에 기록한다.
- 

---

## 오늘 이해한 개념

- Device (`ID3D11Device`)
  - GPU 리소스를 생성하는 객체.
  - Buffer, Shader, Input Layout, Render Target View 같은 DirectX 리소스를 만들 때 사용한다.

- DeviceContext (`ID3D11DeviceContext`)
  - GPU에게 렌더링 명령을 내리는 객체.
  - Vertex Buffer 바인딩, Shader 설정, Viewport 설정, Draw 호출 등을 담당한다.

- SwapChain (`IDXGISwapChain`)
  - Back Buffer와 Front Buffer를 교체해서 최종 화면을 출력하는 객체.
  - `Present()`를 호출하면 Back Buffer에 그린 결과가 화면에 표시된다.

- Back Buffer (`ID3D11Texture2D`)
  - 화면에 표시되기 전 렌더링 결과가 그려지는 버퍼.
  - SwapChain에서 가져와 Render Target View를 만들 때 사용한다.

- Render Target View (`ID3D11RenderTargetView`)
  - Back Buffer를 렌더링 대상으로 사용하기 위한 뷰.
  - Output Merger 단계에서 렌더링 결과가 기록되는 대상이다.

- Viewport (`D3D11_VIEWPORT`)
  - 렌더링 결과를 화면의 어느 영역에 그릴지 정하는 설정.
  - 위치, 너비, 높이, 최소/최대 깊이 범위를 가진다.

- Vertex (`Vertex`)
  - 정점 하나의 데이터를 표현하는 구조체.
  - 현재는 position과 color를 가진다.

- Vertex Buffer (`ID3D11Buffer`)
  - 정점 데이터를 GPU 메모리에 올려두는 버퍼.
  - `D3D11_BIND_VERTEX_BUFFER` 용도로 생성한다.

- Buffer Desc (`D3D11_BUFFER_DESC`)
  - 버퍼를 어떤 용도와 크기로 만들지 설명하는 구조체.
  - Usage, BindFlags, ByteWidth 등을 설정한다.

- Subresource Data (`D3D11_SUBRESOURCE_DATA`)
  - 버퍼 생성 시 초기 데이터를 넘겨주는 구조체.
  - `pSysMem`에 CPU 메모리의 데이터 시작 주소를 넣는다.

- Shader Blob (`ID3DBlob`)
  - 컴파일된 셰이더 바이트코드를 담는 객체.
  - Vertex Shader, Pixel Shader, Input Layout 생성에 사용된다.

- Vertex Shader (`ID3D11VertexShader`)
  - 정점 단위로 실행되는 셰이더.
  - 정점 위치 변환, 색상 전달 등의 일을 한다.

- Pixel Shader (`ID3D11PixelShader`)
  - 픽셀 단위로 실행되는 셰이더.
  - 최종 픽셀 색상을 결정한다.

- Input Element Desc (`D3D11_INPUT_ELEMENT_DESC`)
  - 입력 레이아웃의 각 요소를 설명하는 구조체.
  - Semantic Name, Format, Offset 등을 정의한다.

- Input Layout (`ID3D11InputLayout`)
  - C++의 `Vertex` 구조체와 HLSL의 입력 구조체를 연결하는 설명서.
  - `POSITION`, `COLOR` 같은 시맨틱과 메모리 offset을 정의한다.

- Primitive Topology (`D3D11_PRIMITIVE_TOPOLOGY`)
  - 정점들을 어떤 도형으로 해석할지 정하는 설정.
  - 예: `D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST`

---

## 헷갈렸던 부분

- 
- 
- 

## 다시 볼 것

- 
- 
- 

## 실행 결과

- 
- 빌드 결과:
  - 
  - 

## 한 줄 회고

-
