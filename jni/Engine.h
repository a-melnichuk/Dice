#ifndef _DICE_ENGINE_
#define _DICE_ENGINE_

#include <jni.h>
#include <errno.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <cpu-features.h>

#include "Logs.h"
#include "Timer.h"
#include "Graphics/TextureManager.h"
#include "Physics/CollisionManager.h"
#include "Graphics/Dice.h"
#include "Graphics/Container.h"
#include "Graphics/ShadowMap.h"
#include "Camera.h"
#include "NDKHelper.h"
#include "Graphics/Quad.h"

class Engine
{
public:
    Engine();
    ~Engine();

    static void handleCmd( struct android_app* app, int32_t cmd );
    static int32_t handleInput( android_app* app, AInputEvent* event );

    void setState( android_app* state );

    int initDisplay();
    void loadResources();
    void unloadResources();
    void drawFrame();
    void termDisplay();
    void trimMemory();
    bool isReady();

    void initSensors();
    void processSensors( int32_t id );
    void suspendSensors();
    void resumeSensors();

    void setPickingRay(int32_t x, int32_t y);

private:
    Dice mDice;
    Container mContainer;
    ShadowMap mShadowMap;
    Timer mTimer;
    Quad mQuad;
    Camera mCamera;
    TextureManager mTextureManager;
    CollisionManager mCollisionManager;
    ndk_helper::GLContext* mGlContext;

    bool mInitializedResources;
    bool mHasFocus;
    int32_t mScreenWidth,
    		mScreenHeight;
    android_app* mApp;

    ASensorManager* mSensorManager;
    const ASensor* mAccelerometerSensor;
    ASensorEventQueue* mSensorEventQueue;
};

#endif
