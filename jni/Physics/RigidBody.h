#ifndef _DICE_RIGID_BODY_
#define _DICE_RIGID_BODY_

#include <math.h>
#include "NDKHelper.h"
#include "../Logs.h"

#define MAX_VELOCITY 10.0f


class RigidBody
{
public:
	RigidBody();
	RigidBody(ndk_helper::Vec3 pos, float mass, float friction, float restitution, float dampingL, float dampingA);

	ndk_helper::Vec3 getForce();
	void intergrate(float dt, ndk_helper::Vec3& acceleration);
	void updateModel();
	void updatePrevVectors();
	void __inline__ updateModelMatrix();
	void __inline__ updateInverseInertiaTensor();

	float mFriction;
	float mMass, mInverseMass , mRestitution;
	float mDampingL, mDampingA;

	ndk_helper::Vec3 mPos;
	ndk_helper::Vec3 mVelocityL;
	ndk_helper::Vec3 mVelocityA;
	ndk_helper::Vec3 mAccelerationL, mPrevAccelerationL;
	ndk_helper::Vec3 mAccelerationA, mPrevAccelerationA;
	ndk_helper::Quaternion mOrientation;
	ndk_helper::Mat4 mModel;
	ndk_helper::Mat3 mInverseInertiaTensor, mInverseInertiaTensorW;
private:
};

#endif
