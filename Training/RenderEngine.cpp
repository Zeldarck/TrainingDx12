#include "RenderEngine.h"
#include <dxgi1_4.h>
#include "Util.h"

RenderEngine* RenderEngine::INSTANCE = nullptr;


RenderEngine * RenderEngine::GetInstance(Window * a_window)
{
    RenderEngine * output = GetInstance();
    output->m_window = a_window;
    return output;
}

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

void RenderEngine::InitDXGIFactory() {
    HRESULT hr;

    if (!m_dxgiFactory) {
        hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
        if (FAILED(hr))
        {
            return;
        }
    }
}

DXGI_SAMPLE_DESC* RenderEngine::GetSampleDesc() {
    return m_sampleDesc;
}

RenderEngine::~RenderEngine()
{

    // get swapchain out of full screen before exiting
    BOOL fs = false;
    if (m_swapChain->GetFullscreenState(&fs, NULL))
            m_swapChain->SetFullscreenState(false, NULL);




    SAFE_RELEASE(m_device);
    SAFE_RELEASE(m_swapChain);
    SAFE_RELEASE(m_commandQueue);
    SAFE_RELEASE(m_rtvDescriptorHeap);
    SAFE_RELEASE(m_commandList);


    // wait for the gpu to finish all frames
    for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
    {
        m_frameIndex = i;
        WaitForPreviousFrame();
    }

    for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
    {
        SAFE_RELEASE(m_renderTargets[i]);
        SAFE_RELEASE(m_commandAllocator[i]);
        SAFE_RELEASE(m_fence[i]);
    };


}


bool RenderEngine::WaitForPreviousFrame(){
    HRESULT hr;

    // swap the current rtv buffer index so we draw on the correct buffer
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
    // the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
    if (m_fence[m_frameIndex]->GetCompletedValue() < m_fenceValue[m_frameIndex])
    {
        // we have the fence create an event which is signaled once the fence's current value is "fenceValue"
        hr = m_fence[m_frameIndex]->SetEventOnCompletion(m_fenceValue[m_frameIndex], m_fenceEvent);
        if (FAILED(hr))
        {
            return false;
        }

        // We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
        // has reached "fenceValue", we know the command queue has finished executing
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    // increment fenceValue for next frame
    m_fenceValue[m_frameIndex]++;

    return true;
}


bool RenderEngine::CreateDevice() {

    HRESULT hr;

    InitDXGIFactory();

    IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)

    int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0

    bool adapterFound = false; // set this to true when a good one was found

                               // find first hardware gpu that supports d3d 12
    while (m_dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
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

    InitDXGIFactory();

    DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
    backBufferDesc.Width = m_window->GetWidth(); // buffer width
    backBufferDesc.Height = m_window->GetWidth(); // buffer height
    backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

                                                        // describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
    *m_sampleDesc = {};
    m_sampleDesc->Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

                          // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = FRAME_BUFFER_COUNT; // number of buffers we have
    swapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
    swapChainDesc.OutputWindow = m_window->GetHWND(); // handle to our window
    swapChainDesc.SampleDesc = *m_sampleDesc; // our multi-sampling description
    swapChainDesc.Windowed = !m_window->IsFullScreen(); // set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps

    IDXGISwapChain* tempSwapChain;

    m_dxgiFactory->CreateSwapChain(
        m_commandQueue, // the queue will be flushed once the swap chain is created
        &swapChainDesc, // give it the swap chain description we created above
        &tempSwapChain // store the created swap chain in a temp IDXGISwapChain interface
    );

    m_swapChain = static_cast<IDXGISwapChain3*>(tempSwapChain);

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    return true;
}


bool RenderEngine::CreateRenderTargetViews() {

    HRESULT hr;


    // describe an rtv descriptor heap and create
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FRAME_BUFFER_COUNT; // number of descriptors for this heap.
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // this heap is a render target view heap

                                                       // This heap will not be directly referenced by the shaders (not shader visible), as this will store the output from the pipeline
                                                       // otherwise we would set the heap's flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvDescriptorHeap));
    if (FAILED(hr))
    {
        return false;
    }

    // get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
    // descriptor sizes may vary from device to device, which is why there is no set size and we must ask the 
    // device to give us the size. we will use this size to increment a descriptor handle offset
    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // get a handle to the first descriptor in the descriptor heap. a handle is basically a pointer,
    // but we cannot literally use it like a c++ pointer.
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    // Create a RTV for each buffer (double buffering is two buffers, tripple buffering is 3).
    for (int i = 0; i < FRAME_BUFFER_COUNT; i++)
    {
        // first we get the n'th buffer in the swap chain and store it in the n'th
        // position of our ID3D12Resource array
        hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
        if (FAILED(hr))
        {
            return false;
        }

        // the we "create" a render target view which binds the swap chain buffer (ID3D12Resource[n]) to the rtv handle
        m_device->CreateRenderTargetView(m_renderTargets[i], nullptr, rtvHandle);

        // we increment the rtv handle by the rtv descriptor size we got above
        rtvHandle.Offset(1, m_rtvDescriptorSize);
    }
}

bool RenderEngine::CreateCommandAllocators() {


    HRESULT hr;

    for (int i = 0; i < FRAME_BUFFER_COUNT; i++)
    {
        hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator[i]));
        if (FAILED(hr))
        {
            return false;
        }
    }

    return true;
}

