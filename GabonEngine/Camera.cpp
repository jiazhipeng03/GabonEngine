//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Camera.h"
#include "MathHelper.h"
#include "MainApp.h"
using namespace DirectX;
Camera::Camera()
	: mPosition(0.0f, 0.0f, 0.0f), 
	  mRight(1.0f, 0.0f, 0.0f),
	  mUp(0.0f, 1.0f, 0.0f),
	  mLook(0.0f, 0.0f, 1.0f)
{	
	SetLens(0.25f*MathHelper::Pi, 1.0f, 1.0f, 1000.0f);
// 	XMVECTOR pos = XMVectorSet(0, 0, 1, 1.0f);
// 	XMVECTOR target = XMVectorZero();
// 	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Ogre::Vector3 pos(0, 0, 1), target(0,0,0), up(0,1,0);
	LookAt(pos, target, up);
	UpdateViewMatrix();
}

Camera::~Camera()
{
}

void Camera::InitBuffer()
{
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(CameraBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	HRESULT hr = g_App->GetDevice()->CreateBuffer(&bd, nullptr, &m_CameraBuffer);
	if (FAILED(hr))
	{
		return;
	}
}

void Camera::UpdateBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = g_App->GetDeviceContext()->Map(m_CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}
	CameraBuffer* buf = (CameraBuffer*)(mappedResource.pData);
	buf->eyePosition = mPosition;
	g_App->GetDeviceContext()->Unmap(m_CameraBuffer, 0);
	g_App->GetDeviceContext()->VSSetConstantBuffers(1, 1, &m_CameraBuffer);
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
	mPosition = Ogre::Vector3(x, y, z);
}

void Camera::SetPosition(const Ogre::Vector3& v)
{
	mPosition = v;
}

Ogre::Vector3 Camera::GetRight()const
{
	return mRight;
}


Ogre::Vector3 Camera::GetUp()const
{
	return mUp;
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

void Camera::LookAt(const Ogre::Vector3& pos, const Ogre::Vector3& target, const Ogre::Vector3& up)
{
	mPosition = pos;
	mLook = (target - pos).normalisedCopy();
	mRight = (up.crossProduct(mLook)).normalisedCopy();
	mUp = up;
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

Ogre::Matrix4 Camera::GetOrthoFromCamera()
{
	float w = g_App->GetScreenSize().x;
	float h = g_App->GetScreenSize().y;
	return OrthoLH(w, h, GetNearZ(), GetFarZ());
}

Ogre::Matrix4 Camera::OrthoLH(float w, float h, float zn, float zf)
{
	float halfW = w / 2.0f;
	float halfH = h / 2.0f;
	return OrthoOffCenterLH(-halfW, halfW, -halfH, halfH, zn, zf);
}

Ogre::Matrix4 Camera::OrthoOffCenterLH(float l, float r, float t, float b, float n, float f)
{
	float w = 1.f / (r - l);
	float h = 1.f / (t - b);
	float q = 1.f / (n - f);
	Matrix4 m;
	m[0][0] = 2.f * w;	m[0][1] = 0.0f;		m[0][2] = 0.0f;		m[0][3] = 0.0f;
	m[1][0] = 0.0f;		m[1][1] = 2.f * h;	m[1][2] = 0.0f;		m[1][3] = 0.0f;
	m[2][0] = 0.0f;		m[2][1] = 0;		m[2][2] = -q;		m[2][3] = 0.0f;
	m[3][0] = -(l+r)*w;	m[3][1] = -(t+b)*h;	m[3][2] = n*q;		m[3][3] = 1.0f;
	return m;
}

void Camera::Move(const Ogre::Vector3& v)
{
	mPosition += v;
}

void Camera::Strafe(float d)
{
	// mPosition += d*mRight
// 	Ogre::Vector3 s = XMVectorReplicate(d);
// 	Ogre::Vector3 r = XMLoadFloat3(&mRight);
// 	Ogre::Vector3 p = XMLoadFloat3(&mPosition);
// 	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));
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

	Ogre::Matrix3 rot;
	rot.FromAngleAxis(mRight, angle);
// 	mLook = mLook * rot;
	
	mPosition = rot * mPosition;
	LookAt(mPosition, Ogre::Vector3::ZERO, mUp);
	UpdateViewMatrix();
}

void Camera::Yaw(float angle)
{
	Ogre::Matrix3 rot;
	rot.FromAngleAxis(mUp, angle);
// 	mRight = mRight * rot;
// 	mLook = mLook * rot;
// 	UpdateViewMatrix();

	mPosition = mPosition * rot;
	LookAt(mPosition, Ogre::Vector3::ZERO, mUp);
	UpdateViewMatrix();
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

void Camera::RotateAboutCenter(float angle)
{
	m_YawAngle += angle;
	m_PitchAngle += angle;
	
}

void Camera::UpdateViewMatrix()
{
 	using namespace Ogre;
	Vector3 R = mRight;
	Vector3 U = mUp;
	Vector3 L = mLook;
	Vector3 P = mPosition;
	// Keep camera's axes orthogonal to each other and of unit length.
	L.normalise();
	U = L.crossProduct(R);

	// U, L already ortho-normal, so no need to normalize cross product.
	R = U.crossProduct(L);

	// Fill in the view matrix entries.
	float x = -(P.dotProduct(R));// -XMVectorGetX(XMVector3Dot(P, R));
	float y = -(P.dotProduct(U)); //-XMVectorGetX(XMVector3Dot(P, U));
	float z = -(P.dotProduct(L)); // -XMVectorGetX(XMVector3Dot(P, L));

	mRight = R;
	mUp = U;
	mLook = L;

	mView[0][0] = mRight.x; 
	mView[1][0] = mRight.y; 
	mView[2][0] = mRight.z; 
	mView[3][0] = x;   

	mView[0][1] = mUp.x;
	mView[1][1] = mUp.y;
	mView[2][1] = mUp.z;
	mView[3][1] = y;  

	mView[0][2] = mLook.x; 
	mView[1][2] = mLook.y; 
	mView[2][2] = mLook.z; 
	mView[3][2] = z;   

	mView[0][3] = 0.0f;
	mView[1][3] = 0.0f;
	mView[2][3] = 0.0f;
	mView[3][3] = 1.0f;
}


