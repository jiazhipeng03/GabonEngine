#include "ModelObject.h"
#include "MainApp.h"

#include <fbxsdk.h>

const std::vector<std::string> ModelObject::s_VertexInputNames = {"position", "normal", "texture"};
ModelObject::ModelObject()
	: m_indexBuffer(NULL)
	, m_vertexBuffer(nullptr)
	, m_Shader(NULL)
	, m_name("")
	, m_mat(nullptr)
	, m_World(Ogre::Matrix4::IDENTITY)
	//, m_DiffuseSRV(NULL)
	, m_VertexCount(0)
	, m_StartVertexIndex(0)
{
}


ModelObject::~ModelObject()
{
	for(auto tex : m_TexArray)
		SafeRelease(tex);
	m_TexArray.clear();
}

bool ModelObject::LoadGeometryBuffers(std::string meshName)
{
	if (meshName.find(".fbx") != std::string::npos)
	{
		return LoadMeshFromFBX(meshName);
	}
	else if (meshName.find(".txt") != std::string::npos)
	{
		return LoadMeshFromTxt(meshName);
	}
	return false;
}

void ModelObject::Init(std::string name, TextureShader* shader, std::string meshName, std::vector<std::string> texNames)
{
	m_name = name;
	m_Shader = shader;
	
	LoadGeometryBuffers(meshName);
	CalculateModelVectors();
	BuildGeometryBuffers();
	m_TexArray.resize(texNames.size(), NULL);
	for(int i=0; i<(int)texNames.size(); ++i)
		D3DHelper::InitTexture(g_App->GetDevice(), texNames[i], m_TexArray[i]);
}

void ModelObject::Render()
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	//m_Shader->GetVertexStride();
	stride = m_Shader->GetVertexStride();// sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	g_App->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// no ib
// 	// Set the index buffer to active in the input assembler so it can be rendered.
 	g_App->GetDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

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
	return m_World;
}

void ModelObject::BuildGeometryBuffers()
{
	m_StartVertexIndex = 0;
// 	m_VertexCount = 6; 	
//  VertexType v[6];
// 	v[0] = VertexType(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
// 	v[1] = VertexType(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
// 	v[2] = VertexType(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
// 	v[3] = VertexType(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
// 	v[4] = VertexType(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
// 	v[5] = VertexType(1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
// 	m_Vertex.assign(&v[0], &v[5]);
	unsigned long* m_Indices = new unsigned long[m_IndexCount];
	for (int i = 0; i < m_VertexCount; ++i)
	{
		m_Indices[i] = i;
	}
	m_VertexCount = (int)m_Vertex.size();

	void* data;
	int size;
	RebuildVertexData(data, size);
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = size;// sizeof(VertexType) * m_VertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = data;// m_Vertex.data();
	vinitData.SysMemPitch = 0;
	vinitData.SysMemSlicePitch = 0;
	HRESULT hr = (g_App->GetDevice()->CreateBuffer(&vbd, &vinitData, &m_vertexBuffer));
	if (FAILED(hr))
	{
		return;
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(unsigned long) * m_IndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = m_Indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	HR(g_App->GetDevice()->CreateBuffer(&ibd, &indexData, &m_indexBuffer));

	delete[] m_Indices;
	free(data);
}

void ModelObject::RebuildVertexData(void*& OutDataBuffer, int& OutDataSize)
{
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout = m_Shader->GetVertexLayoutFromShader();
	std::vector<VertexInputType> outLayout;
	ui32 index = 0;
// 	for (auto ele : layout)
// 	{
// 		std::string name = ele.SemanticName;
// 		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
// 		// index 不用重置，找到后总是从当前继续找
// 		for (; index < s_VertexInputNames.size(); ++index)
// 		{
// 			if (s_VertexInputNames[index] == name)
// 				break;
// 		}
// 		assert(index == s_VertexInputNames.size());
// 		outLayout.push_back((VertexInputType)index);
// 	}
	ui32 vertexStride = layout.back().AlignedByteOffset + TextureShader::GetStride(layout.back().Format);
	OutDataSize = vertexStride * m_Vertex.size();
	OutDataBuffer = malloc(sizeof(BYTE) * OutDataSize);
	// 判断layout中需要输出的属性
	BYTE* pDest = (BYTE*)OutDataBuffer;
	for (auto vertexInput : layout)
	{
		ui32 offset = vertexInput.AlignedByteOffset;
		ui32 stride = TextureShader::GetStride(vertexInput.Format);
		for (int i = 0; i < m_VertexCount; ++i)
		{
			pDest = (BYTE*)OutDataBuffer + i * vertexStride + offset;
			if (strcmp(vertexInput.SemanticName, "POSITION") == 0)
			{
				memcpy_s(pDest, stride, &m_Vertex[i].position, stride);
			}
			else if(strcmp(vertexInput.SemanticName, "NORMAL") == 0)
			{
				memcpy_s(pDest, stride, &m_Vertex[i].normal, stride);
			}
			else if(strcmp(vertexInput.SemanticName, "TEXCOORD") == 0)
			{
				memcpy_s(pDest, stride, &m_Vertex[i].texture, stride);
			}
			else if (strcmp(vertexInput.SemanticName, "TANGENT") == 0)
			{
				memcpy_s(pDest, stride, &m_Vertex[i].tangent, stride);
			}
			else if (strcmp(vertexInput.SemanticName, "BINORMAL") == 0)
			{
				memcpy_s(pDest, stride, &m_Vertex[i].binormal, stride);
			}
			else if (strcmp(vertexInput.SemanticName, "NORMAL") == 0)
			{
				memcpy_s(pDest, stride, &m_Vertex[i].normal, stride);
			}
			else
			{
				memset(pDest, 0, stride);
				//assert(0);
			}
		}
			
	}
}

void ModelObject::LoadFbxMesh(fbxsdk::FbxNode* pFbxRootNode)
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
			LoadFbxMesh(pFbxChildNode);			
			if (pFbxChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

			FbxVector4* pVertices = pMesh->GetControlPoints();
			
			for (int j = 0; j < pMesh->GetPolygonCount(); j++)
			{
				int iNumVertices = pMesh->GetPolygonSize(j);
				assert(iNumVertices == 3);
				
				for (int k = 0; k < iNumVertices; k++) 
				{
					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);

// 					ReadVertex(pMesh, iControlPointIndex, &vertex);
// 					ReadColor(pMesh, iControlPointIndex, vertexCounter, &color);
// 					for (int layer = 0; layer < 2; ++layer)
// 					{
// 
// 						ReadUV()
// 					}
					VertexType vertex(0, 0, 0, 0, 0, -1.0, 0.0, 0.0);
					vertex.position[0] = (float)pVertices[iControlPointIndex].mData[0];
					vertex.position[1] = (float)pVertices[iControlPointIndex].mData[1];
					vertex.position[2] = (float)pVertices[iControlPointIndex].mData[2];
					
					m_Vertex.push_back(vertex);
				}
			}

		}

	}
}

