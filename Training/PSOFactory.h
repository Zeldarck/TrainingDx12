#pragma once
#include <unordered_map>
#include "d3dx12.h"
#include "PSO.h"
#include "RootSignatureFactory.h"



//Look les placement new
// look les uniqueptr
//ne pas release les trucs de gpu pose des bleme de secu

class PSOFactory
{
private:
    PSOFactory() = default;
    ~PSOFactory() = default;
    //destroy dans une static method les pointeur static
    static PSOFactory* INSTANCE;
    std::unordered_map<PSO_FLAGS, PSO*> m_psoMap;
    ID3D12Device * m_device;
    ID3D12RootSignature* MakeRootSignature(PSO_FLAGS a_flag);

    D3D12_SHADER_BYTECODE * MakePixelShader(PSO_FLAGS a_flag);

    D3D12_SHADER_BYTECODE * MakeVertexShader(PSO_FLAGS a_flag);

    D3D12_INPUT_LAYOUT_DESC * MakeInputLayoutDesc(PSO_FLAGS a_flag);


public:
    DXGI_SAMPLE_DESC m_sampleDesc;
   

    static PSOFactory * GetInstance(ID3D12Device * a_device, DXGI_SAMPLE_DESC a_sampleDesc);

   static PSOFactory * GetInstance(ID3D12Device * a_device);

    static void DeleteInstance();
    static PSOFactory * ResetInstance(ID3D12Device * a_device);



    PSO* CreatePSO(PSO_FLAGS a_flag);

};

