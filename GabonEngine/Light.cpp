#include "Light.h"
#include "MainApp.h"

Light::Light()
{
}


Light::~Light()
{
	SafeRelease(m_LightBuffer);
}

bool DirectionalLight::Init()
{
	D3D11_BUFFER_DESC bd;
	bd.ByteWidth = sizeof(DirLightBufferType);
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	HRESULT hr = g_App->GetDevice()->CreateBuffer(&bd, NULL, &m_LightBuffer);
	if (FAILED(hr))
	{
		return false;
	}
		
	m_LightData.Ambient = Ogre::Vector4(Vector3(0.5f), 1.0f);
	m_LightData.Diffuse = Ogre::Vector4(Vector3(0.4f), 1.0f);
	m_LightData.Specular = Ogre::Vector4(Vector3(0.7f), 1.0f);
	m_LightData.Direction = Ogre::Vector3(1,-1,1);
	m_LightData.SpecularPower = 0.5f;
	return true;
}

void DirectionalLight::Render()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = g_App->GetDeviceContext()->Map(m_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}
	DirLightBufferType* light = (DirLightBufferType*)(mappedResource.pData);
//	light->Ambient = Ogre::Vector4::ZERO;
	*light = m_LightData;
//	light->Specular = Ogre::Vector4::ZERO;
	g_App->GetDeviceContext()->Unmap(m_LightBuffer, 0);
	g_App->GetDeviceContext()->PSSetConstantBuffers(0, 1, &m_LightBuffer);
}
