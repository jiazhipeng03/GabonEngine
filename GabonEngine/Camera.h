//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Simple first person style camera class that lets the viewer explore the 3D scene.
//   -It keeps track of the camera coordinate system relative to the world space
//    so that the view matrix can be constructed.  
//   -It keeps track of the viewing frustum of the camera so that the projection
//    matrix can be obtained.
//***************************************************************************************

#ifndef CAMERA_H
#define CAMERA_H

#include "d3dUtil.h"
#include "Math/OgreMatrix4.h"
class Camera
{
public:
	Camera();
	~Camera();

	// Get/Set world camera position.
	Ogre::Vector3 GetPositionXM()const;
	Ogre::Vector3 GetPosition()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const Ogre::Vector3& v);
	
	// Get camera basis vectors.
	Ogre::Vector3 GetRightXM()const;
	Ogre::Vector3 GetRight()const;
	Ogre::Vector3 GetUpXM()const;
	Ogre::Vector3 GetUp()const;
	Ogre::Vector3 GetLookXM()const;
	Ogre::Vector3 GetLook()const;

	// Get frustum properties.
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;
	
	// Set frustum.
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const Ogre::Vector3& pos, const Ogre::Vector3& target, const Ogre::Vector3& up);

	// Get View/Proj matrices.
	Ogre::Matrix4 View()const;
	Ogre::Matrix4 Proj()const;
	Ogre::Matrix4 ViewProj()const;

	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

private:

	// Camera coordinate system with coordinates relative to world space.
	Ogre::Vector3 mPosition;
	Ogre::Vector3 mRight;
	Ogre::Vector3 mUp;
	Ogre::Vector3 mLook;

	// Cache frustum properties.
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	// Cache View/Proj matrices.
	Ogre::Matrix4 mView;
	Ogre::Matrix4 mProj;
};

#endif // CAMERA_H