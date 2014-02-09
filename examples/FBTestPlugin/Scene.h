#include "boost/bind.hpp"

template <typename S>
class Shader
{
    typedef HRESULT (__stdcall ID3D10Device::*CreateFn)(const void*, SIZE_T, S**);

    static const CreateFn _create;

public:
    CComPtr<S> ptr;
    boost::scoped_array<char> bytes;
    size_t size;

    Shader() : size(0) {}

    bool Load(const std::wstring& path, ID3D10Device* device)
    {
        bool ok = false;
        FILE* fp = _wfopen(path.data(), L"rb");
        if (fp) {
            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            bytes.reset(new char[size]);
            ok = 1 == fread(bytes.get(), size, 1, fp) && 
                 S_OK == (device->*_create)(bytes.get(), size, &ptr);
            fclose(fp);
        }
        return ok;
    }
};

template<> const Shader<ID3D10VertexShader>::CreateFn Shader<ID3D10VertexShader>::_create = &ID3D10Device::CreateVertexShader;
template<> const Shader<ID3D10PixelShader>::CreateFn Shader<ID3D10PixelShader>::_create = &ID3D10Device::CreatePixelShader;



// lop-off the filename, leave the trailing slash
static std::wstring GetPath(const std::wstring& p)
{
    size_t slash = p.find_last_of('\\');
    return p.substr(0, slash + 1);
}


class Scene
{
    struct Vertex
    {
        float x, y, z;
    };

    float _bgcolor[4];
    std::wstring _csoPath;

    unsigned _frame;
    bool _firstFrame;
    D3D10_VIEWPORT _viewport;
    CComPtr<ID3D10Buffer> _vertexBuffer;
    Shader<ID3D10VertexShader> _vertexShader;
    Shader<ID3D10PixelShader> _pixelShader;
    CComPtr<ID3D10InputLayout> _inputLayout;

public:
    Scene(uint32_t abgrBackground, const std::wstring& pluginPath)
        : _frame(0)
        , _firstFrame(true)
        , _csoPath(GetPath(pluginPath))
    {
        memset(&_viewport, 0, sizeof(_viewport));

        uint32_t r = abgrBackground & 0xFF;
        uint32_t g = (abgrBackground & 0xFF00) >> 8;
        uint32_t b = (abgrBackground & 0xFF0000) >> 16;
        uint32_t a = (abgrBackground & 0xFF000000) >> 24;

        _bgcolor[0] = b * a / 65025.f;
        _bgcolor[1] = g * a / 65025.f;
        _bgcolor[2] = r * a / 65025.f;
        _bgcolor[3] = a / 255.f;
    }

    bool render(ID3D10Device1* device, ID3D10RenderTargetView* rtView, uint32_t width, uint32_t height)
    {
        if (_firstFrame) {

            _firstFrame = false;

            Vertex vertices[] =
            {
                { 0.0f, 0.5f, 0.0f },
                { 0.45f, -0.5f, 0.0f },
                { -0.45f, -0.5f, 0.0f },
            };

            assert(sizeof(Vertex) == sizeof(float) * 3);
            assert(ARRAYSIZE(vertices) == 3);

            D3D10_BUFFER_DESC bd;
            bd.Usage = D3D10_USAGE_DEFAULT;
            bd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
            bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = 0;
            bd.MiscFlags = 0;

            D3D10_SUBRESOURCE_DATA srd = { vertices, 0, 0 };

            HRESULT hr = device->CreateBuffer(&bd, &srd, &_vertexBuffer);
            bool loadedOk = 
                _vertexShader.Load(_csoPath + L"VertexShader.cso", device) &&
                _pixelShader.Load(_csoPath + L"PixelShader.cso", device);

            if (loadedOk) {
                device->VSSetShader(_vertexShader.ptr);
                device->PSSetShader(_pixelShader.ptr);

                // create the input layout
                D3D10_INPUT_ELEMENT_DESC ied[] =
                {
                    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
                };
                device->CreateInputLayout(ied, ARRAYSIZE(ied), _vertexShader.bytes.get(), _vertexShader.size, &_inputLayout);
                device->IASetInputLayout(_inputLayout);
            }
        }

        // draw

        _viewport.Width = width;
        _viewport.Height = height;
        device->RSSetViewports(1, &_viewport);
    
        _bgcolor[2] = (_frame % 100)/99.f;
        device->ClearRenderTargetView(rtView, _bgcolor);

        // set our new render target object as the active render target
        device->OMSetRenderTargets(1, &rtView, 0);

        // set the vertex buffer
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        device->IASetVertexBuffers(0, 1, &_vertexBuffer.p, &stride, &offset);

        // set the primitive topology
        device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // draw 3 vertices, starting from vertex 0
        device->Draw(3, 0);

        _frame++;
        return true;
    }
};

