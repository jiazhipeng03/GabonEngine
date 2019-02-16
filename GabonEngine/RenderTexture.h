#pragma once
#include "d3dUtil.h"
class RenderTexture
{
public:
	RenderTexture();
	~RenderTexture();

	bool Initialize(int width, int height);
	void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView,
		float red, float green, float blue, float alpha);
	ID3D11ShaderResourceView* GetSRV() { return m_ShaderResourceView; }
private:
	ID3D11Texture2D* m_RenderTargetTexture;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11ShaderResourceView* m_ShaderResourceView;
};