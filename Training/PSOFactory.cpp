#include "PSOFactory.h"
#include "ShaderFactory.h"

PSOFactory* PSOFactory::INSTANCE = nullptr;



PSOFactory* PSOFactory::GetInstance(ID3D12Device * a_device, D3D12_INPUT_LAYOUT_DESC a_inputLayoutDesc, DXGI_SAMPLE_DESC a_sampleDesc) {
    if (!INSTANCE) {
        INSTANCE = new PSOFactory();
        INSTANCE->m_device = a_device;
        INSTANCE->m_inputLayoutDesc = a_inputLayoutDesc;
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
        output = new PSO(m_device, m_inputLayoutDesc, MakePixelShader(a_flag), MakeVertexShader(a_flag), MakeRootSignature(a_flag), m_sampleDesc);
        m_psoMap[a_flag] =  output;
    }
    return output;
}

ID3D12RootSignature * PSOFactory::MakeRootSignature(PSO_FLAGS a_flag)
{
    ID3D12RootSignature* output = nullptr;

    if (a_flag & PSO_FLAG_TEXTURE) {
        output = RootSignatureFactory::GetInstance(m_device)->CreateRootSignature(ROOT_SIGNATURE_FLAG_TEXTURE)->GetRootSignature();
    }
    else if(a_flag & PSO_FLAG_FULLCOLOR) {
        output = RootSignatureFactory::GetInstance(m_device)->CreateRootSignature(ROOT_SIGNATURE_FLAG_FULLCOLOR)->GetRootSignature();

    }
    return output;
}

D3D12_SHADER_BYTECODE * PSOFactory::MakePixelShader(PSO_FLAGS a_flag)
{
    D3D12_SHADER_BYTECODE* output = nullptr;

    if (a_flag & PSO_FLAG_TEXTURE) {
        output = ShaderFactory::GetInstance()->CreateShader("TexturePixelShader.hlsl", SHADER_TYPE_PIXEL)->GetShaderByteCode();
    }
    else if (a_flag & PSO_FLAG_FULLCOLOR) {
        output = ShaderFactory::GetInstance()->CreateShader("PixelShader.hlsl", SHADER_TYPE_PIXEL)->GetShaderByteCode();

    }

    return output;
}

D3D12_SHADER_BYTECODE * PSOFactory::MakeVertexShader(PSO_FLAGS a_flag)
{
    D3D12_SHADER_BYTECODE* output = nullptr;

    if (a_flag & PSO_FLAG_TEXTURE) {
        output = ShaderFactory::GetInstance()->CreateShader("TextureVertexShader.hlsl", SHADER_TYPE_VERTEX)->GetShaderByteCode();
    }
    else if (a_flag & PSO_FLAG_FULLCOLOR) {
        output = ShaderFactory::GetInstance()->CreateShader("VertexShader.hlsl", SHADER_TYPE_VERTEX)->GetShaderByteCode();

    }

    return output;
}