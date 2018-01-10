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
	//WaitForPreviousFrame();


	// clean up everything
	Cleanup();

	return 0;
}


void mainloop() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (Running)
	{
		// run game code
        m_window->Update();
        RenderEngine::GetInstance()->PrepareToRender();
		Update(); // update the game logic
        UpdatePipeline();
        RenderEngine::GetInstance()->Render();

		
	}
}

bool InitD3D()
{

    RenderEngine* renderEngine = RenderEngine::GetInstance(m_window);
    bool res = renderEngine->InitD3D();
    if (!res) {
        int k = 0;
    }

/// faire tout ça à la prochaine frame
    m_cubeMesh.SetObj("Assets/soldier.obj");
    m_TorusMesh.SetObj("Assets/cube.obj"); 


    renderEngine->PrepareToRender();
    m_cubeMesh.PushOnGPU(renderEngine->GetDevice(), renderEngine->GetCommandList());
    m_TorusMesh.PushOnGPU(renderEngine->GetDevice(), renderEngine->GetCommandList());
/////



    m_gameObject1.CreateCBUploadHeap(renderEngine->GetDevice(), renderEngine->GetFrameBufferCount());
    m_gameObject2.CreateCBUploadHeap(renderEngine->GetDevice(), renderEngine->GetFrameBufferCount());

    renderEngine->Render();

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


    m_scene.Draw( RenderEngine::GetInstance()->GetCommandList(), RenderEngine::GetInstance()->GetFrameIndex(), &m_camera);


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

}


void Cleanup()
{
    PSOFactory::DeleteInstance();
    ShaderFactory::DeleteInstance();
    RootSignatureFactory::DeleteInstance();
    RenderEngine::DeleteInstance();
}

