#pragma once
#include "OgreMatrix4.h"
#include "OgrePlane.h"
class Frustum
{
public:
	Frustum();
	Frustum(const Frustum&);
	~Frustum();

	void ConstructFrustum(float screenDepth, Ogre::Matrix4 projectionMatrix, Ogre::Matrix4 viewMatrix);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);

private:
	Ogre::Plane m_planes[6];
};