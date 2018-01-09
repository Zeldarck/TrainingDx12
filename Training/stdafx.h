#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers.
#endif

//#include "OBJ_Loader.h"
//#include "GameObject.h"
#include "Camera.h"
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <numeric>
#include <time.h>
#include "MyMesh.h"
#include <wincodec.h>
#include "PSOFactory.h"
#include "Shader.h"
#include "ShaderFactory.h"
#include "RootSignature.h"
#include "Util.h"
#include "RootSignatureFactory.h"
#include "Window.h"
#include "RenderEngine.h"
// we will exit the program when this becomes false
bool Running = true;



// main application loop
void mainloop();








bool InitD3D(); // initializes direct3d 12

void Update(); // update the game logic

void UpdatePipeline(); // update the direct3d pipeline (update command lists)

void Render(); // execute the command list

void Cleanup(); // release com ojects and clean up memory



D3D12_VIEWPORT viewport; // area that output from rasterizer will be stretched to.

D3D12_RECT scissorRect; // the area to draw in. pixels outside that area will not be drawn onto


Camera m_camera = Camera(DirectX::XMFLOAT4(0.0f, 0.0f, -4.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f));

GameObject m_scene = GameObject(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

GameObject m_gameObject1 = GameObject(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
GameObject m_gameObject2 = GameObject(DirectX::XMFLOAT4(-2.0f, 0.0f, 0.0f, 0.0f));

MyMesh m_cubeMesh;
MyMesh m_TorusMesh;

float speed = 0.1f;

bool add_stickMan = false;



Window * m_window = nullptr;