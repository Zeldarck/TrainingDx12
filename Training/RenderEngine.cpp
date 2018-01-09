#include "RenderEngine.h"
#include <dxgi1_4.h>

#define FRAME_BUFFER_COUNT 3

RenderEngine* RenderEngine::INSTANCE = nullptr;


RenderEngine * RenderEngine::GetInstance()
{
    if (!INSTANCE) {
        INSTANCE = new RenderEngine();
    }
    return INSTANCE;
}

void RenderEngine::DeleteInstance()
{
    if (INSTANCE)
        delete INSTANCE;
}


bool RenderEngine::CreateDevice() {

    HRESULT hr;

    
    IDXGIFactory4* dxgiFactory;
    hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr))
    {
        return false;
    }


    IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)

    int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0

    bool adapterFound = false; // set this to true when a good one was found

                               // find first hardware gpu that supports d3d 12
    while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            // we dont want a software device
            continue;
        }

        // we want a device that is compatible with direct3d 12 (feature level 11 or higher)
        hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
        if (SUCCEEDED(hr))
        {
            adapterFound = true;
            break;
        }

        adapterIndex++;
    }

    if (!adapterFound)
    {
        return false;
    }

    // Create the device
    hr = D3D12CreateDevice(
        adapter,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&m_device)
    );
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}


bool RenderEngine::CreateCommandQueue() {
    HRESULT hr;

    D3D12_COMMAND_QUEUE_DESC cqDesc = {};
    cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // direct means the gpu can directly execute this command queue

    hr = m_device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_commandQueue)); // create the command queue
    if (FAILED(hr))
    {
        return false;
    }

}

bool RenderEngine::CreateSwapChain() {

  /*  DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
    /////TODO ASK TO WINDOW CLASS
    backBufferDesc.Width = 800; // buffer width
    backBufferDesc.Height = 600; // buffer height
    backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

                                                        // describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
    DXGI_SAMPLE_DESC sampleDesc = {};
    sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

                          // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = FRAME_BUFFER_COUNT; // number of buffers we have
    swapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
    swapChainDesc.OutputWindow = hwnd; // handle to our window
    swapChainDesc.SampleDesc = sampleDesc; // our multi-sampling description
    swapChainDesc.Windowed = !FullScreen; // set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps

    IDXGISwapChain* tempSwapChain;

    dxgiFactory->CreateSwapChain(
        commandQueue, // the queue will be flushed once the swap chain is created
        &swapChainDesc, // give it the swap chain description we created above
        &tempSwapChain // store the created swap chain in a temp IDXGISwapChain interface
    );

    swapChain = static_cast<IDXGISwapChain3*>(tempSwapChain);

    frameIndex = swapChain->GetCurrentBackBufferIndex();*/
    return true;
}

bool RenderEngine::InitD3D()
{
    HRESULT hr;


    if (!CreateDevice()) {
        return false;
    }

    if (!CreateCommandQueue()) {
        return false;
    }



}
