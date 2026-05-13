# DirectX11 짧은 학습 기록 - Component

## 수강 범위

- 섹션: DirectX11 엔진 구조
- 강의: Component

## 오늘 만든 것

- Unity의 GameObject처럼 컴포넌트를 붙여 동작하는 `GameObject` 구조를 만들었다.
- 필수 컴포넌트와 Script 계열 컴포넌트를 `enum class ComponentType`으로 구분했다.
- 고정 컴포넌트는 `array`, 여러 개 붙을 수 있는 Script는 `vector`로 관리하도록 나누었다.
- `Transform`, `MonoBehavior`, `Camera`를 `Component` 기반 구조로 만들었다.
- Camera Component에서 View Matrix와 Projection Matrix를 계산하는 `UpdateMatrix()`를 만들었다.

## 핵심 흐름

```text
1. ComponentType으로 Transform, Camera, Script 같은 컴포넌트 종류를 정의한다.
2. GameObject는 고정 컴포넌트를 array에, Script 컴포넌트를 vector에 저장한다.
3. AddComponent()로 Component와 GameObject를 서로 연결한다.
4. GameObject::Update()에서 Component들의 Update()를 돌리고 TransformData를 갱신한다.
5. Camera::UpdateMatrix()에서 카메라 Transform을 기준으로 View/Projection Matrix를 만든다.
```

## 핵심 코드

```cpp
enum class ComponentType : uint8
{
    Transform,
    MeshRenderer,
    Camera,
    Animator,
    Script,

    End,
};

enum
{
    FIXED_COMPONENT_COUNT = static_cast<uint8>(ComponentType::End) - 1
};
```

```cpp
array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
vector<shared_ptr<MonoBehavior>> _scripts;
```

```cpp
void GameObject::AddComponent(shared_ptr<Component> component)
{
    component->SetGameObject(shared_from_this());

    uint8 index = static_cast<uint8>(component->GetType());
    if (index < FIXED_COMPONENT_COUNT)
        _components[index] = component;
    else
        _scripts.push_back(dynamic_pointer_cast<MonoBehavior>(component));
}
```

```cpp
void Camera::UpdateMatrix()
{
    Vec3 eyePosition = GetTransform()->GetPosition();
    Vec3 focusPosition = eyePosition + GetTransform()->GetLook();
    Vec3 upDirection = GetTransform()->GetUp();

    S_MatView = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
    S_MatProjection = ::XMMatrixOrthographicLH(800.0f, 600.0f, 0.0f, 1.0f);
}
```

## 렌더링 파이프라인에서 위치

- IA: `GameObject`가 가진 Geometry, Vertex Buffer, Index Buffer, Input Layout을 `Pipeline`에 넘긴다.
- VS: `TransformData`의 World/View/Projection Matrix를 이용해 정점 위치를 변환할 준비를 한다.
- RS: `RasterizerState`를 `PipelineInfo`에 담아 Rasterizer 단계에 바인딩한다.
- PS: Texture (`ID3D11ShaderResourceView`)와 Sampler (`ID3D11SamplerState`)를 Pixel Shader 단계에 바인딩한다.
- OM: Blend State (`ID3D11BlendState`)를 설정하고 최종 색을 Render Target에 기록한다.

## 오늘 이해한 것

- Component는 기능 단위이고, GameObject는 그 기능들을 담는 컨테이너 역할을 한다.
- Transform (`Transform`)은 모든 오브젝트가 기본적으로 가져야 하므로 고정 컴포넌트 쪽에 둔다.
- Script (`MonoBehavior`)는 여러 개 붙을 수 있으므로 `vector`로 따로 관리한다.
- Camera (`Camera`)도 하나의 Component로 만들면 GameObject에 붙여서 위치와 방향을 Transform으로 제어할 수 있다.
- `Game.cpp`에 몰려 있던 DirectX 리소스 생성과 파이프라인 바인딩 코드가 `Graphics`, `Pipeline`, `GameObject` 쪽으로 분리되었다.

## 헷갈린 부분

- `array`는 정해진 종류의 컴포넌트를 빠르게 찾기 위한 고정 슬롯이고, `vector`는 개수가 변하는 Script 목록이다.
- Camera에서 View/Projection Matrix를 계산했지만, 아직 `TransformData`의 `matView`, `matProjection`에 연결되는 흐름은 완성되지 않았다.

## 다시 볼 것

- `shared_from_this()`로 Component가 자기 GameObject를 참조하게 만드는 흐름
- `Transform::UpdateTransform()`에서 Local Matrix와 Parent World Matrix를 곱하는 순서
- `Camera::UpdateMatrix()`에서 `XMMatrixLookAtLH`, `XMMatrixOrthographicLH`, `XMMatrixPerspectiveFovLH`가 하는 역할
- Camera가 만든 View/Projection Matrix를 Constant Buffer (`ID3D11Buffer`)에 넘기는 다음 단계

## 한 줄 정리

- 오늘은 GameObject에 Component를 붙이는 기본 구조를 만들고, Transform과 Camera를 컴포넌트 시스템 안으로 넣어 Unity식 엔진 구조의 첫 뼈대를 세웠다.
