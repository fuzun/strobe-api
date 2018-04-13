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

#include <cstdlib>
#include <chrono>

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

double randfrom(double min, double max)
{
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

void strobe_core::showBlack(void)
{
	return; // MUST BE IMPLEMENTED!!!
}

void strobe_core::showNormal(void)
{
	return; // MUST BE IMPLEMENTED!!!
}

double strobe_core::currentFPS(void)
{
	return randfrom(99, 101); // MUST BE IMPLEMENTED!!!
}

int strobe_core::getSTROBE(void)
{
	return 2; // 1 : NORMAL - BLACK - NORMAL - BLACK ... , 2 : NORMAL - BLACK - BLACK - NORMAL - BLACK - BLACK ... , -2 : BLACK - BLACK - NORMAL - BLACK - BLACK - NORMAL ...
}

int strobe_core::getSTROBE_COOLDOWN(void)
{
	return 2; // x seconds wait if the frame rate is inconsistent
}

int strobe_core::getSWAPINTERVAL()
{
	return 3; // x seconds interval for phase switching
}

void strobe_core::strobe()
{
	__int64 delta1;
	double delta2;
	__int64 currentTime = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count();
	delta2 = (currentTime - recentTime2) / 1000.0;
	recentTime2 = currentTime;
	elapsedTime = currentTime - initialTime;

	if (cdTimer >= 0.0 && delta2 > 0.0)
		cdTimer += delta2;
	if (fCounter - fCounterSnapshot == 1)
	{
		delta[fCounter % ARRAYSIZE(delta)] = currentFPS(); //delta2;
		deviation = StandardDeviation(delta, ARRAYSIZE(delta)); //* 1000;
	}
	fCounterSnapshot = fCounter;

	if (getSTROBE_COOLDOWN() > 0)
	{
		if ((cdTimer > (double)abs(getSTROBE_COOLDOWN())) && cdTriggered == true)
		{
			cdTriggered = false;
			cdTimer = -1.0;
		}

		if (fCounter > ARRAYSIZE(delta))
		{
			if (deviation > DEVIATION_LIMIT)
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

	if (((strobeInterval != getSTROBE()) && (strobeInterval)) ||
		fCounter == UINT_MAX)
	{
		strobe_core();
		strobe();
	}

	strobeInterval = getSTROBE();
	swapInterval = getSWAPINTERVAL();

	if (strobeInterval == 0)
	{
		if (strobeInterval)
		{
			active = false;
		}
		fCounter = 0;

		showNormal();
		return;
	}

	if ((fCounter % 2) == 0)
	{
		++pCounter;
		frameInfo = (fstate_e)(frameInfo | PHASE_POSITIVE);
	}
	else
	{
		++nCounter;
		frameInfo = (fstate_e)(frameInfo & ~PHASE_POSITIVE);
	}

	if (swapInterval < 0)
		swapInterval = abs(swapInterval);

	if ((swapInterval) && (strobeInterval % 2)) // Swapping not enabled for even intervals as it is neither necessary nor works as intended
	{
		delta1 = currentTime - recentTime;                                                                       // New Currenttime for _delta1 ?
		if ((delta1 >= (swapInterval * 1000) && (delta1 < (2 * swapInterval * 1000)))) // Basic timer
		{
			frameInfo = (fstate_e)(frameInfo | PHASE_INVERTED);
		}
		else if (delta1 < (swapInterval * 1000))
		{
			frameInfo = (fstate_e)(frameInfo & ~PHASE_INVERTED);
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
			frameInfo = (fstate_e)((((pCounter - 1) % (abs(strobeInterval) + 1)) == (abs(strobeInterval) / 2)) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //even
		else
			frameInfo = (fstate_e)(frameInfo & ~FRAME_RENDER);
		break;

	case (PHASE_POSITIVE & ~PHASE_INVERTED):
		if (abs(strobeInterval) % 2 == 0)
			frameInfo = (fstate_e)((((pCounter - 1) % (abs(strobeInterval) + 1)) == 0) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //even
		else
		{
			if (abs(strobeInterval) == 1)
				frameInfo = (fstate_e)(frameInfo | FRAME_RENDER);
			else
				frameInfo = (fstate_e)((((pCounter - 1) % ((abs(strobeInterval) + 1) / 2)) == 0) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //odd
		}
		break;

	case (~PHASE_POSITIVE & PHASE_INVERTED):
		if (abs(strobeInterval) % 2 == 0)
			frameInfo = (fstate_e)((((nCounter - 1) % (abs(strobeInterval) + 1)) == 0) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //even
		else
		{
			if (abs(strobeInterval) == 1)
				frameInfo = (fstate_e)(frameInfo | FRAME_RENDER);
			else
				frameInfo = (fstate_e)((((nCounter - 1) % ((abs(strobeInterval) + 1) / 2)) == 0) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //odd
		}
		break;

	case 0:
		if ((abs(strobeInterval) % 2) == 0)
			frameInfo = (fstate_e)((((nCounter - 1) % (abs(strobeInterval) + 1)) == (abs(strobeInterval) / 2)) ? frameInfo | FRAME_RENDER : frameInfo & ~FRAME_RENDER); //even
		else
			frameInfo = (fstate_e)(frameInfo & ~FRAME_RENDER);
		break;

	default:
		frameInfo = (fstate_e)(PHASE_POSITIVE | FRAME_RENDER);
	}

	if (strobeInterval < 0)
		frameInfo = (fstate_e)(frameInfo ^ FRAME_RENDER);

	ProcessFrame();
}

void strobe_core::debugHandler()
{
	GenerateDebugStatistics(debugStr, ARRAYSIZE(debugStr));
	printf("%s", debugStr);
}

strobe_core::strobe_core()
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
	initialTime = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // !!!
	active = true;
}

#endif