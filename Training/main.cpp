#include "stdafx.h"


int WINAPI WinMain(HINSTANCE hInstance,    //Main windows function
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)

{
	// create the window
    m_window = new Window(hInstance, nShowCmd);

	if (!m_window)
	{
		MessageBox(0, L"Window Initialization - Failed",
			L"Error", MB_OK);
		return 1;
	}

	// initialize direct3d
	if (!InitD3D())
	{
		MessageBox(0, L"Failed to initialize direct3d 12",
			L"Error", MB_OK);
		Cleanup();
		return 1;
	}

	// start the main loop
	mainloop();

	// we want to wait for the gpu to finish executing the command list before we start releasing everything
	WaitForPreviousFrame();

	// close the fence event
	CloseHandle(fenceEvent);

	// clean up everything
	Cleanup();

	return 0;
}


void mainloop() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (Running)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// run game code
			Update(); // update the game logic
			Render(); // execute the command queue (rendering the scene is the result of the gpu executing the command lists)
		}
	}
}

bool InitD3D()
{


/// faire tout ça à la prochaine frame
    m_cubeMesh.SetObj("Assets/soldier.obj");
    m_TorusMesh.SetObj("Assets/cube.obj"); ?

    m_cubeMesh.PushOnGPU(device, commandList);
    m_TorusMesh.PushOnGPU(device, commandList);
/////



    m_gameObject1.CreateCBUploadHeap(device, frameBufferCount);
    m_gameObject2.CreateCBUploadHeap(device, frameBufferCount);


	// Now we execute the command list to upload the initial assets (triangle data)
	commandList->Close();
	ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// increment the fence value now, otherwise the buffer might not be uploaded by the time we start drawing
	fenceValue[frameIndex]++;
	hr = commandQueue->Signal(fence[frameIndex], fenceValue[frameIndex]);
	if (FAILED(hr))
	{
		Running = false;
	}
    
	// Fill out the Viewport
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = m_window->GetWidth();
	viewport.Height = m_window->GetHeight();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = m_window->GetWidth();
	scissorRect.bottom = m_window->GetHeight();

    // build projection and view matrix
    DirectX::XMMATRIX tmpMat;
    m_camera.SetFarZ(1000.0f);
    m_camera.SetNearZ(0.01f);
    m_camera.SetIsOrthographic(false);
    m_camera.SetFOV(45.0f);
    m_camera.SetView(m_window->GetWidth(), m_window->GetHeight());

    m_gameObject1.Scale(0.05f, 0.05f, 0.05f);
    m_gameObject2.SetParent(&m_gameObject1);
    m_gameObject1.SetParent(&m_scene);

    m_gameObject1.SetMesh(&m_cubeMesh);
    m_gameObject2.SetMesh(&m_TorusMesh);

    return true;
}

void UpdateCamera() {

    //Move Up and bottom
    if (GetAsyncKeyState('Z')) {
        m_camera.Translate(0.0f, 0.0f, -speed/4.0f);
    }
    else if (GetAsyncKeyState('S')) {
        m_camera.Translate(0.0f, 0.0f, speed / 4.0f);
    }

    //Move Right and Left
    if (GetAsyncKeyState('Q')) {
        m_camera.Translate(speed / 4.0f, 0.0f, 0.0f);
    }
    else if (GetAsyncKeyState('D')) {
        m_camera.Translate(-speed / 4.0f, 0.0f, 0.0f);
    }

    //Move forward/Backward
    if (GetAsyncKeyState('P')) {
        m_camera.Translate(0.0f, -speed / 4.0f, 0.0f);
    }
    else if (GetAsyncKeyState('M')) {
        m_camera.Translate(0.0f, speed / 4.0f, 0.0f);
    }

    //Rotate Y axis
    if (GetAsyncKeyState('A')) {
        m_camera.Rotate(0.0f, 1.0f, 0.0f, speed);
    }
    else if (GetAsyncKeyState('E')) {
        m_camera.Rotate(0.0f, 1.0f, 0.0f, -speed);
    }
    
    //Rotate X axis
    if (GetAsyncKeyState('R')) {
        m_camera.Rotate(0.0f, 0.0f, 1.0f, speed);
    }
    else if (GetAsyncKeyState('T')) {
        m_camera.Rotate(0.0f, 0.0f, 1.0f, -speed);
    }
    
    //Rotate Z axis
    if (GetAsyncKeyState('F')) {
        m_camera.Rotate(1.0f, 0.0f, 0.0f, speed);
    }
    else if (GetAsyncKeyState('G')) {
        m_camera.Rotate(1.0f, 0.0f, 0.0f, -speed);
    }

 }

