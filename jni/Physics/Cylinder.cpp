#include "Cylinder.h"

Cylinder::Cylinder(){}

Cylinder::Cylinder(float radius, float height):
		mRadius(radius), mHeight(height)
{
	mCenter = ndk_helper::Vec3(0.0f, 0.0f, 0.0f);
	ndk_helper::Vec3 pos(0.0f, 0.5f*height, 0.0f);
	ndk_helper::Vec3 normal(0.0f, -1.0f, 0.0f);

	float dist = pos.Length();

	mTopCap.mNormal = normal;
	mTopCap.mPos = pos;
	mTopCap.mDist = dist;

	normal.y_ *= -1;
	pos.y_ *= -1;

	mBottomCap.mNormal = normal;
	mBottomCap.mPos = pos;
	mBottomCap.mDist = dist;
}
