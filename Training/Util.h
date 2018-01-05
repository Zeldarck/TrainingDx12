#pragma once
#include <wincodec.h>
#include "d3dx12.h"
#include <string>


class Util
{
private :
    Util();

public:
    static std::wstring StringToWString(const std::string & s);
    static DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID & wicFormatGUID);
    static WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID & wicFormatGUID);
    static int GetDXGIFormatBitsPerPixel(DXGI_FORMAT & dxgiFormat);
    static int LoadImageDataFromFile(BYTE ** imageData, D3D12_RESOURCE_DESC & resourceDescription, LPCWSTR filename, int & bytesPerRow);
    ~Util();
};

