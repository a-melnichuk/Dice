#include "Timer.h"

#include <cstdlib>
#include <ctime>

Timer::Timer(): mFirstTime(0.0f), mPrevTime(0.0f), mElapsedTotal(0.0f), mStopTime(0.0f), mPausedTime(0.0f)
{
	mFirstTime = now();
	mPrevTime = mFirstTime;
}

void Timer::reset()
{
    mStopTime = mPausedTime = 0.0f;
    mFirstTime = mPrevTime = now();
}

float Timer::tick()
{
    float timePassed =  now();
    float dt = timePassed - mPrevTime;
    mElapsedTotal = timePassed - mFirstTime;
    mPrevTime = timePassed;

    LOGI("time passed: %f prev: %f passed: %f dt: %f",timePassed, mPrevTime, mPausedTime, dt);

    return dt < 0 ? 0 : dt;
}

float Timer::now()
{
    timespec timeVal;
    clock_gettime(CLOCK_MONOTONIC, &timeVal);
    return timeVal.tv_sec + (timeVal.tv_nsec * 1.0e-9);
}
