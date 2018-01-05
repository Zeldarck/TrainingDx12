#pragma once
#include <string>
#include "d3dx12.h"


typedef
enum SHADER_TYPE
{
    SHADER_TYPE_COMPUTE = 0,
    SHADER_TYPE_DOMAIN,
    SHADER_TYPE_GEOMETRY,
    SHADER_TYPE_HULL,
    SHADER_TYPE_PIXEL,
    SHADER_TYPE_VERTEX
} 	SHADER_TYPE;


class Shader
{
protected:
    ID3DBlob* m_shaderCompiled;
    D3D12_SHADER_BYTECODE m_shaderBytecode;
    SHADER_TYPE m_type;

public:
    D3D12_SHADER_BYTECODE* GetShaderByteCode();
    Shader(std::string a_path, SHADER_TYPE a_type);
    ~Shader();
};

