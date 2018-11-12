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
	virtual bool Init() = 0;
	virtual void Render() = 0;
protected:
	struct LightBufferType
	{
		Ogre::Vector4 Ambient;
		Ogre::Vector4 Diffuse;
		Ogre::Vector4 Specular;
		LightBufferType(Ogre::Vector4 a = Ogre::Vector4::ZERO, Ogre::Vector4 d = Ogre::Vector4::ZERO, Ogre::Vector4 s = Ogre::Vector4::ZERO)
			: Ambient(a)
			, Diffuse(d)
			, Specular(s)
		{}
	};
	ID3D11Buffer* m_LightBuffer;
};

class DirectionalLight : public Light
{	
public:
	struct DirLightBufferType : public LightBufferType
	{
		Ogre::Vector3 Direction;
		float SpecularPower;
		DirLightBufferType(Ogre::Vector4 a = Ogre::Vector4::ZERO, Ogre::Vector4 d = Ogre::Vector4::ZERO,
			Ogre::Vector4 s = Ogre::Vector4::ZERO, Ogre::Vector3 dir = Ogre::Vector3::ZERO, float spec = 0.0f)
			: LightBufferType(a, d, s)
			, Direction(dir)
			, SpecularPower(spec)
		{}
	};
	DirectionalLight() {};
	DirectionalLight(DirLightBufferType inData) : m_LightData(inData) { }
	virtual bool Init() override;
	virtual void Render() override;
protected:
	DirLightBufferType m_LightData;
	//float Pad; // Pad the last float so we can set an array of lights if we wanted.
};