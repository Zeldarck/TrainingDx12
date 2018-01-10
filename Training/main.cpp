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

	// clean up everything
	Cleanup();

	return 0;
}


void mainloop() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (m_window->IsOpen())
	{
		// run game code
        m_window->Update();
		UpdateLogic();
        RenderEngine::GetInstance()->PrepareToRender();
        UpdateRender();
        RenderEngine::GetInstance()->Render();

		
	}
}

bool InitD3D()
{

    RenderEngine* renderEngine = RenderEngine::GetInstance(m_window);

    renderEngine->InitD3D();


    //Create Basis Objects 
    m_cubeMesh.SetObj("Assets/soldier.obj");
    m_TorusMesh.SetObj("Assets/cube.obj"); 


    //Push them to GPU
    renderEngine->PrepareToRender();
    m_cubeMesh.PushOnGPU(renderEngine->GetDevice(), renderEngine->GetCommandList());
    m_TorusMesh.PushOnGPU(renderEngine->GetDevice(), renderEngine->GetCommandList());
    m_gameObject1.CreateCBUploadHeap(renderEngine->GetDevice(), renderEngine->GetFrameBufferCount());
    m_gameObject2.CreateCBUploadHeap(renderEngine->GetDevice(), renderEngine->GetFrameBufferCount());
    renderEngine->Render();


    m_gameObject1.Scale(0.05f, 0.05f, 0.05f);
    m_gameObject2.SetParent(&m_gameObject1);
    m_gameObject1.SetParent(&m_scene);

    m_gameObject1.SetMesh(&m_cubeMesh);
    m_gameObject2.SetMesh(&m_TorusMesh);

    // build projection and view matrix
    DirectX::XMMATRIX tmpMat;
    m_camera.SetFarZ(1000.0f);
    m_camera.SetNearZ(0.01f);
    m_camera.SetIsOrthographic(false);
    m_camera.SetFOV(45.0f);
    m_camera.SetView(m_window->GetWidth(), m_window->GetHeight());

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

    m_camera.Rotate(0.0f, 1.0f, 0.0f, -m_window->GetRotX()/1000.0f);
    m_camera.Rotate(1.0f, 0.0f, 0.0f, -m_window->GetRotY()/1000.0f);


 }

void UpdateLogic()
{

    UpdateCamera();


/*    m_gameObject1.Rotate(0.0f, 0.0f, 1.0f, 0.0001f);
    m_gameObject1.Rotate(0.0f, 1.0f, 0.0f, 0.0002f);
    m_gameObject1.Rotate(1.0f, 0.0f, 0.0f, 0.0003f);

    m_gameObject2.Rotate(0.0f, 0.0f, 1.0f, 0.0001f);
    m_gameObject2.Rotate(0.0f, 1.0f, 0.0f, 0.0002f);
    m_gameObject2.Rotate(1.0f, 0.0f, 0.0f, 0.0003f);   */
}

void UpdateRender()
{
    m_scene.Draw( RenderEngine::GetInstance()->GetCommandList(), RenderEngine::GetInstance()->GetFrameIndex(), &m_camera);
}


void Cleanup()
{
    PSOFactory::DeleteInstance();
    ShaderFactory::DeleteInstance();
    RootSignatureFactory::DeleteInstance();
    RenderEngine::DeleteInstance();
}

