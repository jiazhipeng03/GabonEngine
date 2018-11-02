#pragma once
#include <windows.h>
#include "d3dApp.h"
#include "Object.h"
#include "Light.h"
#include "Camera.h"
#include "ShaderManager.h"
//#include "ModelObject.h"
#include "DDSTextureLoader.h"
#include "ModelManager.h"

class MainApp : public D3DApp
{
public:
	// 	enum RenderOptions
	// 	{
	// 		Lighting = 0,
	// 		Textures = 1,
	// 		TexturesAndFog = 2
	// 	};

	MainApp(HINSTANCE hInstance);
	~MainApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;
	void OnMouseWheel(UINT key, int zDelta, int x, int y) override;
	virtual void OnKeyDown(UINT key) override;
	ShaderManager* GetShaderMan() { return m_ShaderMan; }
	Camera* GetCamera() { return m_Camera; }
	
private:	
	
//	RenderOptions mRenderOptions;
	
//	POINT mLastMousePos;
	// get eye, view, proj
	Camera* m_Camera;

	ModelManager* m_ModelMan;
	ShaderManager* m_ShaderMan;

	Ogre::Vector2 m_CurPos;
	Ogre::Vector2 m_LastPos;
};

extern MainApp* g_App;