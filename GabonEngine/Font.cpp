#include "Font.h"
#include <fstream>
#include "MathHeader.h"
#include "MainApp.h"
#include "TextureShader.h"
Font::Font()
	: m_VertexBuffer(nullptr)
	, m_IndexBuffer(nullptr)
	, m_charList(nullptr)
	, m_Tex(nullptr)
{
}


Font::~Font()
{
	delete[] m_charList;
	SafeRelease(m_Tex);
	SafeRelease(m_IndexBuffer);
	SafeRelease(m_VertexBuffer);
}

bool Font::Init(std::string fontData, std::string fontTex, TextureShader* shader)
{
	bool bRet = true;
	m_Shader = shader;
	
	// init font data
	std::ifstream fin;
	int i;
	char temp;	
	
	// Create the font spacing buffer.
	m_charList = new Character[95];
	if (!m_charList)
	{
		return false;
	}

	// Read in the font size and spacing between chars.
	fin.open(fontData);
	if (fin.fail())
	{
		return false;
	}

	// Read in the 95 used ascii characters for text.
	for (i = 0; i < 95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_charList[i].left;
		fin >> m_charList[i].right;
		fin >> m_charList[i].size;
	}

	// Close the file.
	fin.close();

	bRet &= InitResource(fontTex);
	return bRet;
}

void Font::UpdateVertices(const std::string& sentence, Vector2 screenPos, Vector2 size)
{
	//VertexType* m_Vertices = new VertexType[sentence.size()];
	std::vector<VertexType> temp = m_Vertices;
	m_Vertices.resize(temp.size() + sentence.size()*6);
	for (ui32 i = 0; i < temp.size(); ++i)
	{
		m_Vertices[i] = temp[i];
	}
	const float fontHeight = 16;
	screenPos.x -= g_App->GetScreenSize().x / 2.f;
	screenPos.y = -screenPos.y + g_App->GetScreenSize().y / 2.f ;
	
	float drawX = screenPos.x;
	float drawY = screenPos.y;
	int index = (int)temp.size();
	// build vertex buffer

	for (ui32 i = 0; i < sentence.size(); i++)
	{
		int letter = ((int)sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// First triangle in quad.
			m_Vertices[index].position = Vector3(drawX, drawY, 0.0f);  // Top left.
			m_Vertices[index].texture = Vector2(m_charList[letter].left, 0.0f);
			index++;

			m_Vertices[index].position = Vector3((drawX + m_charList[letter].size), (drawY - fontHeight), 0.0f);  // Bottom right.
			m_Vertices[index].texture = Vector2(m_charList[letter].right, 0.5f);
			index++;

			m_Vertices[index].position = Vector3(drawX, (drawY - fontHeight), 0.0f);  // Bottom left.
			m_Vertices[index].texture = Vector2(m_charList[letter].left, 0.5f);
			index++;

			// Second triangle in quad.
			m_Vertices[index].position = Vector3(drawX, drawY, 0.0f);  // Top left.
			m_Vertices[index].texture = Vector2(m_charList[letter].left, 0.0f);
			index++;

			m_Vertices[index].position = Vector3(drawX + m_charList[letter].size, drawY, 0.0f);  // Top right.
			m_Vertices[index].texture = Vector2(m_charList[letter].right, 0.0f);
			index++;

			m_Vertices[index].position = Vector3((drawX + m_charList[letter].size), (drawY - fontHeight), 0.0f);  // Bottom right.
			m_Vertices[index].texture = Vector2(m_charList[letter].right, 0.5f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + m_charList[letter].size + 1.0f;
		}
	}
}

bool Font::InitResource(std::string fontTex)
{	
	// init tex
	if (!D3DHelper::InitTexture(g_App->GetDevice(), fontTex, m_Tex))
	{
		return false;
	}
	
	return true;
}

void Font::ReleaseVertices()
{
	m_Vertices.clear();
}

void Font::ReleaseBuffer()
{	
	SafeRelease(m_VertexBuffer);
	SafeRelease(m_IndexBuffer);
}

void Font::Render()
{
	ReleaseBuffer();
	UpdateBuffer();
	ui32 stride, offset;
	stride = sizeof(VertexType);
	offset = 0;

	g_App->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	g_App->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_App->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Matrix4 world = Matrix4::IDENTITY;
	m_Shader->Render(g_App->GetDeviceContext(), m_Vertices.size(), 0, world, g_App->GetCamera()->GetOrthoFromCamera(), m_Tex);
}

void Font::UpdateBuffer()
{
	assert(!m_VertexBuffer && !m_IndexBuffer);
	// vertex buffer
	D3D11_BUFFER_DESC vb;
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb.ByteWidth = sizeof(VertexType) * m_Vertices.size();
	vb.CPUAccessFlags = 0;
	vb.MiscFlags = 0;
	vb.StructureByteStride = 0;
	vb.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = m_Vertices.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HR(g_App->GetDevice()->CreateBuffer(&vb, &data, &m_VertexBuffer));
	
	// index buffer
	D3D11_BUFFER_DESC ib;
	ib.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ib.ByteWidth = sizeof(ui32) * m_Vertices.size();
	ib.CPUAccessFlags = 0;
	ib.MiscFlags = 0;
	ib.StructureByteStride = 0;
	ib.Usage = D3D11_USAGE_DEFAULT;

	ui32* indices = new ui32[m_Vertices.size()];
	for (ui32 i = 0; i < m_Vertices.size(); ++i)
	{
		indices[i] = i;
	}
	D3D11_SUBRESOURCE_DATA indexdata;
	indexdata.pSysMem = indices;
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	HR(g_App->GetDevice()->CreateBuffer(&ib, &indexdata, &m_IndexBuffer));

	delete[] indices;
}
