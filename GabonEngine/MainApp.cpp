#include "MainApp.h"

MainApp* g_App = NULL;
MainApp::MainApp(HINSTANCE hInstance)
	:D3DApp(hInstance)
{
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
}

MainApp::~MainApp()
{
	SafeDelete(m_ShaderMan);
	SafeDelete(m_ModelMan);
	SafeDelete(m_BitmapMan);
	SafeDelete(m_FontMan);

	SafeDelete(m_Camera);
	SafeDelete(m_Light);
}

bool MainApp::Init()
{
	using namespace Ogre;
	if (!D3DApp::Init())
		return false;

	//m_Camera->LookAt(Vector3(-8.5f, 3.0f, -8.0f), Vector3::ZERO, Vector3::UNIT_Y);
	m_Camera->UpdateViewMatrix();
	m_Camera->InitBuffer();

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
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 以后需要将model按shader分类
	// 暂时先按Model来绘制
	m_Light->Render();
	m_ModelMan->Render();
	
	EnableZBuffer(false);
	m_BitmapMan->Render();
	//EnableAlphaTest(false);
	m_FontMan->Render();
	//EnableAlphaTest(true);
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
