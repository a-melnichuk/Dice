#include "Camera.h"

Camera::Camera(	ndk_helper::Vec3 pos,
				ndk_helper::Vec3 lookAt,
				ndk_helper::Vec3 up,
				float near, float far):
				mPos(pos), mLookAt(lookAt), mUp(up),
				mNear(near), mFar(far)
{
}

void Camera::initView()
{
    int32_t viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    float a = (float) viewport[2] / (float) viewport[3];

    mP = ndk_helper::Mat4::Perspective(a, 1.0f, mNear, mFar);
}

void Camera::initProj()
{
	mV = ndk_helper::Mat4::LookAt(mPos, mLookAt, mUp);
}

void Camera::update()
{
	mViewProj = mP * mV;
	mViewProjInverse = mViewProj;
	mViewProjInverse.Inverse();

	mNormal = mLookAt - mPos;
	mNormal.Normalize();
}
