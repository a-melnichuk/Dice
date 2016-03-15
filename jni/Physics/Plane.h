#ifndef _DICE_PLANE_
#define _DICE_PLANE_

#include "Primitive.h"

class Plane : public Primitive
{
public:
	Plane();
	~Plane(){};
	Plane(ndk_helper::Vec3& normal, ndk_helper::Vec3& pos);
	virtual __inline__ size_t getType() { return PLANE; }
	ndk_helper::Vec3 mNormal, mPos;
	float mDist;


};

#endif