void ModelObject::CalculateModelVectors()
{
	int faceCount, i, index;
	VertexType vertex1, vertex2, vertex3;
	Vector3 tangent, binormal, normal;


	// Calculate the number of faces in the model.
	faceCount = m_VertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.position = m_Vertex[index].position;
		vertex1.texture = m_Vertex[index].texture;
		vertex1.normal = m_Vertex[index].normal;

		index++;

		vertex2.position = m_Vertex[index].position;
		vertex2.texture = m_Vertex[index].texture;
		vertex2.normal = m_Vertex[index].normal;
		index++;

		vertex3.position = m_Vertex[index].position;
		vertex3.texture = m_Vertex[index].texture;
		vertex3.normal = m_Vertex[index].normal;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		for (int i = 1; i <= 3; ++i)
		{
			m_Vertex[index - i].normal = normal;
			m_Vertex[index - i].tangent = tangent;
			m_Vertex[index - i].binormal = binormal;
		}		
// 		m_model[index - 1].ny = normal.y;
// 		m_model[index - 1].nz = normal.z;
// 		m_model[index - 1].tx = tangent.x;
// 		m_model[index - 1].ty = tangent.y;
// 		m_model[index - 1].tz = tangent.z;
// 		m_model[index - 1].bx = binormal.x;
// 		m_model[index - 1].by = binormal.y;
// 		m_model[index - 1].bz = binormal.z;

// 		m_model[index - 2].nx = normal.x;
// 		m_model[index - 2].ny = normal.y;
// 		m_model[index - 2].nz = normal.z;
// 		m_model[index - 2].tx = tangent.x;
// 		m_model[index - 2].ty = tangent.y;
// 		m_model[index - 2].tz = tangent.z;
// 		m_model[index - 2].bx = binormal.x;
// 		m_model[index - 2].by = binormal.y;
// 		m_model[index - 2].bz = binormal.z;
// 
// 		m_model[index - 3].nx = normal.x;
// 		m_model[index - 3].ny = normal.y;
// 		m_model[index - 3].nz = normal.z;
// 		m_model[index - 3].tx = tangent.x;
// 		m_model[index - 3].ty = tangent.y;
// 		m_model[index - 3].tz = tangent.z;
// 		m_model[index - 3].bx = binormal.x;
// 		m_model[index - 3].by = binormal.y;
// 		m_model[index - 3].bz = binormal.z;
	}

	return;
}

