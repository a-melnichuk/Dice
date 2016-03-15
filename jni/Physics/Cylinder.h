#ifndef _DICE_CYLINDER_
#define _DICE_CYLINDER_

#include "Plane.h"

class Cylinder : public Primitive
{
public:
	Cylinder();
	Cylinder(float radius, float height);
	virtual __inline__ size_t getType() { return CYLINDER; }
	virtual void collide(Primitive* primitive) {};

	ndk_helper::Vec3 mCenter;
	Plane mTopCap, mBottomCap;
	float mRadius, mHeight;
};

#endif
