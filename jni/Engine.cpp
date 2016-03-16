#include "Engine.h"

Engine::Engine() :
                mInitializedResources( false ),
                mHasFocus( false ),
                mApp( NULL ),
                mSensorManager( NULL ),
                mAccelerometerSensor( NULL ),
                mSensorEventQueue( NULL ),
                mScreenWidth(0),
                mScreenHeight(0),
                mContainer(mCamera, mTextureManager),
                mDice(mCamera, mTextureManager),
                mGlContext(ndk_helper::GLContext::GetInstance()),
                mCamera(ndk_helper::Vec3(0.0f, 0.0f, 1.3f),
                		ndk_helper::Vec3(0.0f, 0.0f, 0.0f),
                		ndk_helper::Vec3(0.0f, 1.0f, 0.0f))
{
}

Engine::~Engine()
{
}

/**
 * Load resources
 */
void Engine::loadResources()
{
	mCamera.initProj();
	mCamera.initView();
	mCamera.update();

	ndk_helper::Vec3 origin(0.0f, 0.0f, 0.0f);
	ndk_helper::Vec3 halfSize(0.25f, 0.25f, 0.25f);

	RigidBody *dice = new RigidBody(origin, 50.0f, 0.15f, 0.45f, 1.0f, 0.6f);
	Cube *diceCube = new Cube(halfSize, dice);

	mCollisionManager.add(diceCube);

	ndk_helper::Vec3 camPos = mCamera.mPos + mCamera.mNormal*(mCamera.mNear + 0.1f);

	mCollisionManager.add(new Plane(mCamera.mNormal, camPos));

    mDice.init(dice, halfSize);

    float topRadius = 1.0f;
    float bottomRadius = 1.0f;
    float height = 2.0f;

    mCollisionManager.add(new Cylinder(topRadius, height));

    mContainer.init(topRadius, bottomRadius, height, 25, 15);
    ndk_helper::Vec3 lookAt(0.0f,-0.1f,0.0f);
    ndk_helper::Vec3 pos(-0.9f,0.0f,0.9f);
    mShadowMap.init(pos, lookAt ,mCamera);
    mQuad.init();
}

/**
 * Unload resources
 */
void Engine::unloadResources()
{
    mDice.unload();
	mContainer.unload();
	mTextureManager.unloadResources();
	mCollisionManager.unload();
	mQuad.unload();
}

/**
 * Initialize an EGL context for the current display.
 */
int Engine::initDisplay()
{
    if( !mInitializedResources )
    {
    	LOGI("INIT_RESOURCES");
        mGlContext->Init( mApp->window );
        loadResources();
        mInitializedResources = true;
    }
    else
    {
    	LOGI("RESOURCES_ALREADY_INITIALIZED");
        // initialize OpenGL ES and EGL
        if( EGL_SUCCESS != mGlContext->Resume( mApp->window ) )
        {
        	LOGI("RESUME_CONTEXT");
            unloadResources();
            loadResources();
        }
    }

    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

    mScreenWidth = mGlContext->GetScreenWidth();
    mScreenHeight = mGlContext->GetScreenHeight();

    glViewport( 0, 0, mScreenWidth, mScreenHeight );

    mCamera.initView();
    mCamera.update();

    return 0;
}

/**
 * Just the current frame in the display.
 */
