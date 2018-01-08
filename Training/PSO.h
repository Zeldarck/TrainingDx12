#pragma once
#include "d3dx12.h"

typedef
enum PSO_FLAGS
{
    PSO_FLAG_NONE = 0,
    PSO_FLAG_TEXTURE = 0x1,
    PSO_FLAGS_FULLCOLOR = 0x2,
} 	PSO_FLAGS;

DEFINE_ENUM_FLAG_OPERATORS(PSO_FLAGS);



class PSO
{

protected:
    D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
    ID3D12PipelineState* m_pipelineStateObject = nullptr; // pso containing a pipeline state

public:

    ID3D12PipelineState* GetPipelineStateObject();

    PSO(ID3D12Device * a_device, D3D12_INPUT_LAYOUT_DESC a_inputLayoutDesc, D3D12_SHADER_BYTECODE a_pixelShaderBytecode, D3D12_SHADER_BYTECODE a_vertexShaderBytecode, ID3D12RootSignature * a_rootSignature, DXGI_SAMPLE_DESC a_sampleDesc);
    ~PSO();
};

