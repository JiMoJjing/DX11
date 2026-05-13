#pragma once

class VertexBuffer
{
public:
    VertexBuffer(ComPtr<ID3D11Device> device);
    ~VertexBuffer();
    
    ComPtr<ID3D11Buffer> GetComPtr() { return _vertexBuffer; }
    uint32 GetStride() { return _stride; }
    uint32 GetOffset() { return _offset; }
    uint32 GetCount() { return _count; }
    
    template<typename T>
    void Create(const vector<T>& vertices)
    {
        _stride = sizeof(T);
        _count = (uint32)vertices.size();
        
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Usage = D3D11_USAGE_IMMUTABLE;                     // GPU의 접근 제한(Read Only, Write X).
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;              // 이 버퍼를 어떠한 용도로 사용? (지금 여기선 정점 버퍼로).
        desc.ByteWidth = (uint32)(_stride * _count);
        
        
        D3D11_SUBRESOURCE_DATA data;
        ZeroMemory(&data, sizeof(data));
        data.pSysMem = vertices.data();                        // 첫 번째 데이터의 주소.
        
        HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
        CHECK(hr);
    }
    
    
private:
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11Buffer> _vertexBuffer;
    
    uint32 _stride = 0;
    uint32 _offset = 0;
    uint32 _count = 0;
};
