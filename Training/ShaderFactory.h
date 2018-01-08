#pragma once
#include <unordered_map>
#include "d3dx12.h"
#include "Shader.h"
#include <string>

class ShaderFactory
{
private:
    ShaderFactory();
    static ShaderFactory* INSTANCE;

    std::unordered_map<std::string, Shader*> m_shaderMap;
    ~ShaderFactory();

public:
     static ShaderFactory* GetInstance();
     Shader* CreateShader(std::string a_path, SHADER_TYPE a_type);

     static void DeleteInstance();

};

