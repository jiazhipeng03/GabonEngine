#pragma once
#include <windows.h>
#include "Object.h"
#include "Math/OgreVector4.h"
class Light :
	public Object
{
public:
	Light();
	~Light();
protected:
	Ogre::Vector4 Ambient;
	Ogre::Vector4 Diffuse;
	Ogre::Vector4 Specular;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

protected:
	DirectX::XMFLOAT3 Direction;
	//float Pad; // Pad the last float so we can set an array of lights if we wanted.
};