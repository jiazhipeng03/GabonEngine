#pragma once
#include "Object.h"
#include "Material.h"
#include "d3dUtil.h"
class ModelObject :
	public Object
{
public:
	ModelObject();
	~ModelObject();

private:
	std::string m_name;
	
	Material* m_mat;
	DirectX::XMFLOAT4X4 m_World;

	ID3D11Buffer* m_vb;
	ID3D11ShaderResourceView* m_DiffuseSRV;
};

