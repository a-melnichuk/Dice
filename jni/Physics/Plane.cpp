#include "Plane.h"

Plane::Plane()
{

}

Plane::Plane(ndk_helper::Vec3& normal, ndk_helper::Vec3& pos)
{
	mPos = pos;
	mNormal = normal;
	mDist = pos.Length();
}
