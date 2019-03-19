#pragma once
#include "d3dUtil.h"
#include <d3dcompiler.h>
#include "Math/OgreMatrix4.h"
/* TO-DO
	1. vs��ps��samplestate����������ͨ��xmlָ��ʹ�õ����
	2. ����shaderparam. ÿ��shader��param��ζ�Ӧ��	
	20180724 ��Ҫ��shader reflection���������shader�ж�ȡ����
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
	// singletex
	bool Render(ID3D11DeviceContext* deviceContext, int vertexCount, int startVertexIndex,
		const Ogre::Matrix4& worldMatrix, const Ogre::Matrix4& projectionMatrix, ID3D11ShaderResourceView* texture);
	// multitex
	bool Render(ID3D11DeviceContext* deviceContext, int vertexCount, int startVertexIndex, 
		const Ogre::Matrix4& worldMatrix, const Ogre::Matrix4& projectionMatrix, std::vector<ID3D11ShaderResourceView*> texture);

	const std::vector<D3D11_INPUT_ELEMENT_DESC>& GetVertexLayoutFromShader() { return m_ReflectLayout; }
	static ui32 GetStride(DXGI_FORMAT format);
	ui32 GetVertexStride() { return m_vertexStride; }
private:
	// PS, VS, input layout, buffer param, sampler state
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3DBlob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, Ogre::Matrix4, Ogre::Matrix4, Ogre::Matrix4, std::vector<ID3D11ShaderResourceView*>);
	void RenderShader(ID3D11DeviceContext*, int VertexCount, int StartVertexLocation);
	// Ŀǰ�������̫�٣��Ƿ��ܸ���desc��ȫ����������������ܵĻ���Ҫ��xml�ж������Ժ�format
	DXGI_FORMAT GetFormatFromDesc(D3D11_SIGNATURE_PARAMETER_DESC inputDesc, int &OutStride);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	// vertex shader params reflect
	std::vector<D3D11_INPUT_ELEMENT_DESC> m_ReflectLayout;
	ui32 m_vertexStride;
};
