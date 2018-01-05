#include "PSO.h"



ID3D12PipelineState * PSO::GetPipelineStateObject()
{
    return m_pipelineStateObject;
}

PSO::PSO(ID3D12Device * a_device, D3D12_INPUT_LAYOUT_DESC a_inputLayoutDesc, D3D12_SHADER_BYTECODE a_pixelShaderBytecode, D3D12_SHADER_BYTECODE a_vertexShaderBytecode, ID3D12RootSignature* a_rootSignature, DXGI_SAMPLE_DESC a_sampleDesc)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso
    psoDesc.InputLayout = a_inputLayoutDesc; // the structure describing our input layout
    psoDesc.pRootSignature = a_rootSignature; // the root signature that describes the input data this pso needs
    psoDesc.VS = a_vertexShaderBytecode; // structure describing where to find the vertex shader bytecode and how large it is
    psoDesc.PS = a_pixelShaderBytecode; // same as VS but for pixel shader
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
    psoDesc.SampleDesc = a_sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
    psoDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
    psoDesc.NumRenderTargets = 1; // we are only binding one render target
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state

                                                                           // create the pso
   HRESULT hr = a_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineStateObject));
    if (FAILED(hr))
    {
        return;
    }    
}


PSO::~PSO()
{
}
