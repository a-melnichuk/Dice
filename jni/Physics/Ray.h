#ifndef _DICE_RAY_
#define _DICE_RAY_

#include "Primitive.h"

class Ray : public Primitive
{
public:
	Ray(){};
	Ray(ndk_helper::Vec3& dir, ndk_helper::Vec3& pos);
	Ray(ndk_helper::Vec4& dir, ndk_helper::Vec4& pos);
	virtual __inline__ size_t getType() { return RAY; }
	ndk_helper::Vec3 mDir, mDirInverse, mPos;
	int mSign[3];


};
#endif
