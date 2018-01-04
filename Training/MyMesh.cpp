#include "MyMesh.h"


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

void MyMesh::Draw(ID3D12GraphicsCommandList * a_commandList) {
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
        vList[j] = { m_mesh->Vertices[j].Position.X, m_mesh->Vertices[j].Position.Y, m_mesh->Vertices[j].Position.Z, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX };
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
