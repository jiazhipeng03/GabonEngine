#pragma once
#include "Object.h"
#include "Material.h"
#include "d3dUtil.h"
#include "MathHeader.h"
class TextureShader;
namespace fbxsdk
{
	class FbxNode;
}
class ModelObject :
	public Object
{
	// should be defined by vertex shader
	// 定义全部属性，解析文件存储到该类型data中，渲染时根据vs所需格式重新组织传入vertex buffer
	struct VertexType
	{
		Ogre::Vector3 position;
		Ogre::Vector3 normal;
		Ogre::Vector2 texture;
		Ogre::Vector3 tangent;
		Ogre::Vector3 binormal;
		VertexType() : position(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), texture(0.0f, 0.0f) {}
		VertexType(float px, float py, float pz, float nx, float ny, float nz, float u, float v)
			: position(px, py, pz), normal(nx, ny, nz), texture(u, v) {}
	};
	enum VertexInputType
	{
		VI_POSITION,
		VI_NORMAL,
		VI_TEXTURE,
	};
	const static std::vector<std::string> s_VertexInputNames;
// 	struct VertexType
// 	{
// 		Vector3 position;
// 		Vector2 texture;
// 	};
public:
	ModelObject();
	~ModelObject();

	virtual void Init(std::string name, TextureShader* shader, std::string meshName, std::vector<std::string> texNames);
	void SetPosition(Vector3 pos) { m_World.setTrans(pos); }
	bool LoadGeometryBuffers(std::string meshName);
	virtual void Render();
	
	Ogre::Matrix4 GetWorldMatrix();
	std::vector<ID3D11ShaderResourceView*> GetTexArray() { return m_TexArray; }
	TextureShader* GetShader() { return m_Shader; }
	
	int GetVertexCount();
	int GetIndexCount();
	int GetStartVertexIndex();
	//视锥剔除，需要查找快速的算法，简单方法就是根据vertexPosition找到最大最小值生成Cube
	//Cube GetBound();
	std::string GetName() { return m_name; }
	void SetWorldMatrix(Matrix4 world) { m_World = world; }
private:
	// build vertex/index buffers
	void BuildGeometryBuffers();
	// render data
	void RebuildVertexData(void*& OutDataBuffer, int& OutDataSize);
	// Load
	bool LoadMeshFromFBX(const std::string& file);
	bool LoadMeshFromTxt(const std::string& file);
	void LoadFbxMesh(fbxsdk::FbxNode* pFbxRootNode);
	// TBN calculate
	void CalculateModelVectors();
	void CalculateTangentBinormal(VertexType v1, VertexType v2, VertexType v3, Vector3& OutTangent, Vector3& OutBinormal);
	void CalculateNormal(Vector3 tangent, Vector3 binormal, Vector3& OutNormal);
	
protected:
	TextureShader* m_Shader;
	std::string m_name;
	
	Material* m_mat;
	Ogre::Matrix4 m_World;

	std::vector<VertexType> m_Vertex;
	//std::vector<unsigned long> m_Indices;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	std::vector<ID3D11ShaderResourceView*> m_TexArray;
	int m_VertexCount;
	int m_IndexCount;
	int m_StartVertexIndex;
};

