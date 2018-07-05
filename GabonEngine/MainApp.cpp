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
}
