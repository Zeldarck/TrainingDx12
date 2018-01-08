#pragma once
#include <unordered_map>
#include "d3dx12.h"
#include "RootSignature.h"

class RootSignatureFactory
{
private :
    RootSignatureFactory() = default;
    ~RootSignatureFactory() = default;

    static RootSignatureFactory* INSTANCE;
    std::unordered_map<ROOT_SIGNATURE_FLAG, RootSignature*> m_rootSignatureMap;
    ID3D12Device * m_device;


public:

    static RootSignatureFactory * GetInstance(ID3D12Device * a_device);
    static void DeleteInstance();
    RootSignature* CreateRootSignature(ROOT_SIGNATURE_FLAG a_flag);

};

