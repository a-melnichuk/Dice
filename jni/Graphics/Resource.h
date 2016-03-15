#ifndef _DICE_RESOURCE_
#define _DICE_RESOURCE_

#include <android_native_app_glue.h>

class Resource {
public:
	Resource(android_app* app, const char* path);

	int open();
	void close();
	int read(void* buffer, size_t count);

	const char* getPath();

private:
	const char* mPath;
	AAssetManager* mAssetManager;
	AAsset* mAsset;
};
#endif
