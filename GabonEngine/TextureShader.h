#pragma once
#include "d3dUtil.h"
#include <d3dcompiler.h>
#include "Math/OgreMatrix4.h"
/* TO-DO
	1. vs，ps，samplestate的组合情况，通过xml指定使用的组合
	2. 设置shaderparam. 每个shader的param如何对应？	
	20180724 需要用shader reflection来解决。从shader中读取参数
*/
class TextureShader
{
	struct MatrixBufferType
	{
		Ogre::Matrix4 world;
		Ogre::Matrix4 view;
		Ogre::Matrix4 projection;
	};
public:
	TextureShader();
	~TextureShader();

	bool Initialize(std::string vsFileName, std::string psFileName);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int vertexCount, int startVertexIndex, Ogre::Matrix4& worldMatrix, Ogre::Matrix4& projectionMatrix, ID3D11ShaderResourceView* texture);
private:
	// PS, VS, input layout, buffer param, sampler state
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3DBlob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, Ogre::Matrix4&, Ogre::Matrix4&, Ogre::Matrix4&, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int VertexCount, int StartVertexLocation);
	// 目前考虑情况太少，是否能根据desc完全创建出来？如果不能的话需要从xml中定义属性和format
	DXGI_FORMAT GetFormatFromDesc(D3D11_SIGNATURE_PARAMETER_DESC inputDesc, int &OutStride);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};
