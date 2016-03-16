#ifndef _DICE_SHADOW_MAP_
#define _DICE_SHADOW_MAP_

#include "Shape.h"

class ShadowMap : public Shape<ShadowMap>
{
public:
	ShadowMap();
	void init(ndk_helper::Vec3& lightPos,ndk_helper::Vec3& lightDir,Camera& camera );
	void draw();
	void update();
	void initShaders();
	void bindShaderAttributes(GLuint program);
	struct ShaderParams { GLuint program, VP, M; };
	enum SHADER_ATTR { ATTR_VERTEX };
	ndk_helper::Mat4 mLightViewProj;
	GLuint SHADOW_WIDTH, SHADOW_HEIGHT;
	GLuint mDepthMapFBO;
	GLuint mDepthMap;
	GLuint mRenderBuffer;
	bool mHasDepthExtension;
	ShaderParams mShaderParams;
};

#endif