void Update()
{

    UpdateCamera();


/*    m_gameObject1.Rotate(0.0f, 0.0f, 1.0f, 0.0001f);
    m_gameObject1.Rotate(0.0f, 1.0f, 0.0f, 0.0002f);
    m_gameObject1.Rotate(1.0f, 0.0f, 0.0f, 0.0003f);

    m_gameObject2.Rotate(0.0f, 0.0f, 1.0f, 0.0001f);
    m_gameObject2.Rotate(0.0f, 1.0f, 0.0f, 0.0002f);
    m_gameObject2.Rotate(1.0f, 0.0f, 0.0f, 0.0003f);   */
}

void UpdatePipeline()
{
	HRESULT hr;

	// We have to wait for the gpu to finish with the command allocator before we reset it
	WaitForPreviousFrame();

	// we can only reset an allocator once the gpu is done with it
	// resetting an allocator frees the memory that the command list was stored in
	hr = commandAllocator[frameIndex]->Reset();
	if (FAILED(hr))
	{
		Running = false;
	}

	// reset the command list. by resetting the command list we are putting it into
	// a recording state so we can start recording commands into the command allocator.
	// the command allocator that we reference here may have multiple command lists
	// associated with it, but only one can be recording at any time. Make sure
	// that any other command lists associated to this command allocator are in
	// the closed state (not recording).
	// Here you will pass an initial pipeline state object as the second parameter,
	// but in this tutorial we are only clearing the rtv, and do not actually need
	// anything but an initial default pipeline, which is what we get by setting
	// the second parameter to NULL
	hr = commandList->Reset(commandAllocator[frameIndex], nullptr);
	if (FAILED(hr))
	{
		Running = false;
	}

    // here we start recording commands into the commandList (which all the commands will be stored in the commandAllocator)

	// transition the "frameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE,&dsvHandle);
	// Clear the render target by using the ClearRenderTargetView command
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// draw triangle
	commandList->RSSetViewports(1, &viewport); // set the viewports
	commandList->RSSetScissorRects(1, &scissorRect); // set the scissor rects
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology


    m_scene.Draw( commandList, frameIndex, &m_camera);

    commandList->ClearDepthStencilView(dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


    /*  //Not good way? Don't use device in that?
    if (GetAsyncKeyState(VK_LBUTTON) && !add_stickMan) {
        
        PSOFactory::GetInstance()->CreatePSO(PSO_FLAGS_FULLCOLOR);

        MyMesh* sphere = new MyMesh();
        GameObject* go = new GameObject(DirectX::XMFLOAT4(-1.0f,0.0f,0.0f,0.0f));
        go->SetParent(&m_gameObject2);
        sphere->SetObj("sphere.obj");

        sphere->PushOnGPU(device, commandList);
        go->SetMesh(sphere);
        go->CreateCBUploadHeap(device, frameBufferCount);
        add_stickMan = true;
    }*/

    
	// transition the "frameIndex" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
	// warning if present is called on the render target when it's not in the present state
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	hr = commandList->Close();
	if (FAILED(hr))
	{
		Running = false;
	}
}


void Cleanup()
{
    PSOFactory::DeleteInstance();
    ShaderFactory::DeleteInstance();
    RootSignatureFactory::DeleteInstance();
    RenderEngine::DeleteInstance();
}

