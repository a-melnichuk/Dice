#ifndef _DICE_CUBE_MAP_
#define _DICE_CUBE_MAP_

#define NUM_SIDES 6

#include <android_native_app_glue.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <png.h>

#include "Texture.h"



class CubeMap
{
public:
	CubeMap(Texture* posXTex, Texture* negXTex,
			Texture* posYTex, Texture* negYTex,
			Texture* posZTex, Texture* negZTex);
	int load();
	void apply();
private:
	GLuint mTextureId;
	Texture** mTextures;
};



#endif
