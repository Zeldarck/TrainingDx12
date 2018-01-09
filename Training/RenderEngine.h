#pragma once
#include "d3dx12.h"
#include "Window.h"
#include <string>
#define FRAME_BUFFER_COUNT 3

class RenderEngine
{
private:

    RenderEngine() = default;
    ~RenderEngine();
    static RenderEngine* INSTANCE;
    
    
    ID3D12Device* m_device = nullptr; // direct3d device
    ID3D12CommandQueue* m_commandQueue = nullptr; // container for command lists
    Window* m_window = nullptr;
    IDXGISwapChain3* m_swapChain = nullptr; // swapchain used to switch between render targets
    int m_frameIndex = 0;
    ID3D12CommandAllocator* m_commandAllocator[FRAME_BUFFER_COUNT]; // we want enough allocators for each buffer * number of threads (we only have one thread)
    ID3D12GraphicsCommandList* m_commandList = nullptr; // a command list we can record commands into, then execute them to render the frame
    ID3D12Fence* m_fence[FRAME_BUFFER_COUNT];    // an object that is locked while our command list is being executed by the gpu. We need as many 
    HANDLE m_fenceEvent; // a handle to an event when our fence is unlocked by the gpu
    UINT64 m_fenceValue[FRAME_BUFFER_COUNT]; // this value is incremented each frame. each fence will have its own value
    DXGI_SAMPLE_DESC* m_sampleDesc = nullptr;
    IDXGIFactory4* m_dxgiFactory = nullptr;
    ID3D12Resource* m_depthStencilBuffer = nullptr; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
    ID3D12DescriptorHeap* m_dsDescriptorHeap = nullptr; // This is a heap for our depth/stencil buffer descriptor
    ID3D12DescriptorHeap* m_rtvDescriptorHeap = nullptr; // a descriptor heap to hold resources like the render targets
    ID3D12Resource* m_renderTargets[FRAME_BUFFER_COUNT]; // number of render targets equal to buffer count
    int m_rtvDescriptorSize; // size of the rtv descriptor on the device (all front and back buffers will be the same size)
                           // function declarations


    bool CreateDevice();
    bool CreateCommandQueue();

    bool CreateSwapChain();
    bool CreateRenderTargetViews();
    bool CreateCommandAllocators();
    bool CreateCommandList();
    bool CreateFences();
    void InitDXGIFactory();
    bool CreateDepthStencil();


public:
    static RenderEngine * GetInstance(Window * a_window);
    static RenderEngine * GetInstance();

    static void DeleteInstance();

    bool WaitForPreviousFrame();
    DXGI_SAMPLE_DESC * GetSampleDesc();



    bool InitD3D();

    void ErrorMessage(std::string a_msg);

    void Render();

};