void Engine::drawFrame()
{
	if(!mHasFocus) return;
	float dt = mTimer.tick();

	mCollisionManager.update(dt);
	mContainer.update(dt);
    mDice.update( dt );

    glViewport(0, 0, mShadowMap.SHADOW_WIDTH, mShadowMap.SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowMap.mDepthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    mDice.drawToShadowMap(mShadowMap);
    mContainer.drawToShadowMap(mShadowMap);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
    glViewport(0, 0, mScreenWidth, mScreenHeight);

    // Just fill the screen with a color.
    glClearColor( 0.5f, 0.5f, 0.5f, 1.f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    mDice.draw(mShadowMap);
    mContainer.draw(mShadowMap);
    mQuad.draw(mShadowMap);

    // Swap
    if( EGL_SUCCESS != mGlContext->Swap() )
    {
    	LOGI("RELOAD");
        unloadResources();
        loadResources();
    }
}

/**
 * Tear down the EGL context currently associated with the display.
 */
void Engine::termDisplay()
{
    mGlContext->Suspend();
}

void Engine::trimMemory()
{
    LOGI( "Trimming memory" );
    mGlContext->Invalidate();
}
/**
 * Process the next input event.
 */
int32_t Engine::handleInput( android_app* app, AInputEvent* event )
{
    Engine* eng = (Engine*) app->userData;
    if( AInputEvent_getType( event ) == AINPUT_EVENT_TYPE_MOTION )
    {
    	int x = AMotionEvent_getX(event, 0);
    	int y = AMotionEvent_getY(event, 0);

    	eng->mCollisionManager.mPick = true;
    	eng->setPickingRay(x, y);
    	return 1;
    }
    else if (AKeyEvent_getKeyCode(event) == AKEYCODE_BACK)
    {

	   eng->mTimer.setStopped();
	   eng->mHasFocus = false;
	   return 0;
    };


    return 0;
}

/**
 * Process the next main command.
 */
void Engine::handleCmd( struct android_app* app, int32_t cmd )
{
    Engine* eng = (Engine*) app->userData;
    switch( cmd )
    {
    case APP_CMD_INIT_WINDOW:
        if( app->window != NULL )
        {
            eng->initDisplay();
            eng->mTimer.updatePausedTime();
            eng->drawFrame();
        }
        break;
    case APP_CMD_TERM_WINDOW:
        eng->termDisplay();
        eng->mHasFocus = false;
        break;
    case APP_CMD_PAUSE:
    	 eng->mTimer.setStopped();
    	 eng->mHasFocus = false;
    	break;
    case APP_CMD_STOP:
    	eng->mTimer.setStopped();
    	eng->mHasFocus = false;
        break;
    case APP_CMD_GAINED_FOCUS:
		eng->resumeSensors();
		eng->mTimer.updatePausedTime();
		eng->mHasFocus = true;
		break;
    case APP_CMD_LOST_FOCUS:
        eng->suspendSensors();
        eng->mHasFocus = false;
        eng->mTimer.setStopped();
        break;
    case APP_CMD_LOW_MEMORY:
        eng->trimMemory();
        break;
    }
}

void Engine::initSensors()
{
    mSensorManager = ASensorManager_getInstance();
    mAccelerometerSensor = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_ACCELEROMETER );
    mSensorEventQueue = ASensorManager_createEventQueue( mSensorManager, mApp->looper, LOOPER_ID_USER, NULL, NULL );
}

void Engine::processSensors( int32_t id )
{
    // If a sensor has data, process it now.
    if( id == LOOPER_ID_USER )
    {
        if( mAccelerometerSensor != NULL )
        {
            ASensorEvent event;
            while( ASensorEventQueue_getEvents( mSensorEventQueue, &event, 1 ) > 0 )
            {
            	//mCollisionManager.mAccelerometerAcceleration = ndk_helper::Vec3(0.0f,-9.8f, 0.0f);
            	mCollisionManager.mAccelerometerAcceleration = ndk_helper::Vec3(-event.acceleration.x, -event.acceleration.y, -event.acceleration.z);
            }
        }
    }
}

void Engine::resumeSensors()
{
    // When our app gains focus, we start monitoring the accelerometer.
    if( mAccelerometerSensor != NULL )
    {
        ASensorEventQueue_enableSensor( mSensorEventQueue, mAccelerometerSensor );
        ASensorEventQueue_setEventRate( mSensorEventQueue, mAccelerometerSensor, (1000L / 30) * 1000 );
    }
}

void Engine::suspendSensors()
{
    // When our app loses focus, we stop monitoring the accelerometer.
    // This is to avoid consuming battery while not being used.
    if( mAccelerometerSensor != NULL )
    {
        ASensorEventQueue_disableSensor( mSensorEventQueue, mAccelerometerSensor );
    }
}

void Engine::setState( android_app* state )
{
    mApp = state;
    mTextureManager.init(mApp);
    mDice.setAssetManager(mApp->activity->assetManager);
    mContainer.setAssetManager(mApp->activity->assetManager);
    mShadowMap.setAssetManager(mApp->activity->assetManager);
    mQuad.setAssetManager(mApp->activity->assetManager);

}

bool Engine::isReady()
{
    if( mHasFocus )
        return true;

    return false;
}


void Engine::setPickingRay(int32_t x, int32_t y)
{
	float xNdc =  2.0f * ( (float) x/mScreenWidth) - 1.0f;
	float yNdc =  1.0f - 2.0f * ( (float) y/mScreenHeight);

	ndk_helper::Vec4 dNdc(xNdc, yNdc, 0.0f, 1.0f);

	dNdc = mCamera.mViewProjInverse * dNdc;

	ndk_helper::Vec3 dWorld(dNdc.x_, dNdc.y_, dNdc.z_);
	dWorld -= mCamera.mPos;
	dWorld.Normalize();

	mCollisionManager.mPickRay = Ray(dWorld, mCamera.mPos);
}
