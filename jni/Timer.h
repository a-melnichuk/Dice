#ifndef _DICE_TIMER_
#define _DICE_TIMER_

#include "Logs.h"

class Timer
{
public:
	Timer();

    void reset();
    float tick();

    float now();
    float __inline__ getStopped() { return mStopTime; }
    float __inline__ elapsedTotal() { return mElapsedTotal; }
    void __inline__ setStopped() { mPrevTime = now(); }
    void __inline__ updatePausedTime() { mPrevTime += now() - mStopTime; }


private:
    Timer(const Timer&);
    void operator=(const Timer&);

    float	mFirstTime,
			mStopTime,
			mPrevTime,
			mPausedTime,
			mElapsedTotal;
};
#endif
