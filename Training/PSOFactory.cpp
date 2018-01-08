#include "PSOFactory.h"

PSOFactory* PSOFactory::INSTANCE = nullptr;



PSOFactory* PSOFactory::GetInstance(ID3D12Device * a_device, D3D12_INPUT_LAYOUT_DESC a_inputLayoutDesc, D3D12_SHADER_BYTECODE a_pixelShaderBytecode, D3D12_SHADER_BYTECODE a_vertexShaderBytecode, DXGI_SAMPLE_DESC a_sampleDesc) {
    if (!INSTANCE) {
        INSTANCE = new PSOFactory();
        INSTANCE->m_device = a_device;
        INSTANCE->m_inputLayoutDesc = a_inputLayoutDesc;
        INSTANCE->m_pixelShaderBytecode = a_pixelShaderBytecode;
        INSTANCE->m_vertexShaderBytecode = a_vertexShaderBytecode;
        INSTANCE->m_sampleDesc = a_sampleDesc;


    }

    return INSTANCE;
}

//call depuis exit de main
void PSOFactory::DeleteInstance()
{
    if (INSTANCE)
        delete INSTANCE;
}

void PSOFactory::ResetInstance()
{
    DeleteInstance();
    //GetInstance();
}

PSO * PSOFactory::CreatePSO(PSO_FLAGS a_flag)
{
    PSO* output = m_psoMap[a_flag];
    if (!output) {
        RootSignature* rs = MakeRootSignature(a_flag);
        output = new PSO(m_device, m_inputLayoutDesc, m_pixelShaderBytecode, m_vertexShaderBytecode, rs->GetRootSignature(), m_sampleDesc);
        m_psoMap[a_flag] =  output;
    }
    return output;
}

RootSignature * PSOFactory::MakeRootSignature(PSO_FLAGS a_flag)
{
    RootSignature* output = nullptr;

    if (a_flag & PSO_FLAG_TEXTURE) {
        output = RootSignatureFactory::GetInstance(m_device)->CreateRootSignature(ROOT_SIGNATURE_FLAG_TEXTURE);
    }
    else if(a_flag & PSO_FLAGS_FULLCOLOR) {
        output = RootSignatureFactory::GetInstance(m_device)->CreateRootSignature(ROOT_SIGNATURE_FLAG_FULLCOLOR);

    }
    return output;
}
