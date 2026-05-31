# DirectX11 짧은 학습 기록 - Transform과 행렬

## 수강 범위

- 섹션: 3. 행렬 이론
- 연결된 실습: 섹션 4. 프레임워크 제작 중 `Transform` 적용

## 오늘 만든 것

- `Transform` 컴포넌트에 위치, 회전, 크기 값을 저장했다.
- Local Matrix와 World Matrix를 계산했다.
- 부모-자식 Transform 계층을 만들었다.
- 계산된 World Matrix를 Constant Buffer (`ID3D11Buffer`)로 보내 Vertex Shader에서 사용하게 했다.

## 핵심 흐름

```text
1. Local Position / Rotation / Scale 값을 가진다.
2. Scale, Rotation, Translation Matrix를 만든다.
3. S * R * T 순서로 Local Matrix를 만든다.
4. 부모가 있으면 Local Matrix * Parent World Matrix로 World Matrix를 만든다.
5. World Matrix를 TransformData.matWorld에 넣는다.
6. Constant Buffer로 GPU에 복사한다.
7. Vertex Shader에서 matWorld를 정점 위치에 곱한다.
```

## 핵심 코드

```cpp
Matrix matScale = Matrix::CreateScale(_localScale);
Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
matRotation *= Matrix::CreateRotationY(_localRotation.y);
matRotation *= Matrix::CreateRotationZ(_localRotation.z);
Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

_matLocal = matScale * matRotation * matTranslation;

if (HasParent())
    _matWorld = _matLocal * _parent->GetWorldMatrix();
else
    _matWorld = _matLocal;
```

```cpp
_transformData.matWorld = _transform->GetWorldMatrix();
_constantBuffer->CopyData(_transformData);
```

```hlsl
float4 position = mul(input.position, matWorld);
position = mul(position, matView);
position = mul(position, matProjection);
```

## 렌더링 파이프라인에서 위치

- IA: 정점 데이터를 준비한다.
- VS: Transform의 `matWorld`를 정점 위치에 적용한다.
- RS: 변환된 삼각형을 픽셀 후보로 바꾼다.
- PS: 픽셀 색상을 계산한다.
- OM: 최종 색상을 Render Target에 기록한다.

Transform 자체는 DirectX11 API 객체가 아니라, VS(Vertex Shader)에 필요한 행렬 데이터를 준비하는 CPU 쪽 시스템이다.

## 오늘 이해한 것

- Local 값은 부모 기준 위치, 회전, 크기다.
- World 값은 최종 월드 공간 기준 위치, 회전, 크기다.
- 부모가 없으면 `World Matrix = Local Matrix`다.
- 부모가 있으면 `World Matrix = Local Matrix * Parent World Matrix`다.
- `S * R * T`는 정점에 크기 변경, 회전, 이동 순서로 적용된다.
- World Matrix는 Constant Buffer를 통해 HLSL로 전달된다.

## 헷갈린 부분

- Local Position과 World Position은 부모가 생기면 달라진다.
- 행렬 곱셈 순서가 바뀌면 결과도 바뀐다.
- `SetPosition(worldPosition)`은 월드 값을 받지만, 내부에서는 부모 기준 Local Position으로 바꿔 저장한다.
- 방향 벡터 `Right`, `Up`, `Forward`는 위치가 아니라 방향이므로 일반 위치 변환과 다르게 봐야 한다.

## 다시 볼 것

- `S * R * T`와 `T * R * S`의 결과 차이
- 부모 Transform을 움직였을 때 자식 World Matrix가 바뀌는 과정
- 부모 World Matrix의 역행렬로 월드 좌표를 로컬 좌표로 바꾸는 이유
- View Matrix, Projection Matrix가 추가되면 `World -> View -> Projection` 흐름이 어떻게 이어지는지

## 한 줄 정리

- `Transform`은 위치, 회전, 크기를 행렬로 합쳐 World Matrix를 만들고, 그 값을 Vertex Shader에 보내 오브젝트 정점 위치를 바꾸는 역할을 한다.
