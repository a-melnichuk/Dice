#include "CollisionManager.h"

CollisionManager::CollisionManager():mPick(false)
{}

void CollisionManager::add(Primitive* primitive)
{
	primitive->hasStaticRigidBody()
	? mStaticPrimitives.push_back(primitive)
	: mDynamicPrimitives.push_back(primitive);
}

void CollisionManager::update(float dt)
{
	if(mPick && mDynamicPrimitives.size() > 0)
	{
		for(size_t i=0;i<mDynamicPrimitives.size();++i)
			resolveCollision(*mDynamicPrimitives[i], mPickRay);

		mPick = false;
	}
	updateRigidBodies(dt,mAccelerometerAcceleration);
	resolveCollisions();
	updateContacts(dt);
	updatePositions(dt);
	updateVelocities(dt);
	mContacts.clear();


}

void CollisionManager::updateRigidBodies(float dt, ndk_helper::Vec3& acceleration)
{
	for(size_t i=0;i<mDynamicPrimitives.size();++i)
	{
		mDynamicPrimitives[i]->update(dt,acceleration);
	}
}

void CollisionManager::updateContacts(float dt)
{
	for(size_t i=0;i<mContacts.size();++i)
	{
		mContacts[i].calcContactToWorld();
		mContacts[i].mRelativePos = mContacts[i].mPoint - mContacts[i].mBody->mPos;
		mContacts[i].calcVelocity(dt);
		mContacts[i].calcVelocityChange(dt);
	}
}

void CollisionManager::updatePositions(float dt)
{
		size_t numContacts = mContacts.size();

	    // iteratively resolve interpenetrations in order of severity.
	    size_t positionIterationsUsed = 0;
	    size_t positionIterations = 15;
	    while(positionIterationsUsed < positionIterations)
	    {
	    	ndk_helper::Vec3 linearChange, angularChange;
	    	ndk_helper::Vec3 deltaPosition;
	        // Find biggest penetration
	        float max = MIN_POSITION_CHANGE;
	        size_t index = numContacts;
	        for(size_t i=0;i<numContacts;++i)
	        {
	            if(mContacts[i].mPenetration > max)
	            {
	                max = mContacts[i].mPenetration;
	                index = i;
	            }
	        }

	        if(index == numContacts) break;
	        // Resolve the penetration.
	        mContacts[index].applyPositionChange(linearChange, angularChange, max);

	        for(size_t i=0;i<numContacts;++i)
			{
	        	if(mContacts[index].mBody == mContacts[i].mBody)
	        	{
	        		deltaPosition = linearChange + angularChange.Cross(mContacts[i].mRelativePos);
	        		float penetrationChange =  deltaPosition.Dot(mContacts[i].mNormal);
	        		mContacts[i].mPenetration -= penetrationChange;
	        	}
			}

	        ++positionIterationsUsed;
	    }
}

void CollisionManager::updateVelocities(float dt)
{
	    size_t numContacts = mContacts.size();
	    size_t velocityIterationsUsed = 0;
	    size_t velocityIterations = 15;

	    while (velocityIterationsUsed < velocityIterations)
	    {
	    	ndk_helper::Vec3 velocityChange, rotationChange;
	    	ndk_helper::Vec3 deltaVel;
	        // Find contact with maximum magnitude of probable velocity change.
	        float max = MIN_VELOCITY_CHANGE;
	        size_t index = numContacts;
	        for(size_t i=0;i<numContacts;++i)
	        {
	            if(mContacts[i].mVelocityChange > max)
	            {
	                max = mContacts[i].mVelocityChange;
	                index = i;
	            }
	        }
	        if(index == numContacts) break;

	        // Do the resolution on the contact that came out top.
	        mContacts[index].applyVelocityChange(velocityChange, rotationChange);

	        for(size_t i=0;i<numContacts;++i)
			{
	        	if(mContacts[index].mBody == mContacts[i].mBody)
	        	{
	        		deltaVel = velocityChange + rotationChange.Cross(mContacts[i].mRelativePos);
	        		mContacts[i].mVelocity += deltaVel * mContacts[i].mContactW;
	        		mContacts[i].calcVelocityChange(dt);
	        	}
			}

	        ++velocityIterationsUsed;
	    }
}

void CollisionManager::resolveCollisions()
{
	size_t numDynamicPrimitives = mDynamicPrimitives.size();
	if(numDynamicPrimitives > 0)
	{
		if(mStaticPrimitives.size() > 0)
			for(size_t i=0;i<numDynamicPrimitives;++i)
				for(size_t j=0;j<mStaticPrimitives.size();++j)
				{
					resolveCollision(*mDynamicPrimitives[i], *mStaticPrimitives[j]);
				}
		if(numDynamicPrimitives > 1)
			for(size_t i=0;i<numDynamicPrimitives;++i)
				for(size_t j=0;j<numDynamicPrimitives;++j)
				{
					if(j != i)
						resolveCollision(*mDynamicPrimitives[i], *mDynamicPrimitives[j]);
				}
	}
}


void CollisionManager::resolveCollision(Primitive& a, Primitive& b)
{
	unsigned aType = a.getType();
	unsigned primitiveTypeSum = aType | b.getType();

	if(primitiveTypeSum == 0 || (a.hasStaticRigidBody() && b.hasStaticRigidBody() ) ) return;

	switch(primitiveTypeSum)
	{
	case PLANE_CUBE:
		return aType == PLANE
		? resolvePlaneCube((Plane&) a, (Cube&) b)
		: resolvePlaneCube((Plane&) b, (Cube&) a);
	case CYLINDER_CUBE:
		return aType == CYLINDER
		? resolveCylinderCube((Cylinder&) a, (Cube&) b)
		: resolveCylinderCube((Cylinder&) b, (Cube&) a);
	case RAY_CUBE:
		return aType == RAY
		? resolveRayCube((Ray&) a, (Cube&) b)
		: resolveRayCube((Ray&) b, (Cube&) a);
	}
}

