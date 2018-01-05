#include "RootSignature.h"




int RootSignature::GetDescriptorTable(D3D12_ROOT_DESCRIPTOR_TABLE*& descriptorTable, D3D12_STATIC_SAMPLER_DESC*& sampler)
{
    int output = 1;
    // create a descriptor range (descriptor table) and fill it out
    // this is a range of descriptors inside a descriptor heap
    D3D12_DESCRIPTOR_RANGE*  descriptorTableRanges = new D3D12_DESCRIPTOR_RANGE[output]; // only one range right now
    descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
    descriptorTableRanges[0].NumDescriptors = 1; // we only have one texture right now, so the range is only 1
    descriptorTableRanges[0].BaseShaderRegister = 0; // start index of the shader registers in the range
    descriptorTableRanges[0].RegisterSpace = 0; // space 0. can usually be zero
    descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables

    descriptorTable = new D3D12_ROOT_DESCRIPTOR_TABLE[1];                                                                               // create a descriptor table
    descriptorTable[0].NumDescriptorRanges = output;// _countof(descriptorTableRanges); // we only have one range
    descriptorTable[0].pDescriptorRanges = descriptorTableRanges; // the pointer to the beginning of our ranges array


    sampler = new D3D12_STATIC_SAMPLER_DESC[1];                                                                               // create a descriptor table
    sampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    sampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler[0].MipLODBias = 0;
    sampler[0].MaxAnisotropy = 0;
    sampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler[0].MinLOD = 0.0f;
    sampler[0].MaxLOD = D3D12_FLOAT32_MAX;
    sampler[0].ShaderRegister = 0;
    sampler[0].RegisterSpace = 0;
    sampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


    return output;
}

ID3D12RootSignature * RootSignature::GetRootSignature()
{
    return m_rootSignature;
}

RootSignature::RootSignature(ID3D12Device* a_device, ROOT_SIGNATURE_FLAG a_flags)
{
    m_device = a_device;

    D3D12_ROOT_DESCRIPTOR_TABLE* descriptorTable = nullptr;
    D3D12_STATIC_SAMPLER_DESC* sampler = nullptr;

    int numTextures = 0;
    
    if (a_flags == ROOT_SIGNATURE_FLAG_TEXTURE) {
        numTextures = GetDescriptorTable(descriptorTable, sampler);
    }

    // create a root descriptor, which explains where to find the data for this root parameter
    D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
    rootCBVDescriptor.RegisterSpace = 0;
    rootCBVDescriptor.ShaderRegister = 0;

                                                                   // create a root parameter for the root descriptor and fill it out
    D3D12_ROOT_PARAMETER*  rootParameters = new D3D12_ROOT_PARAMETER[1 + numTextures]; // two root parameters
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // this is a constant buffer view root descriptor
    rootParameters[0].Descriptor = rootCBVDescriptor; // this is the root descriptor for this root parameter
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // our pixel shader will be the only shader accessing this parameter for now
                                                                         // fill out the parameter for our descriptor table. Remember it's a good idea to sort parameters by frequency of change. Our constant
                                                                         // buffer will be changed multiple times per frame, while our descriptor table will not be changed at all (in this tutorial)
    for (int i = 0; i < numTextures; ++i) {
        rootParameters[1 + i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // this is a descriptor table
        rootParameters[1 + i].DescriptorTable = descriptorTable[i]; // this is our descriptor table for this root parameter
        rootParameters[1 + i].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // our pixel shader will be the only shader accessing this parameter for now
    }
                                                                        // create a static sampler


    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(1 + numTextures, // we have 2 root parameters
        rootParameters, // a pointer to the beginning of our root parameters array
        numTextures, // we have one static sampler
        sampler, // a pointer to our static sampler (array)
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
    ID3DBlob* signature;
    ID3DBlob* errorBuff;


    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBuff);
    if (FAILED(hr))
    {
        OutputDebugStringA((char*)errorBuff->GetBufferPointer());

        return;
    }

    hr = m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
    if (FAILED(hr))
    {
        return;
    }
}

RootSignature::~RootSignature()
{
}
