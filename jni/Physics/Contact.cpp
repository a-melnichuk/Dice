#include "Contact.h"

void Contact::calcContactToWorld()
{
	ndk_helper::Vec3 contactTangent[2];

	    if (fabs(mNormal.x_) > fabs(mNormal.y_))
	    {
	        float s = 1.0f/sqrtf(mNormal.z_*mNormal.z_ + mNormal.x_*mNormal.x_);

	        contactTangent[0] = ndk_helper::Vec3(mNormal.z_*s, 0.0f, -mNormal.x_*s);

	        contactTangent[1].x_ = mNormal.y_*contactTangent[0].x_;
	        contactTangent[1].y_ = mNormal.z_*contactTangent[0].x_ - mNormal.x_*contactTangent[0].z_;
	        contactTangent[1].z_ = -mNormal.y_*contactTangent[0].x_;
	    }
	    else
	    {
	        float s = 1.0f/sqrtf(mNormal.z_*mNormal.z_ + mNormal.y_*mNormal.y_);

	        contactTangent[0] = ndk_helper::Vec3(0.0f, -mNormal.z_*s, mNormal.y_*s);

	        contactTangent[1].x_ = mNormal.y_*contactTangent[0].z_ - mNormal.z_*contactTangent[0].y_;
	        contactTangent[1].y_ = -mNormal.x_*contactTangent[0].z_;
	        contactTangent[1].z_ = mNormal.x_*contactTangent[0].y_;
	    }

	    mContactW = ndk_helper::Mat3(mNormal, contactTangent[0], contactTangent[1]);
}


void Contact::calcVelocity(float dt)
{
	ndk_helper::Vec3 velocity = mBody->mVelocityA.Cross(mRelativePos);
    velocity += mBody->mVelocityL;

    mVelocity = velocity * mContactW;

    ndk_helper::Vec3 accVelocity = mBody->mPrevAccelerationL * dt;

    accVelocity = accVelocity * mContactW;

    accVelocity.x_ = 0.0f;

    mVelocity += accVelocity;
}

void Contact::calcVelocityChange(float dt)
{
    float velocityFromAcc = mBody->mPrevAccelerationL.Dot(mNormal) * dt;
    float restitution = fabs(mVelocity.x_) < MIN_VELOCITY ? 0.0f : mBody->mRestitution;

    mVelocityChange = -mVelocity.x_ - restitution * (mVelocity.x_ - velocityFromAcc);
}


void Contact::applyVelocityChange(ndk_helper::Vec3& velocityChange,
								  ndk_helper::Vec3& rotationChange)
{
	ndk_helper::Vec3 impulseContact = calcFrictionImpulse();
    ndk_helper::Vec3 impulse = mContactW * impulseContact;

    ndk_helper::Vec3 impulsiveTorque = mRelativePos.Cross(impulse);
    rotationChange = mBody->mInverseInertiaTensorW * impulsiveTorque;

    ndk_helper::Vec3 origin(0.0f,0.0f,0.0f);
    velocityChange = origin;
    velocityChange += impulse * 1.0f/mBody->mMass;

    mBody->mVelocityL += velocityChange;
    mBody->mVelocityA += mBody->mDampingA * rotationChange;
}

ndk_helper::Vec3 Contact::calcFrictionImpulse()
{
    ndk_helper::Mat3 impulseToTorque = ndk_helper::Mat3::CrossProductMatrix(mRelativePos);
    ndk_helper::Mat3 deltaVelWorld = impulseToTorque * mBody->mInverseInertiaTensorW * impulseToTorque;
    deltaVelWorld *= -1.0f;

    ndk_helper::Mat3 deltaVelocity = mContactW.Transpose() * deltaVelWorld * mContactW;

    deltaVelocity.f_[0] += mBody->mInverseMass;
    deltaVelocity.f_[4] += mBody->mInverseMass;
    deltaVelocity.f_[8] += mBody->mInverseMass;

    ndk_helper::Mat3 impulseMatrix = deltaVelocity;
    impulseMatrix.Inverse();

    ndk_helper::Vec3 velKill(mVelocityChange, -mVelocity.y_, -mVelocity.z_);
    ndk_helper::Vec3 impulseContact = impulseMatrix * velKill;

    float friction =  mBody->mFriction;
    float planarImpulse = sqrtf(impulseContact.y_*impulseContact.y_ + impulseContact.z_*impulseContact.z_);

    if (planarImpulse > impulseContact.x_ * friction)
    {
        impulseContact.y_ /= planarImpulse;
        impulseContact.z_ /= planarImpulse;

        impulseContact.x_ = deltaVelocity.f_[0] +
        					deltaVelocity.f_[3]*friction*impulseContact.y_ +
        					deltaVelocity.f_[6]*friction*impulseContact.z_;
        impulseContact.x_ = mVelocityChange / impulseContact.x_;
        impulseContact.y_ *= friction * impulseContact.x_;
        impulseContact.z_ *= friction * impulseContact.x_;
    }

    return impulseContact;
}



void Contact::applyPositionChange(ndk_helper::Vec3& linearChange,
								  ndk_helper::Vec3& angularChange,
                                  float penetration)
{
    float angularMove;
    float linearMove;

    float totalInertia = 0;
    float linearInertia;
    float angularInertia;

	ndk_helper::Vec3 angularInertiaWorld = mRelativePos.Cross(mNormal);
	angularInertiaWorld = mBody->mInverseInertiaTensorW *  angularInertiaWorld;
	angularInertiaWorld = angularInertiaWorld.Cross(mRelativePos);

	angularInertia = angularInertiaWorld.Dot(mNormal);

	linearInertia = mBody->mInverseMass;

	totalInertia += linearInertia + angularInertia;

	angularMove = penetration * (angularInertia / totalInertia);
	linearMove = penetration * (linearInertia / totalInertia);

	ndk_helper::Vec3 projection = mRelativePos;
	projection += mNormal * ( mRelativePos.Dot(mNormal) );

	float maxMagnitude = ANGULAR_LIMIT * projection.Length();
	float totalMove = angularMove + linearMove;
	if (angularMove < -maxMagnitude)
	{
		angularMove = -maxMagnitude;
	}
	else if (angularMove > maxMagnitude)
	{
		angularMove = maxMagnitude;
	}
	linearMove = totalMove - angularMove;

	if (angularMove == 0.0f)
	{
		ndk_helper::Vec3 origin;
		angularChange = origin;
	}
	else
	{
		ndk_helper::Vec3 targetAngularDirection = mRelativePos.Cross(mNormal);
		angularChange = (mBody->mInverseInertiaTensorW * targetAngularDirection) * (angularMove / angularInertia);
	}

	linearChange = mNormal;
	linearChange *= linearMove;

	mBody->mPos = mBody->mPos + linearChange;



	ndk_helper::Vec4 orientation(angularChange, 0.0f);
	mBody->mOrientation += orientation;
}

