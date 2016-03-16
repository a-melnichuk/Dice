#include "ShadowMap.h"

ShadowMap::ShadowMap(): SHADOW_WIDTH(512), SHADOW_HEIGHT(512), mHasDepthExtension(false)
{
}


void ShadowMap::bindShaderAttributes(GLuint program)
{
	glBindAttribLocation( program,  ATTR_VERTEX, "a_Position" );
}

void ShadowMap::initShaders()
{
	mShaderParams.program = loadShaders( "shaders/shadow_map.vsh", "shaders/shadow_map.fsh" );
	mShaderParams.VP = glGetUniformLocation( mShaderParams.program, "u_VPMatrix" );
	mShaderParams.M = glGetUniformLocation( mShaderParams.program, "u_ModelMatrix" );
}

void ShadowMap::init(ndk_helper::Vec3& lightPos,ndk_helper::Vec3& lightDir,Camera& camera )
{
	int32_t viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );
	initShaders();
	float a = (float) viewport[2] / (float) viewport[3];
	ndk_helper::Vec3 up(0.0f, 1.0f, 0.0f);
	ndk_helper::Mat4 lightView = ndk_helper::Mat4::LookAt( lightPos,lightDir, up);
	ndk_helper::Mat4 lightProj = ndk_helper::Mat4::Perspective( a, 1.0f , camera.mNear, camera.mFar);
	mLightViewProj = lightProj * lightView;
    const GLubyte* str = glGetString(GL_EXTENSIONS);
    if(strstr((const char *)str, "OES_depth_texture") != NULL)
    {
    	mHasDepthExtension = true;
    	LOGI("HAS_EXT");
    }

	//Create the shadow map texture
	glGenTextures(1, &mDepthMap);
	glBindTexture(GL_TEXTURE_2D, mDepthMap);

	// Create the depth texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	// Set the textures parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// create frame buffer object for shadow pass
	glGenFramebuffers(1, &mDepthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMap, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOGW("failed to make complete framebuffer object");
	}


	checkGlError("a");


	checkGlError("f");

	 glGenTextures(1, &mDepthMap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glGenFramebuffers(1, &mDepthMapFBO);
	glGenRenderbuffers(1, &mRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBO);
	glBindTexture(GL_TEXTURE_2D, mDepthMap);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
							  GL_DEPTH_ATTACHMENT,
							  GL_RENDERBUFFER,
							  mRenderBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mDepthMap, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOGI("Error: FrameBufferObject is not complete!\n");
	}

}
