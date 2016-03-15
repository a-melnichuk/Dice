#ifndef _DICE_CAMERA_
#define _DICE_CAMERA_

#include <GLES2/gl2.h>
#include "NDKHelper.h"

#define NEAR 0.4f
#define FAR 25.0f

class Camera
{

public:
	Camera( ndk_helper::Vec3 pos,
			ndk_helper::Vec3 lookAt,
			ndk_helper::Vec3 up,
			float near = NEAR, float far = FAR);

	void initView();
	void initProj();
	void update();

	float mNear, mFar;

	ndk_helper::Mat4 mP, mV,
					mViewProj , mViewProjInverse;

	ndk_helper::Vec3 mPos,
					mLookAt, mUp,
					mNormal;

};

#endif
