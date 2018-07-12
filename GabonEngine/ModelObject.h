#pragma once
#include "Object.h"
#include "Material.h"
#include "d3dUtil.h"
#include "Math/OgreMatrix4.h"
class TextureShader;
class ModelObject :
	public Object
{
public:
	ModelObject();
	~ModelObject();

	void Render();
private:
	TextureShader* m_Shader;
	std::string m_name;
	
	Material* m_mat;
	Ogre::Matrix4 m_World;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11ShaderResourceView* m_DiffuseSRV;
};

