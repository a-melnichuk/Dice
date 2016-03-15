#ifndef _DICE_SHAPE_
#define _DICE_SHAPE_


#include <jni.h>
#include <errno.h>
#include <vector>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <cpu-features.h>
#include "NDKHelper.h"
#include "TextureManager.h"
#include "Camera.h"
#include "../Logs.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

template <typename T>
class Shape
{
public:
	Shape(){}
	Shape(android_app* app);
	virtual void setAssetManager(AAssetManager* assetManager);
protected:

	//app->activity->assetManager
	virtual GLuint loadShaders(const char* pVertexSource, const char* pFragmentSource);
	virtual void checkGlError(const char* op);
	virtual void bindShaderAttributes(GLuint program) = 0;


private:
	GLuint loadShader(GLenum shaderType, const char* pSource);
	char* readShaderAsset(const char* path);
	android_app* mApp;
	AAssetManager* mAssetManager;
};


template <typename T>
Shape<T>::Shape(android_app* app): mApp(app)
{

}

template <typename T>
void Shape<T>::setAssetManager(AAssetManager* assetManager)
{
	mAssetManager = assetManager;
}

template <typename T>
char* Shape<T>::readShaderAsset(const char* path)
{

	AAsset* asset = AAssetManager_open(mAssetManager, path, AASSET_MODE_BUFFER);
	if (NULL == asset) {
		LOGW("ASSET NOT FOUND");
	//__android_log_print(ANDROID_LOG_ERROR, NF_LOG_TAG, "_ASSET_NOT_FOUND_");
		return JNI_FALSE;
	}
	size_t size = AAsset_getLength(asset);
	char* buffer = (char*) malloc (sizeof(char)*size + 1);
	AAsset_read (asset,buffer,size);
	//__android_log_print(ANDROID_LOG_ERROR, NF_LOG_TAG, buffer);
	AAsset_close(asset);
	buffer[size] = 0;
	//LOGI("reading asset p: %s size: %i len: %i buff: %s",path, size, strlen(buffer), buffer );
	return buffer;
}

template <typename T>
GLuint Shape<T>::loadShader(GLenum shaderType, const char* pSource)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader)
	{
		glShaderSource(shader, 1, &pSource, NULL);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen)
			{
				char* buf = (char*) malloc(infoLen);
				if (buf)
				{
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGE("Could not compile shader %d:\n%s\n",shaderType, buf);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

template <typename T>
GLuint Shape<T>::loadShaders(const char* pVertexSource, const char* pFragmentSource) {

	char *vertexShaderSrc = readShaderAsset(pVertexSource);
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSrc );
    if (!vertexShader)
    {
    	free(vertexShaderSrc);
        return 0;
    }
    char *fragmentShaderSrc = readShaderAsset(pFragmentSource);
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSrc );
    if (!pixelShader)
    {
    	free(vertexShaderSrc);
    	free(fragmentShaderSrc);
        return 0;
    }

    GLuint program = glCreateProgram();

    if (program)
    {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");

        bindShaderAttributes(program);

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength)
            {
                char* buf = (char*) malloc(bufLength);
                if (buf)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    if (!program)
    {
        LOGE("0x Could not create program.");
    }
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    return program;
}

template <typename T>
void Shape<T>::checkGlError(const char* op)
{
    for (GLint error = glGetError(); error; error = glGetError())
    {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}



#endif
