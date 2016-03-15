#include "TextureManager.h"
#include "Resource.h"
#include "CubeMap.h"

#include <GLES/gl.h>
#include <GLES/glext.h>

TextureManager::TextureManager()
{
}

void TextureManager::init(android_app* pApplication)
{
	mApplication = pApplication;
	mTextureCount = 0;
}

TextureManager::~TextureManager()
{

	// Deletes all textures.z
	for (int32_t i = 0; i < mTextureCount; ++i)
	{
		delete mTextures[i];
		mTextures[i] = NULL;
	}
	mTextureCount = 0;
}

int TextureManager::loadResources()
{
	// Loads all requested textures.
	for (int32_t i = 0; i < mTextureCount; ++i)
	{
		if (mTextures[i]->load() != 1)
		{
			return 0;
		}
	}
	return 1;
}

int TextureManager::unloadResources()
{
	for (int32_t i = 0; i < mTextureCount; ++i)
	{
		mTextures[i]->unload();
	}
	return 1;
}

Texture* TextureManager::registerTexture( const char* pPath)
{
	// Finds out if texture already loaded.
	for (int32_t i = 0; i < mTextureCount; ++i) {
		if (strcmp(pPath, mTextures[i]->getPath()) == 0)
		{
			return mTextures[i];
		}
	}

	// Appends a new texture to the texture array.
	Texture* lTexture = new Texture(mApplication, pPath);
	mTextures[mTextureCount++] = lTexture;
	return lTexture;
}

CubeMap* TextureManager::registerCubeMap(const char* posXPath, const char* negXPath,
										  const char* posYPath, const char* negYPath,
										  const char* posZPath, const char* negZPath)
{
	CubeMap * cubeMap = new CubeMap(registerTexture(posXPath),registerTexture(negXPath),
									registerTexture(posYPath),registerTexture(negYPath),
									registerTexture(posZPath),registerTexture(negZPath) );
	return cubeMap;
}


