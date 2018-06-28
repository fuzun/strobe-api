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


#ifndef STROBE_DISABLED

#include "strobe-core.h"

#include <cstring>
#include <cstdlib>
#include <chrono>

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

using namespace std;

Strobe_Core::Strobe_Core(int strobe_method, int strobe_cooldownDelay, int strobe_swapInterval, double deviation_limit) : deviationLimit(deviation_limit)
{
	recentTime = 0;
	recentTime2 = 0;
	memset(delta, 0, sizeof(delta));
	fCounterSnapshot = 0;
	offsetX = 0;
	nexttime = 0;
	lasttime = 0;
	strobeInterval = 0;
	swapInterval = 0;
	initialTime = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count();
	active = true;

	strobeMethod = strobe_method;
	cooldownDelay = strobe_cooldownDelay;
	swapInterval = strobe_swapInterval;

    strobeInterval = strobeMethod;

}

void Strobe_Core::setMethod(int strobe_method)
{
	strobeMethod = strobe_method;
	strobeInterval = strobeMethod;
}

double Strobe_Core::FPS(void)
{
	return fps;
}

void Strobe_Core::setFPS(double newFPS)
{
	fps = newFPS;
}

bool Strobe_Core::strobe()
{
	int delta1;
	double delta2;
	int currentTime = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count();
	delta2 = (currentTime - recentTime2) / 1000.0;
	recentTime2 = currentTime;
	elapsedTime = currentTime - initialTime;

	if (cdTimer >= 0.0 && delta2 > 0.0)
		cdTimer += delta2;
	if (fCounter - fCounterSnapshot == 1)
	{
		delta[fCounter % ARRAYSIZE(delta)] = FPS();
		deviation = StandardDeviation(delta, ARRAYSIZE(delta));
	}
	fCounterSnapshot = fCounter;

	if (cooldownDelay > 0)
	{
		if ((cdTimer > (double)abs(cooldownDelay)) && cdTriggered == true)
		{
			cdTriggered = false;
			cdTimer = -1.0;
		}

        if (fCounter > (int)ARRAYSIZE(delta))
		{
			if (deviation > deviationLimit)
			{
				cdTriggered = true;
				cdTimer = 0.0;
			}
		}
	}
	else
	{
		cdTriggered = false;
	}

	if (fCounter == INT_MAX)
	{
		Strobe_Core(strobeMethod, cooldownDelay, swapInterval, deviationLimit);
		strobe();
	}


	if (strobeInterval == 0)
	{
		if (strobeInterval)
		{
			active = false;
		}
		fCounter = 0;

		return true;
	}

	if ((fCounter % 2) == 0)
	{
		++pCounter;
		frameInfo = (Framestate)(frameInfo | PHASE_POSITIVE);
	}
	else
	{
		++nCounter;
		frameInfo = (Framestate)(frameInfo & ~PHASE_POSITIVE);
	}

	if (swapInterval < 0)
		swapInterval = abs(swapInterval);

	if ((swapInterval) && (strobeInterval % 2)) // Swapping not enabled for even intervals as it is neither necessary nor works as intended
	{
		delta1 = currentTime - recentTime; // New Currenttime for _delta1 ?
		if ((delta1 >= (swapInterval * 1000) && (delta1 < (2 * swapInterval * 1000)))) // Basic timer
		{
			frameInfo = (Framestate)(frameInfo | PHASE_INVERTED);
		}
		else if (delta1 < (swapInterval * 1000))
		{
			frameInfo = (Framestate)(frameInfo & ~PHASE_INVERTED);
		}
		else //if (delta1 >= (2 * swapInterval * 1000))
		{
			recentTime = currentTime;
		}
	}

	switch (frameInfo & (PHASE_POSITIVE | PHASE_INVERTED))
	{
	case (PHASE_POSITIVE | PHASE_INVERTED):
		if ((abs(strobeInterval) % 2) == 0)
			frameInfo = (Framestate)((((pCounter - 1) % (abs(strobeInterval) + 1)) == (abs(strobeInterval) / 2)) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //even
		else
			frameInfo = (Framestate)(frameInfo & ~FRAME_RENDER);
		break;

	case (PHASE_POSITIVE & ~PHASE_INVERTED):
		if (abs(strobeInterval) % 2 == 0)
			frameInfo = (Framestate)((((pCounter - 1) % (abs(strobeInterval) + 1)) == 0) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //even
		else
		{
			if (abs(strobeInterval) == 1)
				frameInfo = (Framestate)(frameInfo | FRAME_RENDER);
			else
				frameInfo = (Framestate)((((pCounter - 1) % ((abs(strobeInterval) + 1) / 2)) == 0) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //odd
		}
		break;

	case (~PHASE_POSITIVE & PHASE_INVERTED):
		if (abs(strobeInterval) % 2 == 0)
			frameInfo = (Framestate)((((nCounter - 1) % (abs(strobeInterval) + 1)) == 0) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //even
		else
		{
			if (abs(strobeInterval) == 1)
				frameInfo = (Framestate)(frameInfo | FRAME_RENDER);
			else
				frameInfo = (Framestate)((((nCounter - 1) % ((abs(strobeInterval) + 1) / 2)) == 0) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //odd
		}
		break;

	case 0:
		if ((abs(strobeInterval) % 2) == 0)
			frameInfo = (Framestate)((((nCounter - 1) % (abs(strobeInterval) + 1)) == (abs(strobeInterval) / 2)) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //even
		else
			frameInfo = (Framestate)(frameInfo & ~FRAME_RENDER);
		break;

	default:
		frameInfo = (Framestate)(PHASE_POSITIVE | FRAME_RENDER);
	}

	if (strobeInterval < 0)
		frameInfo = (Framestate)(frameInfo ^ FRAME_RENDER);

	return ProcessFrame();
}

void Strobe_Core::debugHandler()
{
	GenerateDebugStatistics(debugStr, ARRAYSIZE(debugStr));
}

#endif
