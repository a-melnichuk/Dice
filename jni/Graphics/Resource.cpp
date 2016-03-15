#include "Resource.h"

Resource::Resource(android_app* app, const char* path):
	mPath(path),
	mAssetManager(app->activity->assetManager),
	mAsset(NULL)
{}

int Resource::open() {
	mAsset = AAssetManager_open(mAssetManager, mPath,
								AASSET_MODE_UNKNOWN);
	return (mAsset != NULL) ? 1 : 0;
}

void Resource::close() {
	if (mAsset != NULL) {
		AAsset_close(mAsset);
		mAsset = NULL;
	}
}

int Resource::read(void* buffer, size_t count) {
	int32_t readCount = AAsset_read(mAsset, buffer, count);
	return (readCount == count) ? 1 : 0;
}

const char* Resource::getPath() {
	return mPath;
}
