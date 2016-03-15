#ifndef _DICE_DICE_
#define _DICE_DICE_

#include "Shape.h"
#include "../Physics/RigidBody.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class Dice : public Shape<Dice>
{
public:
    Dice(Camera& camera, TextureManager& textureManager);
    ~Dice();
    void init(RigidBody* rigidBody, ndk_helper::Vec3& halfSize);
    void draw();
    void update( float dt );
    void unload();
    RigidBody* mRigidBody;
private:
    struct Vertex { float pos[3], normal[3]; };
    struct ShaderParams { GLuint program, MVP, M, lightColor, eyePos, sampler; };
    enum SHADER_ATTR { ATTR_VERTEX, ATTR_NORMAL };

    void initShaders();
	void initBuffers();
	void initTextures();

	void bindShaderAttributes(GLuint program);


	int32_t mNumIdices,
	    	mNumVertices;

	GLuint	mIbo,
			mVbo;

	CubeMap* mTexture;
	Camera& mCamera;
	TextureManager& mTextureManager;
	ShaderParams mShaderParams;
	Vertex mVertex;

	ndk_helper::Mat4 mM, mMVP, mInitial;
	ndk_helper::Vec3 mHalfSize;

};

#endif

