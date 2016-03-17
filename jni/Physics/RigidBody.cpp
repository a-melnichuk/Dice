#include "RigidBody.h"

RigidBody::RigidBody():
mMass(1.0f), mRestitution(1.0f), mFriction(1.0f), mDampingL(1.0f), mDampingA(1.0f)
{
	mInverseMass = 1.0f/mMass;
}

RigidBody::RigidBody(ndk_helper::Vec3 pos, float mass, float friction, float restitution, float dampingL, float dampingA):
mPos(pos), mMass(mass), mRestitution(restitution), mFriction(friction), mDampingL(dampingL), mDampingA(dampingA),
mAverageAngMotion(1.0f)
{
	mInverseMass = 1.0f/mMass;
}

void RigidBody::intergrate(float dt, ndk_helper::Vec3& acceleration )
{

	float angMotion = mVelocityA.Dot(mVelocityA);
	float linMotion = mVelocityL.Dot(mVelocityL);

	float weightCoeff = pow(0.4f,dt);
	float stableAngMotion = weightCoeff*mAverageAngMotion + (1.0f-weightCoeff)*angMotion;

	mPrevAccelerationL = mAccelerationL;

	if(stableAngMotion < 0.01f)
		mOrientation = mPrevOrientation;
	else
		mPrevOrientation = mOrientation;

	if(linMotion < 0.005f)
		mPos = mPrevPos;
	else
		mPrevPos = mPos;

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

	mAverageAngMotion = 0.8f * mAverageAngMotion + 0.2f*angMotion;
	mAverageAngMotion *= 0.2f;
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
