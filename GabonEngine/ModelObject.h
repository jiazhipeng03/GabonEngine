#pragma once
#include "Object.h"
#include "Material.h"
#include "d3dUtil.h"
#include "Math/OgreMatrix4.h"
class TextureShader;
namespace fbxsdk
{
	class FbxNode;
}
class ModelObject :
	public Object
{
	// should be defined by vertex shader
	struct VertexType
	{
		Ogre::Vector3 position;
		Ogre::Vector3 normal;
		Ogre::Vector2 texture;
		VertexType() : position(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), texture(0.0f, 0.0f) {}
		VertexType(float px, float py, float pz, float nx, float ny, float nz, float u, float v)
			: position(px, py, pz), normal(nx, ny, nz), texture(u, v) {}
	};
public:
	ModelObject();
	~ModelObject();

	virtual void Init(std::string name, TextureShader* shader, std::string fbxName);
	bool LoadGeometryBuffers(std::string meshName);
	void Render();
	
	Ogre::Matrix4 GetWorldMatrix();
	ID3D11ShaderResourceView* GetDiffuseSRV() { return m_DiffuseSRV; }
	TextureShader* GetShader() { return m_Shader; }
	
	int GetVertexCount();
	int GetIndexCount();
	int GetStartVertexIndex();
	//视锥剔除，需要查找快速的算法，简单方法就是根据vertexPosition找到最大最小值生成Cube
	//Cube GetConvex();
private:
	void BuildGeometryBuffers();
	void InitTexture(LPCWSTR texName);
	bool LoadMeshFromFBX(const std::string& file);
	bool LoadMeshFromTxt(const std::string& file);
	void LoadFbxMesh(fbxsdk::FbxNode* pFbxRootNode);
private:
	TextureShader* m_Shader;
	std::string m_name;
	
	Material* m_mat;
	Ogre::Matrix4 m_World;

	std::vector<VertexType> m_Vertex;
	//std::vector<unsigned long> m_Indices;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11ShaderResourceView* m_DiffuseSRV;
	int m_VertexCount;
	int m_IndexCount;
	int m_StartVertexIndex;
};

