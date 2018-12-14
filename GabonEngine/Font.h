#pragma once
#include "MathHeader.h"
#include "d3dUtil.h"
class TextureShader;
class Font
{
	struct Character
	{
		float left, right;
		int size;
	};
	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};
public:
	Font();
	~Font();
	bool Init(std::string fontData, std::string fontTex, TextureShader* shader);
	
	// UpdateVertices
	void UpdateVertices(const std::string& sentence, Vector2 screenPos, Vector2 size);	
	void ReleaseVertices();
	// UpdateBuffer
	// Vertices全写入后只调用一次更新一次Buffer
	void UpdateBuffer();
	void ReleaseBuffer();

	void Render();	
private:
	bool InitResource(std::string fontTex);
	

private:
	Character* m_charList;
	// render
	ID3D11ShaderResourceView* m_Tex;
	ID3D11Buffer* m_VertexBuffer;
	ID3D11Buffer* m_IndexBuffer;
	TextureShader* m_Shader;
	std::vector<VertexType> m_Vertices;
};

