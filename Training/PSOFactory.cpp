#include "PSOFactory.h"
#include "ShaderFactory.h"

PSOFactory* PSOFactory::INSTANCE = nullptr;



PSOFactory* PSOFactory::GetInstance(ID3D12Device * a_device, DXGI_SAMPLE_DESC a_sampleDesc) {
    if (!INSTANCE) {
        INSTANCE = new PSOFactory();
        INSTANCE->m_device = a_device;
        INSTANCE->m_sampleDesc = a_sampleDesc;


    }

    return INSTANCE;
}

PSOFactory* PSOFactory::GetInstance(ID3D12Device * a_device) {
    if (!INSTANCE) {
        INSTANCE = new PSOFactory();
        INSTANCE->m_device = a_device;
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
        output = new PSO(m_device, MakeInputLayoutDesc(a_flag), MakePixelShader(a_flag), MakeVertexShader(a_flag), MakeRootSignature(a_flag), m_sampleDesc);
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

D3D12_INPUT_LAYOUT_DESC * PSOFactory::MakeInputLayoutDesc(PSO_FLAGS a_flag)
{
    D3D12_INPUT_ELEMENT_DESC * inputLayout = nullptr;
    D3D12_INPUT_LAYOUT_DESC * inputLayoutDesc = new D3D12_INPUT_LAYOUT_DESC();;

    if (a_flag & PSO_FLAG_TEXTURE) {
        inputLayout = new D3D12_INPUT_ELEMENT_DESC[3];
            
        inputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
        inputLayout[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
        inputLayout[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT  , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
        inputLayoutDesc->NumElements = 3;
    }
    else if (a_flag & PSO_FLAG_FULLCOLOR) {
        inputLayout = new D3D12_INPUT_ELEMENT_DESC[2];
        inputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
        inputLayout[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
        inputLayoutDesc->NumElements = 2;

    }
    inputLayoutDesc->pInputElementDescs = inputLayout;

    return inputLayoutDesc;

}