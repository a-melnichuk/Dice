#ifndef _DICE_COLLISION_MANAGER_
#define _DICE_COLLISION_MANAGER_

#include <vector>
#include <iostream>
#include <float.h>

#include "RigidBody.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Plane.h"
#include "Ray.h"
#include "Contact.h"
#include "../Logs.h"

#define PLANE_CUBE (CUBE | PLANE)
#define CYLINDER_CUBE (CUBE | CYLINDER)
#define RAY_CUBE (CUBE | RAY)

#define MIN_VELOCITY_CHANGE 0.01f
#define MIN_POSITION_CHANGE 0.01f



class CollisionManager
{
public:
	CollisionManager();

	void update(float dt);
	void unload();
	void add(Primitive* primitive);

	ndk_helper::Vec3 mAccelerometerAcceleration;
	Ray mPickRay;
	bool mPick;

private:
	void resolveCollisions();
	void updateContacts(float dt);
	void updatePositions(float dt);
	void updateVelocities(float dt);
	void updateRigidBodies(float dt, ndk_helper::Vec3& acceleration);

	void resolveCollision(Primitive& a, Primitive& b);
	void resolvePlaneCube(Plane& plane, Cube& cube);
	void resolveCylinderCube(Cylinder& cylinder, Cube& cube);
	void resolveRayCube(Ray& ray, Cube& cube);

	bool intersectionPlaneCube(Plane& plane, Cube& cube);
	bool intersectionCylinderCube(Cylinder& cylinder, Cube& cube);

	std::vector<Primitive*> mDynamicPrimitives;
	std::vector<Primitive*> mStaticPrimitives;
	std::vector<Contact> mContacts;

};


#endif
