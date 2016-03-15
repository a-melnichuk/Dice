#ifndef _DICE_PRIMITIVE_
#define _DICE_PRIMITIVE_

#define PRIMITIVE 1
#define CUBE 2
#define PLANE 4
#define CYLINDER 8
#define RAY 16

#include "NDKHelper.h"
#include "RigidBody.h"
#include "../Logs.h"

class Primitive
{
public:
	Primitive(){};
	virtual void update(float dt, ndk_helper::Vec3& acceleration){}
	virtual void setInverseInertiaTensor(ndk_helper::Mat3& out, float mass, ndk_helper::Vec3& dist) {}

	virtual __inline__ size_t getType() { return PRIMITIVE; }
	virtual __inline__ bool hasStaticRigidBody() { return true; }

};

#endif
