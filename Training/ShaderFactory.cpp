#include "ShaderFactory.h"


ShaderFactory* ShaderFactory::INSTANCE = nullptr;

ShaderFactory::ShaderFactory()
{
}


ShaderFactory * ShaderFactory::GetInstance()
{
   if (!INSTANCE) {
        INSTANCE = new ShaderFactory();
    }

    return INSTANCE;
}

Shader * ShaderFactory::CreateShader(std::string a_path, SHADER_TYPE a_type)
{
    Shader* output = m_shaderMap[a_path];
    if (!output) {
        output = new Shader(a_path,a_type);
        m_shaderMap[a_path] = output;
    }
    return output;
}

//call depuis exit de main
void ShaderFactory::DeleteInstance()
{
    delete INSTANCE;
}


ShaderFactory::~ShaderFactory()
{
}
