#include "Bitmap.h"
#include "MainApp.h"


Bitmap::Bitmap()
{
}


Bitmap::~Bitmap()
{
	SafeRelease(m_Tex);
	SafeRelease(m_VertexBuffer);
	SafeRelease(m_IndexBuffer);
}

bool Bitmap::Init(Ogre::Vector2 position, Ogre::Vector2 imgSize, Ogre::Vector2 screenSize, std::string texName, TextureShader* shader)
{	
	m_Shader = shader;
	D3DHelper::InitTexture(g_App->GetDevice(), texName, m_Tex);
	m_ImageSize = imgSize;
	SetPosition(position);
	SetScreenSize(screenSize);

	m_Vertices.resize(m_VertexCount);

	// Init buffer
	D3D11_BUFFER_DESC vb;
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb.ByteWidth = sizeof(VertexType) * m_VertexCount;
	vb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vb.MiscFlags = 0;
	vb.StructureByteStride = 0;
	vb.Usage = D3D11_USAGE_DYNAMIC;
// 	D3D11_SUBRESOURCE_DATA vertexData;
// 	vertexData.pSysMem = &m_Vertices[0];
// 	vertexData.SysMemPitch = 0;
// 	vertexData.SysMemSlicePitch = 0;

	HRESULT result = g_App->GetDevice()->CreateBuffer(&vb, nullptr, &m_VertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	unsigned long indices[6] = { 0,1,2,3,4,5 };
	D3D11_BUFFER_DESC ib;
	ib.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ib.ByteWidth = sizeof(unsigned long) * m_IndexCount;
	ib.CPUAccessFlags = 0;
	ib.MiscFlags = 0;
	ib.StructureByteStride = 0;
	ib.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	result = g_App->GetDevice()->CreateBuffer(&ib, &indexData, &m_IndexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void Bitmap::SetPosition(Ogre::Vector2 position)
{
	m_Position = position;
}

void Bitmap::SetScreenSize(Ogre::Vector2 screenSize)
{
	m_ScreenSize = screenSize;
}

void Bitmap::Render()
{
	RenderBuffer();
	RenderShader();
}

void Bitmap::RenderBuffer()
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	g_App->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	g_App->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	g_App->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Bitmap::RenderShader()
{
	Matrix4 mWorld = Matrix4::IDENTITY;
	Matrix4 ortho = g_App->GetCamera()->GetOrthoFromCamera();
	m_Shader->Render(g_App->GetDeviceContext(), m_VertexCount, 0, mWorld, ortho, m_Tex);
}

void Bitmap::UpdateBuffer()
{
	//(3) 0---4
	//    |\  |
	//    |  \|
	//    2---1(5)
	float left, top, right, bottom;
	left = m_ScreenSize.x / 2.f * -1.f + m_Position.x;
	right = left + m_ImageSize.x;
	top = m_ScreenSize.y / 2.f - m_Position.y;
	bottom = top - m_ImageSize.y;
	float z = g_App->GetCamera()->GetNearZ();
	m_Vertices[0].position = Vector3(left, top, z);
	m_Vertices[0].texture = Vector2(0.0f, 0.0f);
	m_Vertices[1].position = Vector3(right, bottom, z);
	m_Vertices[1].texture = Vector2(1.0f, 1.0f);
	m_Vertices[2].position = Vector3(left, bottom, z);
	m_Vertices[2].texture = Vector2(0.0f, 1.0f);
	m_Vertices[3].position = Vector3(left, top, z);
	m_Vertices[3].texture = Vector2(0.0f, 0.0f);
	m_Vertices[4].position = Vector3(right, top, z);
	m_Vertices[4].texture = Vector2(1.0f, 0.0f);
	m_Vertices[5].position = Vector3(right, bottom, z);
	m_Vertices[5].texture = Vector2(1.0f, 1.0f);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	g_App->GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VertexType* vertex = (VertexType*)(mappedResource.pData);
	for (int i = 0; i < m_VertexCount; ++i)
	{
		vertex[i] = m_Vertices[i];
	}
	memcpy(vertex, m_Vertices.data(), (sizeof(VertexType) * m_VertexCount));
	g_App->GetDeviceContext()->Unmap(m_VertexBuffer, 0);
}
