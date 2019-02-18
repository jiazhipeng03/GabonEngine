#include "ConstantBuffer.h"
#include "MainApp.h"
ConstantBuffer::ConstantBuffer()
	:m_FogBuffer(NULL)
	, m_ClipPlaneBuffer(NULL)
{

}

ConstantBuffer::~ConstantBuffer()
{
	SafeRelease(m_FogBuffer);
	SafeRelease(m_ClipPlaneBuffer);
}

void ConstantBuffer::InitBuffer()
{
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(FogBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	m_FogData.gFogColor = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
	m_FogData.gFogEnable = true;
	m_FogData.gFogStart = 2.f;
	m_FogData.gFogRange = 40.0f;
	D3D11_SUBRESOURCE_DATA bufData;
	bufData.pSysMem = &m_FogData;
	bufData.SysMemPitch = 0;
	bufData.SysMemSlicePitch = 0;
	HRESULT hr = g_App->GetDevice()->CreateBuffer(&bd, &bufData, &m_FogBuffer);
	if (FAILED(hr))
	{
		return;
	}

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(FogBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	// xyz指示方向，w指示标量
	// y轴以上的都被clip
	// dot(pos, clip)
	m_ClipPlaneData.clipPlane = Vector4(0.0f, -1.0f, 0.0f, 0.0f);
	
	bufData.pSysMem = &m_ClipPlaneData;
	bufData.SysMemPitch = 0;
	bufData.SysMemSlicePitch = 0;
	hr = g_App->GetDevice()->CreateBuffer(&bd, &bufData, &m_ClipPlaneBuffer);
	if (FAILED(hr))
	{
		return;
	}
}

void ConstantBuffer::UpdateBuffer()
{
	// Fog buffer
	g_App->GetDeviceContext()->PSSetConstantBuffers(1, 1, &m_FogBuffer);

	// Clip plane buffer
	g_App->GetDeviceContext()->VSSetConstantBuffers(2, 1, &m_ClipPlaneBuffer);
}

