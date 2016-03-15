#include "RigidBody.h"

RigidBody::RigidBody():
mMass(1.0f), mRestitution(1.0f), mFriction(1.0f), mDampingL(1.0f), mDampingA(1.0f)
{
	mInverseMass = 1.0f/mMass;
}

RigidBody::RigidBody(ndk_helper::Vec3 pos, float mass, float friction, float restitution, float dampingL, float dampingA):
mPos(pos), mMass(mass), mRestitution(restitution), mFriction(friction), mDampingL(dampingL), mDampingA(dampingA)
{
	mInverseMass = 1.0f/mMass;
}

void RigidBody::intergrate(float dt, ndk_helper::Vec3& acceleration )
{
	mPrevAccelerationL = mAccelerationL;
	ndk_helper::Vec3 accDiff = acceleration - mPrevAccelerationL;

	mAccelerationL = acceleration;

	mVelocityL += mAccelerationL * dt;

	mVelocityL *= pow(mDampingL, dt);
	mVelocityL.Clamp(MAX_VELOCITY);

	mVelocityA += mAccelerationA * dt;
	mVelocityA *= pow(mDampingA, dt);

	mPos += mVelocityL * dt;

	ndk_helper::Vec4 orientation(mVelocityA * dt, 0.0f);
	mOrientation += orientation;

	updateModel();
}

void RigidBody::updateModel()
{
	updateModelMatrix();
	updateInverseInertiaTensor();
}

void RigidBody::updateModelMatrix()
{
	mModel = ndk_helper::Mat4::Translation(mPos);
	mOrientation.Normalize();
	mOrientation.ToMatrixPreserveTranslate(mModel);
}

void RigidBody::updateInverseInertiaTensor()
{
	ndk_helper::Mat3 model = mModel.GetMat3();
	mInverseInertiaTensorW = model * mInverseInertiaTensor * model.Transpose();
}
