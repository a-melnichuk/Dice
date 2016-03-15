#ifndef _DICE_TEXTURE_MANAGER_
#define _DICE_TEXTURE_MANAGER_

#include <android_native_app_glue.h>
#include "Texture.h"
#include "CubeMap.h"

class TextureManager {
public:
	TextureManager();
	~TextureManager();

	Texture* registerTexture(const char* pPath);
	CubeMap* registerCubeMap(const char* posXPath, const char* negXPath,
							 const char* posYPath, const char* negYPath,
							 const char* posZPath, const char* negZPath);
	void init(android_app* app);
	int loadResources();
	int unloadResources();

private:
	android_app* mApplication;
	Texture* mTextures[32];
	int32_t mTextureCount;
};

#endif
