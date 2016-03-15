#ifndef _DICE_CUBE_
#define _DICE_CUBE_

#include <math.h>

#include "Primitive.h"
#include "RigidBody.h"

class Cube : public Primitive
{
public:
	Cube(ndk_helper::Vec3 halfSize, RigidBody* rigidBody);
	~Cube();
	virtual void update(float dt, ndk_helper::Vec3& acceleration);
	virtual void setInverseInertiaTensor(ndk_helper::Mat3& out, float mass, ndk_helper::Vec3& dist);
	virtual __inline__ size_t getType() { return CUBE; }
	virtual __inline__ bool hasStaticRigidBody() { return mHasStaticRigidBody; }

	float getCubeAxisLength(ndk_helper::Vec3& axis);

	RigidBody* mRigidBody;
	ndk_helper::Vec3 mHalfSize;
	ndk_helper::Vec3 mCubeVertices[8];
	ndk_helper::Vec3 mBounds[2];
	float mRadius;
private:
	void setCubeVertices(ndk_helper::Vec3& halfSize);
	bool mHasStaticRigidBody;
};

#endif