bool RenderEngine::CreateCommandList() {
    // create the command list with the first allocator

    if (!CreateCommandAllocators()) {
        return false;
    }


    HRESULT hr;

    hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[m_frameIndex], NULL, IID_PPV_ARGS(&m_commandList));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool RenderEngine::CreateFences() {
    HRESULT hr;

    // create the fences
    for (int i = 0; i < FRAME_BUFFER_COUNT; i++)
    {
        hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence[i]));
        if (FAILED(hr))
        {
            return false;
        }
        m_fenceValue[i] = 0; // set the initial fence value to 0
    }

    // create a handle to a fence event
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
    {
        return false;
    }

    return true;
}


bool RenderEngine::CreateDepthStencil() {
    HRESULT hr;


    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    hr = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsDescriptorHeap));
    if (FAILED(hr))
    {
        return false;
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_window->GetWidth(), m_window->GetHeight(), 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthOptimizedClearValue,
        IID_PPV_ARGS(&m_depthStencilBuffer)
    );
    m_dsDescriptorHeap->SetName(L"Depth/Stencil Resource Heap");

    m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilDesc, m_dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    return true;
}

bool RenderEngine::InitD3D()
{
    HRESULT hr;
    if (!m_window) {
        return false;
    }

    if (!CreateDevice()) {
        return false;
    }

    if (!CreateCommandQueue()) {
        return false;
    }

    if (!CreateSwapChain()) {
        return false;
    }

    if (!CreateRenderTargetViews()) {
        return false;
    }

    if (!CreateCommandList()) {
        return false;
    }

    if (!CreateFences()) {
        return false;
    }

    if (!CreateDepthStencil()) {
        return false;
    }


}

void RenderEngine::ErrorMessage(std::string a_msg) {
    MessageBox(0, Util::StringToWString(a_msg).c_str(),
        L"Error", MB_OK);
}


void RenderEngine::Render()
{
    HRESULT hr;

    UpdatePipeline(); // update the pipeline by sending commands to the commandqueue

                      // create an array of command lists (only one command list here)
    ID3D12CommandList* ppCommandLists[] = { m_commandList };

    // execute the array of command lists
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // this command goes in at the end of our command queue. we will know when our command queue 
    // has finished because the fence value will be set to "fenceValue" from the GPU since the command
    // queue is being executed on the GPU
    hr = m_commandQueue->Signal(m_fence[m_frameIndex], m_fenceValue[m_frameIndex]);
    if (FAILED(hr))
    {
        return;
    }

    // present the current backbuffer
    hr = m_swapChain->Present(1, 0);
    if (FAILED(hr))
    {
        return;
    }
}
