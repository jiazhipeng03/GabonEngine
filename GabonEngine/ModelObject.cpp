#include "ModelObject.h"
#include "MainApp.h"


ModelObject::ModelObject()
	: m_indexBuffer(NULL)
	, m_vertexBuffer(nullptr)
	, m_Shader(NULL)
	, m_name("")
	, m_mat(nullptr)
	, m_World(Ogre::Matrix4::IDENTITY)
	, m_DiffuseSRV(NULL)
	, m_VertexCount(0)
	, m_StartVertexIndex(0)
{
}


ModelObject::~ModelObject()
{
	SafeDelete(m_DiffuseSRV);
}

void ModelObject::Init(std::string name, TextureShader* shader)
{
	m_name = name;
	m_Shader = shader;
	if (name == "wall" || name == "floor" || name == "square")
	{
		BuildGeometryBuffers();
		InitTexture(L"Textures/checkboard.dds");
	}
}

void ModelObject::Render()
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	//m_Shader->GetVertexStride();
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	g_App->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// no ib
// 	// Set the index buffer to active in the input assembler so it can be rendered.
// 	g_App->GetDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	g_App->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	return;
}

int ModelObject::GetIndexCount()
{
	return m_IndexCount;
}

Ogre::Matrix4 ModelObject::GetWorldMatrix()
{
	return Ogre::Matrix4::IDENTITY;
}

void ModelObject::BuildGeometryBuffers()
{
	m_VertexCount = 6;
	m_StartVertexIndex = 0;

	VertexType v[6];
	v[0] = VertexType(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[1] = VertexType(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[2] = VertexType(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[3] = VertexType(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = VertexType(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = VertexType(1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexType) * 6;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &v[0];
	HR(g_App->GetDevice()->CreateBuffer(&vbd, &vinitData, &m_vertexBuffer));
}

void ModelObject::InitTexture(LPCWSTR texName)
{
	ID3D11Resource* texture = nullptr;
	DirectX::CreateDDSTextureFromFile(g_App->GetDevice(),
		texName, &texture, &m_DiffuseSRV);
}

int ModelObject::GetVertexCount()
{
	return m_VertexCount;
}

int ModelObject::GetStartVertexIndex()
{
	return m_StartVertexIndex;
}
