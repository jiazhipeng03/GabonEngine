#pragma once
#include <windows.h>
#include "d3dApp.h"
#include "Object.h"
class MainApp : public D3DApp
{
public:
	enum RenderOptions
	{
		Lighting = 0,
		Textures = 1,
		TexturesAndFog = 2
	};

	MainApp(HINSTANCE hInstance);
	~MainApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void InitObjects();
	
private:
	
	ID3D11ShaderResourceView* mFloorDiffuseMapSRV;
	ID3D11ShaderResourceView* mWallDiffuseMapSRV;
	ID3D11ShaderResourceView* mMirrorDiffuseMapSRV;
	ID3D11ShaderResourceView* mDirtDiffuseMapSRV;
	ID3D11ShaderResourceView* mStoneDiffuseMapSRV;

	DirectionalLight mDirLights[3];
	
	RenderOptions mRenderOptions;
	
	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

	// get eye, view, proj
	Camera* m_Camera;

	std::vector<Object*> ModelList;
};

