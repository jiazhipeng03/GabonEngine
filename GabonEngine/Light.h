#pragma once
#include <windows.h>
#include "Object.h"
class Light :
	public Object
{
public:
	Light();
	~Light();
protected:
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

protected:
	DirectX::XMFLOAT3 Direction;
	//float Pad; // Pad the last float so we can set an array of lights if we wanted.
};