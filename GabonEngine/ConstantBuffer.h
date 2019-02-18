#pragma once
#include "d3dUtil.h"
#include "MathHeader.h"
class ConstantBuffer
{
	struct FogBuffer
	{
		float  gFogStart;
		float  gFogRange;
		bool gFogEnable;
		float padding;
		Vector4 gFogColor;
	};
	struct ClipPlaneBuffer
	{
		Vector4 clipPlane;
	};
public:
	ConstantBuffer();
	~ConstantBuffer();
	void InitBuffer();
	void UpdateBuffer();
	ID3D11Buffer* m_FogBuffer;
	FogBuffer m_FogData;
	ID3D11Buffer* m_ClipPlaneBuffer;
	ClipPlaneBuffer m_ClipPlaneData;
};