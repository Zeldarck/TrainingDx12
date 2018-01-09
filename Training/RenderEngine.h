#pragma once
#include "d3dx12.h"

class RenderEngine
{
private:

    RenderEngine() = default;
    ~RenderEngine() = default;
    static RenderEngine* INSTANCE;
    
    
    ID3D12Device* m_device; // direct3d device
    ID3D12CommandQueue* m_commandQueue; // container for command lists



    bool CreateDevice();
    bool CreateCommandQueue();

    bool CreateSwapChain();

public:
    static RenderEngine * GetInstance();

    static void DeleteInstance();



    bool InitD3D();

};

