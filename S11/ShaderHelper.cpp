#include "pch.h"
#include "ShaderHelper.h"

#pragma warning(disable: 6031)

using namespace std;
using namespace Microsoft::WRL;

const vector<std::byte> ShaderHelper::LoadCompiledShaderBlob(const wstring& wszShaderFileName)
{
    // relative path
    wstring wszBuf(1024, 0);
    GetModuleFileName(0, wszBuf.data(), static_cast<DWORD>(wszBuf.size()));
    auto pos = wszBuf.find_last_of(L'\\');
    wszBuf.replace(wszBuf.begin() + pos + 1, wszBuf.end(), wszShaderFileName);

    // read
    vector<std::byte> vtBlob;
    HANDLE hFile = CreateFile(wszBuf.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, 0);
    auto size = GetFileSize(hFile, nullptr);
    if (size != INVALID_FILE_SIZE) {
        vtBlob.resize(size);
        ReadFile(hFile, vtBlob.data(), size, &size, nullptr);
    }
    CloseHandle(hFile);

    return vtBlob;
}

const vector<std::byte> ShaderHelper::LoadDDSTextureFromFile(const std::wstring& wszDDSFileName, DDS_HEADER **ppHeader)
{
    // relative path
    wstring wszBuf(1024, 0);
    GetModuleFileName(0, wszBuf.data(), static_cast<DWORD>(wszBuf.size()));
    auto pos = wszBuf.find_last_of(L'\\');
    wszBuf.replace(wszBuf.begin() + pos + 1, wszBuf.end(), wszDDSFileName);

    // read
    vector<std::byte> vtBlob;
    HANDLE hFile = CreateFile(wszBuf.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, 0);
    auto size = GetFileSize(hFile, nullptr);
    if (size != INVALID_FILE_SIZE) {
        vtBlob.resize(size);
        ReadFile(hFile, vtBlob.data(), size, &size, nullptr);
    }
    CloseHandle(hFile);

    if (memcmp(vtBlob.data(), "DDS\x20", 4) != 0) {
		throw exception("Not a DDS file");
	}

    *ppHeader = reinterpret_cast<DDS_HEADER*>(vtBlob.data() + 4);
    if ((*ppHeader)->dwSize != sizeof(DDS_HEADER)) {
        *ppHeader = nullptr;
        throw exception("Invalid DDS header");
    }

    return vtBlob;
}

const vector<std::byte> ShaderHelper::LoadImageTextureFromFile(const std::wstring& wszImageFileName, UINT *puWidth, UINT *puHeight)
{
    // relative path
    wstring wszBuf(1024, 0);
    GetModuleFileName(0, wszBuf.data(), static_cast<DWORD>(wszBuf.size()));
    auto pos = wszBuf.find_last_of(L'\\');
    wszBuf.replace(wszBuf.begin() + pos + 1, wszBuf.end(), wszImageFileName);

    ComPtr<IWICImagingFactory2> spWICFactory2;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&spWICFactory2));
    if(FAILED(hr)) throw exception("Failed to create WICImagingFactory2");

    ComPtr<IWICBitmapDecoder> spDecoder;
    hr = spWICFactory2->CreateDecoderFromFilename(wszBuf.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &spDecoder);
    if(FAILED(hr)) throw exception("Failed to create IWICBitmapDecoder");

    ComPtr<IWICBitmapFrameDecode> spFrame;
    hr = spDecoder->GetFrame(0, &spFrame);
    if(FAILED(hr)) throw exception("Failed to get IWICBitmapFrameDecode");

    ComPtr<IWICFormatConverter> spConverter;
    hr = spWICFactory2->CreateFormatConverter(&spConverter);
    if(FAILED(hr)) throw exception("Failed to create IWICFormatConverter");

    hr = spConverter->Initialize(spFrame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if(FAILED(hr)) throw exception("Failed to initialize IWICFormatConverter");

    hr = spConverter->GetSize(puWidth, puHeight);
    UINT uStride = ((*puWidth + 3) / 4) * 4 * 4;
    UINT uSize = uStride * (*puHeight);
    vector<std::byte> vtBlob(uSize);
    vtBlob.resize(uSize);
    hr = spConverter->CopyPixels(nullptr, uStride, uSize, (BYTE*)vtBlob.data());
    if(FAILED(hr)) throw exception("Failed to copy pixels");

    return vtBlob;
}

DXGI_FORMAT ShaderHelper::GetDXGIFormat(const DDS_PIXELFORMAT& ddspf)
{
    // https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds-pguide#using-texture-arrays-in-direct3d-1011
    DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
    char buf[1024] = "Not supported DDS format.";

    if (ddspf.dwFlags & DDS_PIXELFORMAT_FLAGS::DDPF_FOURCC) {
        char fourCC[5] = { 0 };
        memcpy(fourCC, &ddspf.dwFourCC, 4);
        if (strcmp(fourCC, "DXT1") == 0) {
            fmt = DXGI_FORMAT_BC1_UNORM;
        }
        else if (strcmp(fourCC, "DXT5") == 0) {
			fmt = DXGI_FORMAT_BC3_UNORM;
		}
        else {
            sprintf_s(buf, "Not supported DDS format: %s", fourCC);
        }
    }
    else if (ddspf.dwFlags & (DDS_PIXELFORMAT_FLAGS::DDPF_RGB | DDS_PIXELFORMAT_FLAGS::DDPF_ALPHAPIXELS)) {
        if (ddspf.dwRGBBitCount == 32) {
            if (ddspf.dwBBitMask == 0x000000FF
             && ddspf.dwGBitMask == 0x0000FF00
             && ddspf.dwRBitMask == 0x00FF0000
             && ddspf.dwABitMask == 0xFF000000) {
                fmt = DXGI_FORMAT_B8G8R8A8_UNORM;
            }
        }
    }

    if (fmt == DXGI_FORMAT_UNKNOWN) throw exception(buf);

    return fmt;
}

UINT ShaderHelper::GetBlockSize(DXGI_FORMAT fmt)
{
    constexpr UINT nPixels = 4;
    UINT uBlockSize = 4 * nPixels;
    switch (fmt) {
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            uBlockSize = 2 * nPixels;
			break;
    }

    return uBlockSize;
}