#include "ModelObject.h"
#include "MainApp.h"

#include <fbxsdk.h>
#include <vector>


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
	ReleaseCOM(m_DiffuseSRV);
}

void ModelObject::Init(std::string name, TextureShader* shader, std::string fbxName)
{
	m_name = name;
	m_Shader = shader;
	LoadFBX(fbxName);
	BuildGeometryBuffers();
// 	if (name == "wall" || name == "floor" || name == "square")
// 	{
// 		BuildGeometryBuffers();
 		InitTexture(L"Textures/checkboard.dds");
// 	}
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
// 	m_VertexCount = 6;
 	m_StartVertexIndex = 0;
// 
// 	VertexType v[6];
// 	v[0] = VertexType(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
// 	v[1] = VertexType(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
// 	v[2] = VertexType(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
// 	v[3] = VertexType(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
// 	v[4] = VertexType(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
// 	v[5] = VertexType(1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	m_VertexCount = (int)m_Vertex.size();
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexType) * m_VertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &m_Vertex[0];
	HR(g_App->GetDevice()->CreateBuffer(&vbd, &vinitData, &m_vertexBuffer));
}

void ModelObject::InitTexture(LPCWSTR texName)
{
	ID3D11Resource* texture = nullptr;
	
	DirectX::CreateDDSTextureFromFile(g_App->GetDevice(), texName, &texture, &m_DiffuseSRV);
	ReleaseCOM(texture);
}

void ModelObject::LoadMesh(fbxsdk::FbxNode* pFbxRootNode)
{
	// temp store
	Ogre::Vector3 vertex[3];
	Ogre::Vector4 color[3];
	Ogre::Vector3 normal[3];
	Ogre::Vector3 tangent[3];
	Ogre::Vector2 uv[3][2];
	int vertexCounter = 0;
	if (pFbxRootNode)
	{
		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
		{
			fbxsdk::FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);
			std::string name = pFbxChildNode->GetName();
			LoadMesh(pFbxChildNode);			
			if (pFbxChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

			//FbxVector4* pVertices = pMesh->GetControlPoints();
			
			for (int j = 0; j < pMesh->GetPolygonCount(); j++)
			{
				int iNumVertices = pMesh->GetPolygonSize(j);
				assert(iNumVertices == 3);
				
				for (int k = 0; k < iNumVertices; k++) 
				{
					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);

					ReadVertex(pMesh, iControlPointIndex, &vertex);
					ReadColor(pMesh, iControlPointIndex, vertexCounter, &color);
					for (int layer = 0; layer < 2; ++layer)
					{

						ReadUV()
					}
// 					VertexType vertex(0, 0, 0, 0, 0, -1.0, 0.0, 0.0);
// 					vertex.position[0] = (float)pVertices[iControlPointIndex].mData[0];
// 					vertex.position[1] = (float)pVertices[iControlPointIndex].mData[1];
// 					vertex.position[2] = (float)pVertices[iControlPointIndex].mData[2];
// 					
// 					m_Vertex.push_back(vertex);
				}
			}

		}

	}
}

int ModelObject::GetVertexCount()
{
	return m_VertexCount;
}

int ModelObject::GetStartVertexIndex()
{
	return m_StartVertexIndex;
};

FbxManager* g_pFbxSdkManager = nullptr;

bool ModelObject::LoadFBX(std::string fileName)
{
	if (g_pFbxSdkManager == nullptr)
	{
		g_pFbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_pFbxSdkManager, IOSROOT);
		g_pFbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(g_pFbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(g_pFbxSdkManager, "");

	bool bSuccess = pImporter->Initialize(fileName.c_str()/*"C:\\MyPath\\MyModel.fbx"*/, -1, g_pFbxSdkManager->GetIOSettings());
	if (!bSuccess) return false;

	bSuccess = pImporter->Import(pFbxScene);
	if (!bSuccess) return false;

	pImporter->Destroy();

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
	LoadMesh(pFbxRootNode);
	
	return true;
}