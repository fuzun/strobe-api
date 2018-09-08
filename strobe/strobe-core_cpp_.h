/*
MIT License

Copyright (c) 2018 fuzun

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cstring>
#include <cstdlib>
#include <chrono>

inline StrobeCore::StrobeCore(int mode, int phaseSwitchInterval) : StrobeAPI(mode, phaseSwitchInterval)
{
	fps = 0;
	timerSnapshot = 0;
	modeSnapshot = mode;

	active = true;
}

inline StrobeCore::~StrobeCore()
{

}

inline bool StrobeCore::isActive(void)
{
	return active;
}

inline void StrobeCore::disable(void)
{
	active = false;
	modeSnapshot = strobeMode;
	setMode(0);
}

inline void StrobeCore::enable(void)
{
	active = true;
	strobeMode = modeSnapshot;
}

inline int StrobeCore::getStrobeMode(void)
{
	if (modeSnapshot != strobeMode)
		return modeSnapshot;
	else
		return strobeMode;
}

inline void StrobeCore::setActive(bool status)
{
	active = status;
	if (active)
		enable();
	else
		disable();
}

inline void StrobeCore::setMode(int mode)
{
	strobeMode = mode;
}

inline double StrobeCore::FPS(void)
{
	return fps;
}

inline void StrobeCore::setFPS(double newFPS)
{
	fps = newFPS;
}

inline bool StrobeCore::strobe(void)
{
	bool phasePositive = !(counter.total() % 2);

	int pCounter = counter.totalPositive();
	int nCounter = counter.totalNegative();

	if (phasePositive)
	{
		++pCounter;
		frameState = (FrameState)(frameState | PHASE_POSITIVE);
	}
	else
	{
		++nCounter;
		frameState = (FrameState)(frameState & ~PHASE_POSITIVE);
	}

	if (!strobeMode) // if strobe mode == 0
	{
		// disable();

		if (phasePositive)
			++counter.positiveRenderedFrameCount;
		else
			++counter.negativeRenderedFrameCount;

		return true;
	}

	if (switchInterval < 0)
		switchInterval = abs(switchInterval);

	if ((switchInterval) && (strobeMode % 2)) // Swapping not enabled for even modes as it is neither necessary nor works as intended
	{
		double difference = strobeTimer.elapsedSeconds() - timerSnapshot;

		if (difference >= switchInterval)
		{
			frameState = (FrameState)(frameState ^ PHASE_INVERTED);
			timerSnapshot = strobeTimer.elapsedSeconds();
		}
	}


	switch (frameState & (PHASE_POSITIVE | PHASE_INVERTED))
	{
	case (PHASE_POSITIVE | PHASE_INVERTED):
		if ((abs(strobeMode) % 2) == 0)
			frameState = (FrameState)((((pCounter - 1) % (abs(strobeMode) + 1)) == (abs(strobeMode) / 2)) ? frameState | FRAME_RENDER : frameState & ~FRAME_RENDER);
		else
			frameState = (FrameState)(frameState & ~FRAME_RENDER);
		break;

	case (PHASE_POSITIVE & ~PHASE_INVERTED):
		if (abs(strobeMode) % 2 == 0)
			frameState = (FrameState)((((pCounter - 1) % (abs(strobeMode) + 1)) == 0) ? frameState | FRAME_RENDER : frameState & ~FRAME_RENDER);
		else
			if (abs(strobeMode) == 1)
				frameState = (FrameState)(frameState | FRAME_RENDER);
			else
				frameState = (FrameState)((((pCounter - 1) % ((abs(strobeMode) + 1) / 2)) == 0) ? frameState | FRAME_RENDER : frameState & ~FRAME_RENDER);
		break;

	case (~PHASE_POSITIVE & PHASE_INVERTED):
		if (abs(strobeMode) % 2 == 0)
			frameState = (FrameState)((((nCounter - 1) % (abs(strobeMode) + 1)) == 0) ? frameState | FRAME_RENDER : frameState & ~FRAME_RENDER);
		else
			if (abs(strobeMode) == 1)
				frameState = (FrameState)(frameState | FRAME_RENDER);
			else
				frameState = (FrameState)((((nCounter - 1) % ((abs(strobeMode) + 1) / 2)) == 0) ? frameState | FRAME_RENDER : frameState & ~FRAME_RENDER);
		break;

	case 0:
		if ((abs(strobeMode) % 2) == 0)
			frameState = (FrameState)((((nCounter - 1) % (abs(strobeMode) + 1)) == (abs(strobeMode) / 2)) ? frameState | FRAME_RENDER : frameState & ~FRAME_RENDER);
		else
			frameState = (FrameState)(frameState & ~FRAME_RENDER);
		break;

	default:
		frameState = (FrameState)(PHASE_POSITIVE | FRAME_RENDER);
	}

	if (strobeMode < 0)
		frameState = (FrameState)(frameState ^ FRAME_RENDER);

	
	return processFrame();
}
