#include "MainApp.h"
MainApp* g_App = NULL;
MainApp::MainApp(HINSTANCE hInstance)
	:D3DApp(hInstance)
{
	mMainWndCaption = L"Main Demo";
	mEnable4xMsaa = false;

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	m_ShaderMan = new ShaderManager;
	m_Camera = new Camera;
}

MainApp::~MainApp()
{
	SafeDelete(m_ShaderMan);
	for (auto m : m_ModelList)
	{
		SafeDelete(m);
	}
	SafeDelete(m_Camera);
}

bool MainApp::Init()
{
	using namespace Ogre;
	if (!D3DApp::Init())
		return false;

	m_Camera->LookAt(Vector3(-8.5f, 3.0f, -8.0f), Vector3::ZERO, Vector3::UNIT_Y);
	m_Camera->UpdateViewMatrix();

	if (!m_ShaderMan->Init())
	{
		return false;
	}
	if (!InitObjects())
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
	// move camera
}

void MainApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (auto m : m_ModelList)
	{
		m->Render();
		m_ShaderMan->GetShader("default")->Render(md3dImmediateContext, m->GetVertexCount(), 0,
			m->GetWorldMatrix(), m_Camera->View(), m_Camera->Proj(), m->GetDiffuseSRV());
	}
	

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

}

bool MainApp::InitObjects()
{
	// obj name, shader, buffer, material
	std::string name = "square";
	std::string shadername = "default";
	TextureShader* shader = m_ShaderMan->GetShader(shadername);
	ModelObject* obj = new ModelObject;
	obj->Init(name, shader);	
	m_ModelList.push_back(obj);
	return true;
}
