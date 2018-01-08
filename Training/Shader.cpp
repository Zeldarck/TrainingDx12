#include "Shader.h"
#include <D3Dcompiler.h>
#include "Util.h"



D3D12_SHADER_BYTECODE * Shader::GetShaderByteCode()
{
    return &m_shaderBytecode;
}

Shader::Shader(std::string a_path, SHADER_TYPE a_type)
{
    m_type = a_type;
    LPCSTR target = "";

    switch (a_type) {
    case SHADER_TYPE_COMPUTE:
        target = "cs_5_0";
        break;
    case SHADER_TYPE_DOMAIN:
        target = "ds_5_0";
        break;
    case SHADER_TYPE_GEOMETRY:
        target = "gs_5_0";
        break;
    case SHADER_TYPE_HULL:
        target = "hs_5_0";
        break;
    case SHADER_TYPE_PIXEL:
        target = "ps_5_0";
        break;
    default:
        target = "vs_5_0";
        break;
    }
    ID3DBlob* errorBuff;
    HRESULT hr = D3DCompileFromFile(Util::StringToWString(a_path).c_str(),
        nullptr,
        nullptr,
        "main",
        target,
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &m_shaderCompiled,
        &errorBuff);
    if (FAILED(hr))
    {
        OutputDebugStringA((char*)errorBuff->GetBufferPointer());
        return;
    }

    m_shaderBytecode = {};
    m_shaderBytecode.BytecodeLength = m_shaderCompiled->GetBufferSize();
    m_shaderBytecode.pShaderBytecode = m_shaderCompiled->GetBufferPointer();
}

Shader::~Shader()
{
    SAFE_RELEASE(m_shaderCompiled);
}
