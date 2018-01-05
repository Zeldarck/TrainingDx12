#pragma once
#include "d3dx12.h"
typedef
enum ROOT_SIGNATURE_FLAG
{
    ROOT_SIGNATURE_FLAG_NONE = 0,
    ROOT_SIGNATURE_FLAG_TEXTURE = 0x1,
    ROOT_SIGNATURE_FLAG_FULLCOLOR = 0x2,
} 	ROOT_SIGNATURE_FLAG;

DEFINE_ENUM_FLAG_OPERATORS(ROOT_SIGNATURE_FLAG);


class RootSignature
{
private:
   /* D3D12_ROOT_DESCRIPTOR* rootCBVDescriptor;
    D3D12_DESCRIPTOR_RANGE*  descriptorTableRanges[1]; // only one range right now
    D3D12_ROOT_DESCRIPTOR_TABLE* descriptorTable;
    D3D12_ROOT_PARAMETER*  rootParameters;
    D3D12_STATIC_SAMPLER_DESC* sampler;*/
    ID3D12RootSignature* m_rootSignature = nullptr;
    ID3D12Device * m_device = nullptr;

public:
    ID3D12RootSignature* GetRootSignature();
    RootSignature(ID3D12Device* a_device,ROOT_SIGNATURE_FLAG a_flags);
    ~RootSignature();
};

