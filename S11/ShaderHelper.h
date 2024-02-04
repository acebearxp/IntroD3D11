#pragma once

enum DDS_PIXELFORMAT_FLAGS {
	DDPF_ALPHAPIXELS = 0x1,
	DDPF_ALPHA = 0x2,
	DDPF_FOURCC = 0x4,
	DDPF_RGB = 0x40,
	DDPF_YUV = 0x200,
	DDPF_LUMINANCE = 0x20000
};

struct DDS_PIXELFORMAT {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	DWORD dwRGBBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwABitMask;
};

typedef struct {
	DWORD           dwSize;
	DWORD           dwFlags;
	DWORD           dwHeight;
	DWORD           dwWidth;
	DWORD           dwPitchOrLinearSize;
	DWORD           dwDepth;
	DWORD           dwMipMapCount;
	DWORD           dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	DWORD           dwCaps;
	DWORD           dwCaps2;
	DWORD           dwCaps3;
	DWORD           dwCaps4;
	DWORD           dwReserved2;
} DDS_HEADER;

class ShaderHelper
{
public:
	static const std::vector<std::byte> LoadCompiledShaderBlob(const std::wstring& wszShaderFileName);
	static const std::vector<std::byte> LoadDDSTextureFromFile(const std::wstring& wszDDSFileName, DDS_HEADER **ppHeader);
	static const std::vector<std::byte> LoadImageTextureFromFile(const std::wstring& wszImageFileName, UINT *puWidth, UINT *puHeight);
	static DXGI_FORMAT GetDXGIFormat(const DDS_PIXELFORMAT& ddpf);
	static UINT GetBlockSize(DXGI_FORMAT fmt);
};