void ModelObject::CalculateTangentBinormal(VertexType v1, VertexType v2, VertexType v3, Vector3& OutTangent, Vector3& OutBinormal)
{
	Vector3 vec1, vec2;
	Vector2 texVec1, texVec2;
// 	float vector1[3], vector2[3];
// 	float tuVector[2], tvVector[2];
 	float den;

	// Calculate the two vectors for this face.
	vec1 = v2.position - v1.position;
// 	vector1[0] = vertex2.x - v1.position.x;
// 	vector1[1] = vertex2.y - vertex1.y;
// 	vector1[2] = vertex2.z - vertex1.z;

	vec2 = v3.position - v1.position;
// 	vector2[1] = vertex3.y - vertex1.y;
// 	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	
	texVec1 = v2.texture - v1.texture;
// 	tuVector[0] = vertex2.tu - vertex1.tu;
// 	tvVector[0] = vertex2.tv - vertex1.tv;

	texVec2 = v3.texture - v1.texture;
// 	tuVector[1] = vertex3.tu - vertex1.tu;
// 	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (texVec1.x * texVec2.y - texVec1.y * texVec2.x);
	//den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	OutTangent = (texVec2.y * vec1 - texVec1.y * vec2) * den;
// 	OutTangent.x = (texVec2.y * vec1.x - texVec1.y * vec2.x) * den;
// 	OutTangent.y = (texVec2.y * vec1.y - texVec1.y * vec2.y) * den;
// 	OutTangent.z = (texVec2.y * vec1.z - texVec1.y * vec2.z) * den;

	OutBinormal = (texVec1.x * vec2 - texVec2.x * vec1) * den;
// 	OutBinormal.x = (texVec1.x * vec2.x - texVec2.x * vec1.x) * den;
// 	OutBinormal.y = (texVec1.x * vec2.y - texVec2.x * vec1.y) * den;
// 	OutBinormal.z = (texVec1.x * vec2.z - texVec2.x * vec1.z) * den;

	// Calculate the length of this normal.
	//length = OutTangent.length();
	//length = sqrt((OutTangent.x * OutTangent.x) + (OutTangent.y * OutTangent.y) + (OutTangent.z * OutTangent.z));

	// Normalize the normal and then store it
	OutTangent.normalise();
// 	tangent.x = tangent.x / length;
// 	tangent.y = tangent.y / length;
// 	tangent.z = tangent.z / length;

// 	// Calculate the length of this normal.
// 	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));
// 
// 	// Normalize the normal and then store it
// 	binormal.x = binormal.x / length;
// 	binormal.y = binormal.y / length;
// 	binormal.z = binormal.z / length;
	OutBinormal.normalise();
	return;
}

void ModelObject::CalculateNormal(Vector3 tangent, Vector3 binormal, Vector3& OutNormal)
{
	//float length;

	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	OutNormal = tangent.crossProduct(binormal);
	OutNormal.normalise();
// 	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
// 	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
// 	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);
// 
// 	// Calculate the length of the normal.
// 	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
// 
// 	// Normalize the normal.
// 	normal.x = normal.x / length;
// 	normal.y = normal.y / length;
// 	normal.z = normal.z / length;

	return;
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

bool ModelObject::LoadMeshFromFBX(const std::string& fileName)
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
	LoadFbxMesh(pFbxRootNode);
	
	return true;
}

bool ModelObject::LoadMeshFromTxt(const std::string& file)
{
	std::ifstream fin;
	char input;
	int i;

	// Open the model file.
	fin.open(file);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_VertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_IndexCount = m_VertexCount;

	// Create the model using the vertex count that was read in.
// 	m_model = new ModelType[m_vertexCount];
// 	if (!m_model)
// 	{
// 		return false;
// 	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < m_VertexCount; i++)
	{
		VertexType v;
		fin >> v.position.x >> v.position.y >> v.position.z;
		fin >> v.texture.x >> v.texture.y;
		fin >> v.normal.x >> v.normal.y >> v.normal.z;
		m_Vertex.push_back(v);
	}

	// Close the model file.
	fin.close();

	return true;
}