void CollisionManager::resolvePlaneCube(Plane& plane, Cube& cube)
{
	if(!intersectionPlaneCube(plane, cube)) return;

	for(size_t i=0;i<8;++i)
	{
		ndk_helper::Vec3 vertexPosW = cube.mCubeVertices[i] + cube.mRigidBody->mPos;
		float vertexDist = vertexPosW.Dot(plane.mNormal);
		float distDiff = vertexDist + plane.mDist;

		if(distDiff < 0)
		{
			mContacts.push_back(Contact());
			Contact& contact = mContacts[mContacts.size() - 1];
			contact.mBody = cube.mRigidBody;
			contact.mNormal = plane.mNormal;
			contact.mPenetration = -distDiff;
			contact.mPoint = contact.mPenetration * plane.mNormal + vertexPosW;

		}
	}


}

void CollisionManager::resolveCylinderCube(Cylinder& cylinder, Cube& cube)
{
	resolvePlaneCube(cylinder.mBottomCap, cube);
	resolvePlaneCube(cylinder.mTopCap, cube);

	if(!intersectionCylinderCube(cylinder, cube)) return;

	float r2 = cylinder.mRadius*cylinder.mRadius;

	for(size_t i = 0; i < 8; ++i)
	{
		ndk_helper::Vec3 vertexPosW = cube.mCubeVertices[i] + cube.mRigidBody->mPos;
		ndk_helper::Vec3 center = cylinder.mCenter;
		center.y_ = vertexPosW.y_;

		if( (vertexPosW - center).SquaredLength() > r2 )
		{

			mContacts.push_back(Contact());
			Contact& contact = mContacts[mContacts.size() - 1];

			contact.mBody = cube.mRigidBody;
			contact.mNormal = center - vertexPosW;
			contact.mPenetration = contact.mNormal.Length() - cylinder.mRadius;

			contact.mNormal.Normalize();
			contact.mPoint = contact.mPenetration * contact.mNormal + vertexPosW;

		}
	}
}


void CollisionManager::resolveRayCube(Ray& ray, Cube& cube)
{
	ndk_helper::Mat4 modelInverse = cube.mRigidBody->mModel;
	modelInverse.Inverse();
	ndk_helper::Vec4 rayPosM = modelInverse * ndk_helper::Vec4(ray.mPos, 1.0f);
	ndk_helper::Vec4 rayDirM = modelInverse * ndk_helper::Vec4(ray.mDir, 0.0f);

	Ray r(rayDirM,rayPosM);

	float tmin, tmax, ty_min, ty_max, tz_min, tz_max;

	tmin = (cube.mBounds[r.mSign[0]].x_ - r.mPos.x_) * r.mDirInverse.x_;
	tmax = (cube.mBounds[1-r.mSign[0]].x_ - r.mPos.x_) * r.mDirInverse.x_;
	ty_min = (cube.mBounds[r.mSign[1]].y_ - r.mPos.y_) * r.mDirInverse.y_;
	ty_max = (cube.mBounds[1-r.mSign[1]].y_ - r.mPos.y_) * r.mDirInverse.y_;

	if ((tmin > ty_max) || (ty_min > tmax))
	{
		return;
	}

	if (ty_min > tmin)
		tmin = ty_min;
	if (ty_max < tmax)
		tmax = ty_max;

	tz_min = (cube.mBounds[r.mSign[2]].z_ - r.mPos.z_) * r.mDirInverse.z_;
	tz_max = (cube.mBounds[1-r.mSign[2]].z_ - r.mPos.z_) * r.mDirInverse.z_;

	if ((tmin > tz_max) || (tz_min > tmax))
	{
		return;
	}

	if (tz_min > tmin)
		tmin = tz_min;
	if (tz_max < tmax)
		tmax = tz_max;

	float forceL = 7.0f;

	cube.mRigidBody->mVelocityL += forceL * ray.mDir;

}


bool CollisionManager::intersectionPlaneCube(Plane& plane, Cube& cube)
{
	if(cube.hasStaticRigidBody()) return false;
	float projectedRadius = cube.getCubeAxisLength(plane.mNormal);

	float boxDistance = plane.mNormal.Dot( cube.mRigidBody->mPos ) - projectedRadius;
	return boxDistance <= plane.mDist;
}

bool CollisionManager::intersectionCylinderCube(Cylinder& cylinder, Cube& cube)
{
	if(cube.hasStaticRigidBody()) return false;

	float remainingRadius = cylinder.mRadius - cube.mRadius;

	return fabs(cube.mRigidBody->mPos.x_ - cylinder.mCenter.x_) >= remainingRadius
		|| fabs(cube.mRigidBody->mPos.z_ - cylinder.mCenter.z_) >= remainingRadius;
}

void CollisionManager::unload()
{
	for(int i=0;i<mStaticPrimitives.size();++i)
	{
		delete (mStaticPrimitives[i]);
	}

	mStaticPrimitives.clear();

	for (int i=0;i<mDynamicPrimitives.size();++i)
	{
		delete (mDynamicPrimitives[i]);
	}

	mDynamicPrimitives.clear();
}

