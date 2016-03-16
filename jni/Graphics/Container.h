#ifndef _DICE_CONTAINER_
#define _DICE_CONTAINER_

#define _USE_MATH_DEFINES

#include <math.h>
#include <algorithm>
#include <vector>
#include "Shape.h"
#include "ShadowMap.h"

class Container : public Shape<Container>
{
public:
	Container(android_app* app);
	Container(Camera& camera, TextureManager& textureManager);
	~Container();

	void init(	float topRadius, float bottomRadius,
				float height,
				uint32_t numSlices, uint32_t numStacks);
	void update(float dt);
	void draw(ShadowMap& shadowMap);
	void drawToShadowMap(ShadowMap& shadowMap);
	void unload();

private:
	struct Vertex
	{
		Vertex() {};
		Vertex( float p_x,float p_y, float p_z,
				float n_x,float n_y, float n_z,
				float tex_u, float tex_v);
		Vertex(ndk_helper::Vec3& P, ndk_helper::Vec3& N, float tex_u, float tex_v);

	    float pos[3], normal[3], tex[2];
	};

	enum SHADER_ATTR { ATTR_VERTEX, ATTR_NORMAL, ATTR_TEXTURE };
	struct ShaderParams { GLuint program, MVP, M, lightViewProj, lightColor, eyePos, sampler, lightSampler; };

	void initBuffers(Vertex* verticesArr, short* indicesArr);
	void initTextures();
	void initShaders();
	void initCaps();
	void initCyllinder();

	void bindShaderAttributes(GLuint program);

	void buildBottomCap(std::vector<Vertex>& vertices, std::vector<short>& indices);
	void buildTopCap(std::vector<Vertex>& vertices, std::vector<short>& indices);
	void buildStacks(std::vector<Vertex>& vertices, std::vector<short>& indices);

	uint32_t mNumVertices,
			 mNumFaces;

	GLuint  mIbo,
			mVbo;

	float mTopRadius,
		  mBottomRadius,
		  mHeight;

	uint32_t mNumSlices,
			 mNumStacks;

	Texture* mTexture;
	TextureManager& mTextureManager;
	Camera& mCamera;
	ShaderParams mShaderParams;
	ndk_helper::Mat4 mM;

};

#endif
