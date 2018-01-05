#include "MyMesh.h"
#include "Util.h"

std::vector<MyMesh*> MyMesh::getChildren()
{
    return m_children;
}

D3D12_VERTEX_BUFFER_VIEW * MyMesh::GetVertexBufferView()
{
    return &vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW * MyMesh::GetIndexBufferView()
{
    return &indexBufferView;
}

void MyMesh::SetObj(std::string a_obj)
{
    objl::Loader* Loader = new objl::Loader();

    bool loadout = Loader->LoadFile(a_obj);
    if (Loader->LoadedMeshes.size() > 1) {
        for (int i = 0; i < Loader->LoadedMeshes.size(); ++i) {
            MyMesh* mesh = new MyMesh(&Loader->LoadedMeshes[i]);
            m_children.push_back(mesh);
        }
    }
    else {
        m_mesh = &Loader->LoadedMeshes[0];
    }
}

bool MyMesh::HaveATexture() {
    return m_mesh->MeshMaterial.map_Kd.size() > 0;
}

void MyMesh::Draw(ID3D12GraphicsCommandList * a_commandList) {

    if (!m_mesh) {
        return;
    }

    if (HaveATexture()) {
        // set the descriptor heap
        ID3D12DescriptorHeap* descriptorHeaps[] = { mainDescriptorHeap };
        a_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        // set the descriptor table to the descriptor heap (parameter 1, as constant buffer root descriptor is parameter index 0)
        a_commandList->SetGraphicsRootDescriptorTable(1, mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    }

    a_commandList->IASetVertexBuffers(0, 1, GetVertexBufferView()); // set the vertex buffer (using the vertex buffer view)
    a_commandList->IASetIndexBuffer(GetIndexBufferView()); // set the vertex buffer (using the vertex buffer view)


    a_commandList->DrawIndexedInstanced(GetCountIndex(), 1, 0, 0, 0);
}

void MyMesh::PushOnGPU(ID3D12Device * a_device, ID3D12GraphicsCommandList * a_commandList)
{

    for (MyMesh* a_mesh : m_children) {
        a_mesh->PushOnGPU(a_device, a_commandList);
    }

    if (!m_mesh) {
        return;
    }
    Vertex *vList = new Vertex[m_mesh->Vertices.size()];

    for (int j = 0; j < m_mesh->Vertices.size(); j++)
    {
        vList[j] = { m_mesh->Vertices[j].Position.X, m_mesh->Vertices[j].Position.Y, m_mesh->Vertices[j].Position.Z
          //  , (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX
            , m_mesh->MeshMaterial.Kd.X,  m_mesh->MeshMaterial.Kd.Y,  m_mesh->MeshMaterial.Kd.Z, 1.0f
            ,m_mesh->Vertices[j].TextureCoordinate.X,m_mesh->Vertices[j].TextureCoordinate.Y };
    }

    vBufferSize = sizeof(Vertex) * m_mesh->Vertices.size();


    DWORD *vIndices = new DWORD[m_mesh->Indices.size()];
    for (int j = 0; j < m_mesh->Indices.size(); j++) {
        vIndices[j] = m_mesh->Indices[j];
    }

    vIndexSize = sizeof(DWORD) * m_mesh->Indices.size();

    a_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
        D3D12_HEAP_FLAG_NONE, // no flags
        &CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
        D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
                                        // from the upload heap to this heap
        nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
        IID_PPV_ARGS(&vertexBuffer));

    // we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
    vertexBuffer->SetName(L"Vertex Buffer Resource Heap");


    // create upload heap
    // upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
    // We will upload the vertex buffer using this heap to the default heap
    ID3D12Resource* vBufferUploadHeap;
    a_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
        D3D12_HEAP_FLAG_NONE, // no flags
        &CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
        D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
        nullptr,
        IID_PPV_ARGS(&vBufferUploadHeap));
    vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

    // store vertex buffer in upload heap
    D3D12_SUBRESOURCE_DATA vertexData = {};
    vertexData.pData = reinterpret_cast<BYTE*>(vList); // pointer to our vertex array
    vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
    vertexData.SlicePitch = vBufferSize; // also the size of our triangle vertex data

                                         // we are now creating a command with the command list to copy the data from
                                         // the upload heap to the default heap
    UpdateSubresources(a_commandList, vertexBuffer, vBufferUploadHeap, 0, 0, 1, &vertexData);

    // transition the vertex buffer data from copy destination state to vertex buffer state
    a_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));





    // create default heap
    // default heap is memory on the GPU. Only the GPU has access to this memory
    // To get data into this heap, we will have to upload the data using
    // an upload heap
    a_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
        D3D12_HEAP_FLAG_NONE, // no flags
        &CD3DX12_RESOURCE_DESC::Buffer(vIndexSize), // resource description for a buffer
        D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
                                        // from the upload heap to this heap
        nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
        IID_PPV_ARGS(&indexBuffer));

    // we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
    indexBuffer->SetName(L"Index Buffer Resource Heap");


    // create upload heap
    // upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
    // We will upload the vertex buffer using this heap to the default heap
    ID3D12Resource* vIndexUploadHeap;
    a_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
        D3D12_HEAP_FLAG_NONE, // no flags
        &CD3DX12_RESOURCE_DESC::Buffer(vIndexSize), // resource description for a buffer
        D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
        nullptr,
        IID_PPV_ARGS(&vIndexUploadHeap));
    vIndexUploadHeap->SetName(L"Index Buffer Upload Resource Heap");

    // store vertex buffer in upload heap
    D3D12_SUBRESOURCE_DATA IndexData = {};
    IndexData.pData = reinterpret_cast<BYTE*>(vIndices); // pointer to our vertex array
    IndexData.RowPitch = vIndexSize; // size of all our triangle vertex data
    IndexData.SlicePitch = vIndexSize; // also the size of our triangle vertex data


                                       // we are now creating a command with the command list to copy the data from
                                       // the upload heap to the default heap
    UpdateSubresources(a_commandList, indexBuffer, vIndexUploadHeap, 0, 0, 1, &IndexData);

    // transition the vertex buffer data from copy destination state to vertex buffer state
    a_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(indexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

    SetBufferVertexView();
    SetBufferIndexView();

    PushTextureOnGPU(a_device, a_commandList);
}


