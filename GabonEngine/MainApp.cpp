#include "MainApp.h"

MainApp::MainApp(HINSTANCE hInstance)
	:D3DApp(hInstance)
{
	mMainWndCaption = L"Main Demo";
	mEnable4xMsaa = false;

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
}

MainApp::~MainApp()
{
}

bool MainApp::Init()
{
	if (!D3DApp::Init())
		return false;

	InitObjects();
	return true;
}

void MainApp::OnResize()
{

}

void MainApp::UpdateScene(float dt)
{

}

void MainApp::DrawScene()
{

}

void MainApp::OnMouseDown(WPARAM btnState, int x, int y)
{

}

void MainApp::OnMouseUp(WPARAM btnState, int x, int y)
{

}

void MainApp::OnMouseMove(WPARAM btnState, int x, int y)
{

}

void MainApp::InitObjects()
{

}
