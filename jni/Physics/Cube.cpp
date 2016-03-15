#include "Cube.h"

Cube::Cube(ndk_helper::Vec3 halfSize, RigidBody* rigidBody): mHalfSize(halfSize), mRigidBody(rigidBody)
{
	setCubeVertices(mHalfSize);
	if(rigidBody != NULL)
	{
		mHasStaticRigidBody = false;
		mRigidBody = rigidBody;
		setInverseInertiaTensor(mRigidBody->mInverseInertiaTensor, mRigidBody->mMass, mHalfSize);
	}
	else
	{
		mHasStaticRigidBody = true;
		mRigidBody = new RigidBody();
		mRigidBody->mInverseInertiaTensor = ndk_helper::Mat3::Identity();
	}
	mRigidBody->mInverseInertiaTensorW = mRigidBody->mInverseInertiaTensor;

	mBounds[0] = mBounds[1] = mHalfSize;
	mBounds[0] *= -1;

	mRadius = mHalfSize.Length();
}


Cube::~Cube()
{
	delete mRigidBody;
}

void Cube::setInverseInertiaTensor(ndk_helper::Mat3& out, float mass, ndk_helper::Vec3& dist)
{
	out = ndk_helper::Mat3::Identity();
	float c = 1.0f/12.0f*mass;
	float x2 = dist.x_*dist.x_;
	float y2 = dist.y_*dist.y_;
	float z2 = dist.z_*dist.z_;

	out.f_[0] = c*(y2 + z2);
	out.f_[4] = c*(x2 + z2);
	out.f_[8] = c*(x2 + y2);

	out.Inverse();
}


void Cube::setCubeVertices(ndk_helper::Vec3& halfSize)
{
	ndk_helper::Vec3 vertices[8] = {
		ndk_helper::Vec3(-halfSize.x_, -halfSize.y_, -halfSize.z_),
		ndk_helper::Vec3(-halfSize.x_, halfSize.y_, -halfSize.z_),
		ndk_helper::Vec3(halfSize.x_, halfSize.y_, -halfSize.z_),
		ndk_helper::Vec3(halfSize.x_, -halfSize.y_, -halfSize.z_),

		ndk_helper::Vec3(-halfSize.x_, -halfSize.y_, halfSize.z_),
		ndk_helper::Vec3(-halfSize.x_, halfSize.y_, halfSize.z_),
		ndk_helper::Vec3(halfSize.x_, halfSize.y_, halfSize.z_),
		ndk_helper::Vec3(halfSize.x_, -halfSize.y_, halfSize.z_)
	};

	for(size_t i=0;i<8;++i)
	{
		mCubeVertices[i] = mRigidBody->mModel * vertices[i];
	}

}

void Cube::update(float dt, ndk_helper::Vec3& acceleration)
{
	if(mHasStaticRigidBody) return;
	mRigidBody->intergrate(dt,acceleration);
	setCubeVertices(mHalfSize);
}

float Cube::getCubeAxisLength(ndk_helper::Vec3& axis)
{
	return  mHalfSize.x_ * fabs( axis.Dot(mRigidBody->mModel.GetRow(0) ) ) +
			mHalfSize.y_ * fabs( axis.Dot(mRigidBody->mModel.GetRow(1) ) ) +
			mHalfSize.z_ * fabs( axis.Dot(mRigidBody->mModel.GetRow(2) ) );
}