void MyMesh::PushTextureOnGPU(ID3D12Device * a_device, ID3D12GraphicsCommandList * a_commandList)
{
    if (!HaveATexture()) {
        return;
    }

    // Load the image from file
    D3D12_RESOURCE_DESC textureDesc;
    int imageBytesPerRow;
    BYTE* imageData;
    int imageSize = Util::LoadImageDataFromFile(&imageData, textureDesc, Util::StringToWString("Assets/" + m_mesh->MeshMaterial.map_Kd).c_str(), imageBytesPerRow);

    // create a default heap where the upload heap will copy its contents into (contents being the texture)
    HRESULT hr = a_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
        D3D12_HEAP_FLAG_NONE, // no flags
        &textureDesc, // the description of our texture
        D3D12_RESOURCE_STATE_COPY_DEST, // We will copy the texture from the upload heap to here, so we start it out in a copy dest state
        nullptr, // used for render targets and depth/stencil buffers
        IID_PPV_ARGS(&textureBuffer));
    if (FAILED(hr))
    {
        return;
    }
    textureBuffer->SetName(L"Texture Buffer Resource Heap");

    UINT64 textureUploadBufferSize;
    // this function gets the size an upload buffer needs to be to upload a texture to the gpu.
    // each row must be 256 byte aligned except for the last row, which can just be the size in bytes of the row
    // eg. textureUploadBufferSize = ((((width * numBytesPerPixel) + 255) & ~255) * (height - 1)) + (width * numBytesPerPixel);
    //textureUploadBufferSize = (((imageBytesPerRow + 255) & ~255) * (textureDesc.Height - 1)) + imageBytesPerRow;
    a_device->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);

    // now we create an upload heap to upload our texture to the GPU
    hr = a_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
        D3D12_HEAP_FLAG_NONE, // no flags
        &CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize), // resource description for a buffer (storing the image data in this heap just to copy to the default heap)
        D3D12_RESOURCE_STATE_GENERIC_READ, // We will copy the contents from this heap to the default heap above
        nullptr,
        IID_PPV_ARGS(&textureBufferUploadHeap));
    if (FAILED(hr))
    {
        return;
    }
    textureBufferUploadHeap->SetName(L"Texture Buffer Upload Resource Heap");

    // store vertex buffer in upload heap
    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = &imageData[0]; // pointer to our image data
    textureData.RowPitch = imageBytesPerRow; // size of all our triangle vertex data
    textureData.SlicePitch = imageBytesPerRow * textureDesc.Height; // also the size of our triangle vertex data

                                                                    // Now we copy the upload buffer contents to the default heap
    UpdateSubresources(a_commandList, textureBuffer, textureBufferUploadHeap, 0, 0, 1, &textureData);

    // transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
    a_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    // create the descriptor heap that will store our srv
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    hr = a_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mainDescriptorHeap));
    if (FAILED(hr))
    {
        return;
    }

    // now we create a shader resource view (descriptor that points to the texture and describes it)
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    a_device->CreateShaderResourceView(textureBuffer, &srvDesc, mainDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    
}


void MyMesh::SetBufferVertexView()
{
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(Vertex);
    vertexBufferView.SizeInBytes = vBufferSize;
}

void MyMesh::SetBufferIndexView()
{
    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = vIndexSize;
    indexBufferView.Format = DXGI_FORMAT_R32_UINT; // 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
}


int MyMesh::GetCountIndex()
{
    
    return m_mesh ? m_mesh->Indices.size() : 0;
}


MyMesh::MyMesh()
{
}


MyMesh::MyMesh(objl::Mesh* a_mesh)
{
    m_mesh = a_mesh;
}

MyMesh::~MyMesh()
{
}



