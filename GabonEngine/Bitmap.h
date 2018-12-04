#pragma once
#include "d3dUtil.h"
#include "MathHeader.h"
#include "TextureShader.h"
class Bitmap
{
	struct VertexType
	{
		Ogre::Vector3 position;
		Ogre::Vector2 texture;
	};
public:
	Bitmap();
	~Bitmap();
	bool Init(Ogre::Vector2 position, Ogre::Vector2 imgSize, Ogre::Vector2 screenSize, std::string texName, TextureShader* shader);	
	void SetPosition(Ogre::Vector2 position);
	void SetScreenSize(Ogre::Vector2 screenSize);
	void Render();
	void RenderBuffer();
	void RenderShader();
	void UpdateBuffer();

private:	
	TextureShader* m_Shader;
	ID3D11Buffer* m_IndexBuffer;
	ID3D11Buffer* m_VertexBuffer;
	ID3D11ShaderResourceView* m_Tex;
	Ogre::Vector2 m_Position;
	Ogre::Vector2 m_ImageSize;
	Ogre::Vector2 m_ScreenSize;

	std::vector<VertexType> m_Vertices;
	int m_VertexCount = 6;
	int m_IndexCount = 6;
};

