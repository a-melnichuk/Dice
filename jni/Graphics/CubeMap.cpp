#include "CubeMap.h"

CubeMap::CubeMap( Texture* posXTex, Texture* negXTex,
				  Texture* posYTex, Texture* negYTex,
				  Texture* posZTex, Texture* negZTex)
{
	mTextures = new Texture *[NUM_SIDES];

	mTextures[0] = posXTex;
	mTextures[1] = negXTex;
	mTextures[2] = posYTex;
	mTextures[3] = negYTex;
	mTextures[4] = posZTex;
	mTextures[5] = negZTex;

}

int CubeMap::load()
{
	GLenum lErrorResult;
	glGenTextures(1, &mTextureId);
	// Bind the texture object
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);

	for(size_t i=0;i<NUM_SIDES;++i)
	{
		uint8_t* pixels = mTextures[i]->loadImage();
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0,
		mTextures[i]->mFormat, mTextures[i]->mWidth,  mTextures[i]->mHeight,
		0, mTextures[i]->mFormat, GL_UNSIGNED_BYTE, pixels);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	if (glGetError() != GL_NO_ERROR) {
		LOGW("GL_ERR");
		for(size_t i=0;i<NUM_SIDES;++i)
		{
			delete[] mTextures[i]->mPixelsPtr;
			mTextures[i]->unload();
		}
		return 0;
	}

	return 1;
}

void CubeMap::apply()
{
	glActiveTexture( GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);
}

