#include "MainApp.h"
#include "RenderTexture.h"
#include "Light.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "DDSTextureLoader.h"
#include "ModelManager.h"
#include "BitmapManager.h"
#include "FontManager.h"
MainApp* g_App = NULL;
MainApp::MainApp(HINSTANCE hInstance)
	:D3DApp(hInstance)
{
	//_CrtSetBreakAlloc(4283);
	
	mMainWndCaption = L"Main Demo";
	mEnable4xMsaa = false;

// 	mLastMousePos.x = 0;
// 	mLastMousePos.y = 0;

	m_ShaderMan = new ShaderManager;
	m_ModelMan = new ModelManager;
	m_BitmapMan = new BitmapManager;
	m_Camera = new Camera;
	m_Light = new DirectionalLight;
	m_CurPos = Ogre::Vector2::ZERO;
	m_LastPos = Ogre::Vector2::ZERO;
	m_FontMan = new FontManager;
	m_RenderTexture = new RenderTexture;
}

MainApp::~MainApp()
{
	SafeDelete(m_ShaderMan);
	SafeDelete(m_ModelMan);
	SafeDelete(m_BitmapMan);
	SafeDelete(m_FontMan);

	SafeDelete(m_Camera);
	SafeDelete(m_Light);
	SafeDelete(m_Frustum);
}

bool MainApp::Init()
{
	using namespace Ogre;
	if (!D3DApp::Init())
		return false;

	//m_Camera->LookAt(Vector3(-8.5f, 3.0f, -8.0f), Vector3::ZERO, Vector3::UNIT_Y);
	m_Camera->UpdateViewMatrix();
	m_Camera->InitBuffer();


	m_RenderTexture->Initialize(100, 100);
	if (!m_ShaderMan->Init("shader.xml"))
	{
		return false;
	}
	if (!m_ModelMan->Init("model.xml"))
	{
		return false;
	}
	if (!m_Light->Init())
		return false;

	if (!m_BitmapMan->Init("model.xml"))
		return false;

	if (!m_FontMan->Init())
	{
		return false;
	}
	return true;
}

void MainApp::OnResize()
{
	D3DApp::OnResize();
	m_Camera->SetLens(0.25f * Ogre::Math::PI, AspectRatio(), 1.0f, 1000.0f);
}

void MainApp::UpdateScene(float dt)
{
	// Update
	m_Camera->UpdateBuffer();
	m_Camera->UpdateViewMatrix();
	m_FontMan->Print("test", Vector2(0, 0), Vector2(1, 1));
	// test bitmap UpdateBuffer
// 	static float t = 0;
// 	t += dt * Math::PI;
// 	m_BitmapMan->GetBitmap(0)->SetPosition(Vector2((sin(t)+1) * 400.f, (sin(t) + 1) * 300.f));
}

void MainApp::DrawScene()
{
	RenderToTexture();
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 以后需要将model按shader分类?
	m_Light->Render();
	m_Frustum = new Frustum;
	m_Frustum->ConstructFrustum(1000.f, m_Camera->Proj(), m_Camera->View());
	m_ModelMan->Render(m_Frustum);
	
	EnableZBuffer(false);
	m_BitmapMan->Render();
	EnableAlphaBlending(true);
	m_FontMan->Render();
	EnableAlphaBlending(false);
	EnableZBuffer(true);
	HR(mSwapChain->Present(0, 0));
}

void MainApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	
}

void MainApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MainApp::OnMouseMove(WPARAM btnState, int x, int y)
{	
	m_CurPos = Ogre::Vector2(float(x), float(y));
	Ogre::Vector2 delta = m_CurPos - m_LastPos;
	float speed = 0.003f;
	if (m_MouseState == MOUSE_STATE::L_DOWN)
	{
		if(delta.y)
		{
			speed = delta.y < 0 ? speed : -speed;
			m_Camera->Pitch(speed);
		}
		else if(delta.x)
		{
			speed = delta.x < 0 ? speed : -speed;
			m_Camera->Yaw(speed);
		}
	}
	m_LastPos = m_CurPos;
}

void MainApp::OnMouseWheel(UINT key, int zDelta, int x, int y)
{
	float speed = 5.f;
	float delta = zDelta < 0 ? -5.f: 5.f;
	Ogre::Vector3 dir = m_Camera->GetLook();
	m_Camera->Move(dir*delta);
}

void MainApp::OnKeyDown(UINT key)
{
	float speed = 0.5f;
	if (key == 'w' || key == 'W')
	{		
		m_Camera->Move(speed * m_Camera->GetLook());
	}
	else if (key == 's' || key == 'S')
	{
		m_Camera->Move(-speed * m_Camera->GetLook());
	}
	else if (key == 'a' || key == 'A')
	{
		m_Camera->Move(-speed * m_Camera->GetRight());
	}
	else if (key == 'd' || key == 'D')
	{
		m_Camera->Move(speed * m_Camera->GetRight());
	}
}

ShaderManager* MainApp::GetShaderMan()
{
	return m_ShaderMan;
}

void MainApp::RenderToTexture()
{
	bool result;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(md3dImmediateContext, GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(md3dImmediateContext, GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	result = RenderScene();
	if (!result)
	{
		return;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	SetBackBufferRenderTarget();

	Bitmap* renderTexture = m_BitmapMan->CreateBitmap();
	std::vector<std::string> texNames;
	TextureShader* shader = m_ShaderMan->GetShader("bitmap");
	
	renderTexture->Init(Vector2(0, 100), Vector2(100.0f), g_App->GetScreenSize(), texNames, shader);
	renderTexture->SetTextureResource(m_RenderTexture->GetSRV());
	return;
}

bool MainApp::RenderScene()
{
	//Matrix4 worldMatrix, viewMatrix, projectionMatrix;
	//bool result;
	static float rotation = 0.0f;


	// Generate the view matrix based on the camera's position.
	//m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	
	// Update the rotation variable each frame.
	rotation += (float)Math::PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	
	//D3DXMatrixRotationY(&worldMatrix, rotation);
	m_Light->Render();
	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_ModelMan->Render(NULL);

	// Render the model using the light shader.
// 	result = m_Light->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
// 		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());

	return true;
}

void MainApp::SetBackBufferRenderTarget()
{
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, GetDepthStencilView());
}
