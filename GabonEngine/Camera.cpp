//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Camera.h"
#include "MathHelper.h"
using namespace DirectX;
Camera::Camera()
	: mPosition(0.0f, 0.0f, 0.0f), 
	  mRight(1.0f, 0.0f, 0.0f),
	  mUp(0.0f, 1.0f, 0.0f),
	  mLook(0.0f, 0.0f, 1.0f)
{	
	SetLens(0.25f*MathHelper::Pi, 1.0f, 1.0f, 1000.0f);
	XMVECTOR pos = XMVectorSet(0, 0, 1, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	LookAt(pos, target, up);
	UpdateViewMatrix();
}

Camera::~Camera()
{
}

Ogre::Vector3 Camera::GetPositionXM()const
{
	return mPosition;
	//return XMLoadFloat3(&mPosition);
}

Ogre::Vector3 Camera::GetPosition()const
{
	return mPosition;
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(const Ogre::Vector3& v)
{
	mPosition = v;
}

Ogre::Vector3 Camera::GetRightXM()const
{
	return XMLoadFloat3(&mRight);
}

Ogre::Vector3 Camera::GetRight()const
{
	return mRight;
}

Ogre::Vector3 Camera::GetUpXM()const
{
	return XMLoadFloat3(&mUp);
}

Ogre::Vector3 Camera::GetUp()const
{
	return mUp;
}

Ogre::Vector3 Camera::GetLookXM()const
{
	return XMLoadFloat3(&mLook);
}

Ogre::Vector3 Camera::GetLook()const
{
	return mLook;
}

float Camera::GetNearZ()const
{
	return mNearZ;
}

float Camera::GetFarZ()const
{
	return mFarZ;
}

float Camera::GetAspect()const
{
	return mAspect;
}

float Camera::GetFovY()const
{
	return mFovY;
}

float Camera::GetFovX()const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / mNearZ);
}

float Camera::GetNearWindowWidth()const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight()const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth()const
{
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight()const
{
	return mFarWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf( 0.5f*mFovY );
	mFarWindowHeight  = 2.0f * mFarZ * tanf( 0.5f*mFovY );

	mProj = Ogre::Matrix4::ZERO;
	mProj[0][0] = 1 / (tan(fovY * 0.5f) *aspect);
	mProj[1][1] = 1 / tan(fovY * 0.5f);
	mProj[2][2] = zf / (zf - zn);
	mProj[2][3] = 1.0f;
	mProj[3][2] = (zn * zf) / (zn - zf);
// 	Ogre::Matrix4 P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
// 	XMStoreFloat4x4(&mProj, P);
}

void Camera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp)
{
// 	Ogre::Vector3 L = XMVector3Normalize(XMVectorSubtract(target, pos));
// 	Ogre::Vector3 R = XMVector3Normalize(XMVector3Cross(worldUp, L));
// 	Ogre::Vector3 U = XMVector3Cross(L, R);
// 
// 	XMStoreFloat3(&mPosition, pos);
// 	XMStoreFloat3(&mLook, L);
// 	XMStoreFloat3(&mRight, R);
// 	XMStoreFloat3(&mUp, U);
}

void Camera::LookAt(const Ogre::Vector3& pos, const Ogre::Vector3& target, const Ogre::Vector3& up)
{
// 	Ogre::Vector3 P = XMLoadFloat3(&pos);
// 	Ogre::Vector3 T = XMLoadFloat3(&target);
// 	Ogre::Vector3 U = XMLoadFloat3(&up);
// 
// 	LookAt(P, T, U);
}

Ogre::Matrix4 Camera::View()const
{
	return mView;
}

Ogre::Matrix4 Camera::Proj()const
{
	return mProj;
}

Ogre::Matrix4 Camera::ViewProj()const
{
	return mView * mProj;
}

void Camera::Strafe(float d)
{
	// mPosition += d*mRight
	Ogre::Vector3 s = XMVectorReplicate(d);
	Ogre::Vector3 r = XMLoadFloat3(&mRight);
	Ogre::Vector3 p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));
}

void Camera::Walk(float d)
{
// 	// mPosition += d*mLook
// 	XMVECTOR s = XMVectorReplicate(d);
// 	XMVECTOR l = XMLoadFloat3(&mLook);
// 	XMVECTOR p = XMLoadFloat3(&mPosition);
// 	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
}

void Camera::Pitch(float angle)
{
// 	// Rotate up and look vector about the right vector.
// 
// 	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);
// 
// 	XMStoreFloat3(&mUp,   XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
// 	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
}

void Camera::RotateY(float angle)
{
// 	// Rotate the basis vectors about the world y-axis.
// 
// 	XMMATRIX R = XMMatrixRotationY(angle);
// 
// 	XMStoreFloat3(&mRight,   XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
// 	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
// 	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
}

void Camera::UpdateViewMatrix()
{
	using namespace Ogre;
	Vector3 z((mPosition - mLook)
// 	XMVECTOR R = XMLoadFloat3(&mRight);
// 	XMVECTOR U = XMLoadFloat3(&mUp);
// 	XMVECTOR L = XMLoadFloat3(&mLook);
// 	XMVECTOR P = XMLoadFloat3(&mPosition);
// 
// 	// Keep camera's axes orthogonal to each other and of unit length.
// 	L = XMVector3Normalize(L);
// 	U = XMVector3Normalize(XMVector3Cross(L, R));
// 
// 	// U, L already ortho-normal, so no need to normalize cross product.
// 	R = XMVector3Cross(U, L); 
// 
// 	// Fill in the view matrix entries.
// 	float x = -XMVectorGetX(XMVector3Dot(P, R));
// 	float y = -XMVectorGetX(XMVector3Dot(P, U));
// 	float z = -XMVectorGetX(XMVector3Dot(P, L));
// 
// 	XMStoreFloat3(&mRight, R);
// 	XMStoreFloat3(&mUp, U);
// 	XMStoreFloat3(&mLook, L);
// 
// 	mView[0][0] = mRight.x; 
// 	mView[1][0] = mRight.y; 
// 	mView[2][0] = mRight.z; 
// 	mView[3][0] = x;   
// 
// 	mView[0][1] = mUp.x;
// 	mView[1][1] = mUp.y;
// 	mView[2][1] = mUp.z;
// 	mView[3][1] = y;  
// 
// 	mView[0][2] = mLook.x; 
// 	mView[1][2] = mLook.y; 
// 	mView[2][2] = mLook.z; 
// 	mView[3][2] = z;   
// 
// 	mView[0][3] = 0.0f;
// 	mView[1][3] = 0.0f;
// 	mView[2][3] = 0.0f;
// 	mView[3][3] = 1.0f;
}


