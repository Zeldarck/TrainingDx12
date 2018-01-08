#include "RootSignatureFactory.h"

RootSignatureFactory* RootSignatureFactory::INSTANCE = nullptr;


RootSignatureFactory * RootSignatureFactory::GetInstance(ID3D12Device * a_device)
{
    if (!INSTANCE) {
        INSTANCE = new RootSignatureFactory();
        INSTANCE->m_device = a_device;
    }

    return INSTANCE;
}

void RootSignatureFactory::DeleteInstance()
{
    if(INSTANCE)
        delete INSTANCE;
}

RootSignature * RootSignatureFactory::CreateRootSignature(ROOT_SIGNATURE_FLAG a_flag)
{
    RootSignature* output = m_rootSignatureMap[a_flag];
    if (!output) {
        output = new RootSignature(m_device, a_flag);
        m_rootSignatureMap[a_flag] = output;
    }
    return output;
}
