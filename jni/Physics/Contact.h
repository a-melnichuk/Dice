#ifndef _DICE_CONTACT_
#define _DICE_CONTACT_

#include <math.h>
#include "NDKHelper.h"
#include "RigidBody.h"
#include "../Logs.h"

#define MIN_VELOCITY 0.1f
#define ANGULAR_LIMIT 0.2f

class Contact
{
public:
	Contact(){}

	void calcContactToWorld();
	void calcVelocity(float dt);
	void calcVelocityChange(float dt);
	void applyVelocityChange(ndk_helper::Vec3& velocityChange,
							 ndk_helper::Vec3& rotationChange);
	void applyPositionChange(ndk_helper::Vec3& linearChange,
							 ndk_helper::Vec3& angularChange,
	                         float penetration);
	ndk_helper::Vec3 calcFrictionImpulse();
	RigidBody *mBody;
	ndk_helper::Vec3 mPoint, mNormal, mRelativePos, mVelocity;
	float mPenetration, mVelocityChange;
	ndk_helper::Mat3 mContactW;
};


#endif
