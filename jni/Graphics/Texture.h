#ifndef _DICE_TEXTURE_
#define _DICE_TEXTURE_

#include <android_native_app_glue.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <png.h>

#include "../Logs.h"
#include "Resource.h"

    class Texture {
    public:
        Texture(android_app* pApplication, const char* pPath);

        const char* getPath();
        int32_t getHeight();
        int32_t getWidth();
        uint8_t* loadImage();
        uint8_t* handleError(png_structp& pngPtr, png_bytep* rowPtrs, png_byte* imageBuffer,png_infop& infoPtr);
        int load();
        void unload();
        void apply();

        GLuint mTextureId;
        int32_t mWidth, mHeight;
        GLint mFormat;
        uint8_t* mPixelsPtr;

    private:
        static void callback_read(png_structp pngStruct,
            png_bytep data, png_size_t size);

    private:
        Resource mResource;


    };
#endif
