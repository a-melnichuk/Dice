#include "Engine.h"

Engine g_engine;

void android_main( android_app* state )
{
    app_dummy();

    g_engine.setState( state );

    state->userData = &g_engine;
    state->onAppCmd = Engine::handleCmd;
    state->onInputEvent = Engine::handleInput;

    g_engine.initSensors();

    while( 1 )
    {
        int id;
        int events;
        android_poll_source* source;

        while( (id = ALooper_pollAll( g_engine.isReady() ? 0 : -1, NULL, &events, (void**) &source ))
                >= 0 )
        {
            if( source != NULL )
                source->process( state, source );

            g_engine.processSensors( id );

            if( state->destroyRequested != 0 )
            {
                g_engine.termDisplay();
                return;
            }
        }

        if( g_engine.isReady() )
        {
            g_engine.drawFrame();
        }
    }
}
