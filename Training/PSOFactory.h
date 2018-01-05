#pragma once
#include <unordered_map>
#include "d3dx12.h"
#include "PSO.h"




class PSOFactory
{
private:
    PSOFactory();
    static PSOFactory* INSTANCE;

public:
    ID3D12Device * m_device;
    D3D12_INPUT_LAYOUT_DESC m_inputLayoutDesc;
    D3D12_SHADER_BYTECODE m_pixelShaderBytecode;
    D3D12_SHADER_BYTECODE m_vertexShaderBytecode;
    ID3D12RootSignature* m_rootSignature;
    DXGI_SAMPLE_DESC m_sampleDesc;




   // static PSOFactory * GetInstance();

    std::unordered_map<PSO_FLAGS, PSO*> m_psoMap;


    static PSOFactory * GetInstance(ID3D12Device * a_device, D3D12_INPUT_LAYOUT_DESC a_inputLayoutDesc, D3D12_SHADER_BYTECODE a_pixelShaderBytecode, D3D12_SHADER_BYTECODE a_vertexShaderBytecode, ID3D12RootSignature * a_rootSignature, DXGI_SAMPLE_DESC a_sampleDesc);

    PSO* CreatePSO(PSO_FLAGS a_flag);

    ~PSOFactory();
};

