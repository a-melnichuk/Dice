#include "Ray.h"

Ray::Ray(ndk_helper::Vec3& dir, ndk_helper::Vec3& pos): mDir(dir),mPos(pos)
{
	mDirInverse.x_ = 1.0f/mDir.x_;
	mDirInverse.y_ = 1.0f/mDir.y_;
	mDirInverse.z_ = 1.0f/mDir.z_;

	mSign[0] = mDirInverse.x_ < 0;
	mSign[1] = mDirInverse.y_ < 0;
	mSign[2] = mDirInverse.z_ < 0;
}

Ray::Ray(ndk_helper::Vec4& dir, ndk_helper::Vec4& pos)
{
	mDir.x_ = dir.x_;
	mDir.y_ = dir.y_;
	mDir.z_ = dir.z_;

	mPos.x_ = pos.x_;
	mPos.y_ = pos.y_;
	mPos.z_ = pos.z_;

	mDirInverse.x_ = 1.0f/mDir.x_;
	mDirInverse.y_ = 1.0f/mDir.y_;
	mDirInverse.z_ = 1.0f/mDir.z_;

	mSign[0] = mDirInverse.x_ < 0;
	mSign[1] = mDirInverse.y_ < 0;
	mSign[2] = mDirInverse.z_ < 0;
}
