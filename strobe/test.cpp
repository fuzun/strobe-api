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

#include <cstdio>
#include <cassert>
#include <ctime>

#include "strobe-core.h"

class Timer // Simple timer. Gathered from the internet
{
public:
	inline Timer()
	{
		init();
	}

	inline void restart()
	{
		init();
	}

	inline double elapsedMilliseconds()
	{
		std::chrono::time_point<std::chrono::system_clock> endTime;
		endTime = std::chrono::system_clock::now();

		return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
	}

	inline double elapsedSeconds()
	{
		return elapsedMilliseconds() / 1000.0;
	}

private:
	inline void init()
	{
		m_StartTime = std::chrono::system_clock::now();
	}

	std::chrono::time_point<std::chrono::system_clock> m_StartTime;
};


double random(int min, int max)
{
	int min_ = min * 100;
	int max_ = max * 100;
	return (rand() % (max_ - min_ + 1) + min_) / 100.0;
}

struct StrobeOutput
{
	double eFPS;
	double fps_;
	int totalFrameCount;
	int positiveFrameCount;
	int negativeFrameCount;
	int positiveBlackFrameCount;
	int negativeBlackFrameCount;
	int positiveRenderedFrameCount;
	int negativeRenderedFrameCount;
	double frequency;
	double period;
	double brightnessReduction;
	double badness;
	double badnessPWM;
	double badnessReduced;
	double badnessReducedPwm;
};

void assign(StrobeOutput * const output, StrobeAPI * const strobe)
{
	output->eFPS                       = strobe->effectiveFPS();
	output->fps_                       = strobe->FPS();
	output->totalFrameCount            = strobe->frameCount(StrobeAPI::TotalFrame);
	output->positiveFrameCount         = strobe->frameCount(StrobeAPI::PositiveFrame);
	output->negativeFrameCount         = strobe->frameCount(StrobeAPI::NegativeFrame);
	output->positiveBlackFrameCount    = strobe->frameCount(StrobeAPI::PositiveBlackFrame);
	output->negativeBlackFrameCount    = strobe->frameCount(StrobeAPI::NegativeBlackFrame);
	output->positiveRenderedFrameCount = strobe->frameCount(StrobeAPI::PositiveRenderedFrame);
	output->negativeRenderedFrameCount = strobe->frameCount(StrobeAPI::NegativeRenderedFrame);
	output->frequency                  = strobe->frequency();
	output->period                     = strobe->period();
	output->brightnessReduction        = strobe->actualBrightnessReduction();
	output->badness                    = strobe->badness(false);
	output->badnessPWM                 = strobe->badness(true);
	output->badnessReduced             = strobe->badnessReduced(false);
	output->badnessReducedPwm          = strobe->badnessReduced(true);
}

int main()
{
	StrobeAPI *strobe;
	Timer timer;
	StrobeOutput output;
	double fps;

	srand((unsigned int)time(NULL));
	memset(&output, 0, sizeof(output));

	strobe = new StrobeCore(1, 1, false);
	while (timer.elapsedSeconds() < 10)
	{
		fps = random(58, 62);
		strobe->setFPS(fps);
		strobe->strobe();
		printf("%s", strobe->getDebugInformation());

		assign(&output, strobe);

		assert(strobe->getPhaseSwitchInterval() == 1);
		assert(strobe->getStrobeMode() == 1);
		assert(output.fps_ >= 58 && output.fps_ <= 62);
		assert(output.eFPS == output.fps_ / 2);
		assert(output.totalFrameCount == output.negativeFrameCount + output.positiveFrameCount);
		assert(output.positiveFrameCount == output.positiveBlackFrameCount + output.positiveRenderedFrameCount);
		assert(output.negativeFrameCount == output.negativeBlackFrameCount + output.negativeRenderedFrameCount);
		assert(output.frequency > 0);
		assert(fabs(output.frequency - output.eFPS) < 1);
		assert(fabs(output.frequency - 1000 / output.period) < 0.1);
		assert(output.brightnessReduction == (output.fps_ - output.eFPS) * 100.0 / output.fps_);
		assert(output.badnessPWM == output.badness * output.period);
		assert(output.badnessReducedPwm == output.badnessReduced * output.period);
	}
	fflush(stdout);
	delete strobe;
	
	strobe = new StrobeCore(2, 3, false);
	timer.restart();
	while (timer.elapsedSeconds() < 10)
	{
		fps = random(120, 144);
		strobe->setFPS(fps);
		strobe->strobe();
		printf("%s", strobe->getDebugInformation());

		assign(&output, strobe);

		assert(strobe->getPhaseSwitchInterval() == 3);
		assert(strobe->getStrobeMode() == 2);
		assert(output.fps_ >= 120 && output.fps_ <= 144);
		assert(output.eFPS == output.fps_ / 3);
		assert(output.totalFrameCount == output.negativeFrameCount + output.positiveFrameCount);
		assert(output.positiveFrameCount == output.positiveBlackFrameCount + output.positiveRenderedFrameCount);
		assert(output.negativeFrameCount == output.negativeBlackFrameCount + output.negativeRenderedFrameCount);
		assert(output.frequency > 0);
		assert(fabs(output.frequency - 1000 / output.period) < 0.1);
		assert(output.brightnessReduction == (output.fps_ - output.eFPS) * 100.0 / output.fps_);
		assert(output.badnessPWM == output.badness * output.period);
		assert(output.badnessReducedPwm == output.badnessReduced * output.period);
	}
	fflush(stdout);
	delete strobe;

	strobe = new StrobeCore(0, 0, false);
	timer.restart();
	while (timer.elapsedSeconds() < 10)
	{
		fps = random(29, 31);
		strobe->setFPS(fps);
		bool result = strobe->strobe();
		printf("%s", strobe->getDebugInformation());

		assign(&output, strobe);

		assert(strobe->getPhaseSwitchInterval() == 0);
		assert(strobe->getStrobeMode() == 0);
		assert(result);
		assert(output.fps_ >= 29 && output.fps_ <= 31);
		assert(output.eFPS == output.fps_);
		assert(output.totalFrameCount == output.negativeFrameCount + output.positiveFrameCount);
		assert(output.positiveFrameCount == output.positiveBlackFrameCount + output.positiveRenderedFrameCount);
		assert(output.negativeFrameCount == output.negativeBlackFrameCount + output.negativeRenderedFrameCount);
		assert(output.negativeBlackFrameCount == 0);
		assert(output.positiveBlackFrameCount == 0);
		assert(output.frequency == 0.0);
		assert(output.period >= DBL_MAX);
		assert(output.brightnessReduction == 0.0);
		assert(output.badnessReduced <= DBL_MIN);
	}
	fflush(stdout);
	delete strobe;

	strobe = new StrobeCore(-3, 2, false);
	timer.restart();
	while (timer.elapsedSeconds() < 10)
	{
		fps = random(950, 1050);
		strobe->setFPS(fps);
		strobe->strobe();
		printf("%s", strobe->getDebugInformation());

		assign(&output, strobe);

		assert(strobe->getPhaseSwitchInterval() == 2);
		assert(strobe->getStrobeMode() == -3);
		assert(output.fps_ >= 950 && output.fps_ <= 1050);
		assert(output.eFPS == output.fps_ * 3 / 4);
		assert(output.totalFrameCount == output.negativeFrameCount + output.positiveFrameCount);
		assert(output.positiveFrameCount == output.positiveBlackFrameCount + output.positiveRenderedFrameCount);
		assert(output.negativeFrameCount == output.negativeBlackFrameCount + output.negativeRenderedFrameCount);
		assert(output.frequency > 0);
		assert(fabs(output.frequency - 1000 / output.period) < 0.1);
		assert(output.brightnessReduction < 50.0);
		assert(output.badnessPWM == output.badness * output.period);
		assert(output.badnessReducedPwm == output.badnessReduced * output.period);
	}
	fflush(stdout);
	delete strobe;

	strobe = new StrobeCore(4, 3, true);
	timer.restart();
	while (timer.elapsedSeconds() < 10)
	{
		strobe->strobe();
		printf("%s", strobe->getDebugInformation());

		assign(&output, strobe);

		assert(strobe->getPhaseSwitchInterval() == 3);
		assert(strobe->getStrobeMode() == 4);
		assert(output.totalFrameCount == output.negativeFrameCount + output.positiveFrameCount);
		assert(output.positiveFrameCount == output.positiveBlackFrameCount + output.positiveRenderedFrameCount);
		assert(output.negativeFrameCount == output.negativeBlackFrameCount + output.negativeRenderedFrameCount);
	}
	fflush(stdout);
	delete strobe;

	strobe = new StrobeCore(1, 0, false);
	strobe->disable();
	assert(!strobe->isActive());
	timer.restart();
	while (timer.elapsedSeconds() < 10)
	{
		fps = random(119, 121);
		strobe->setFPS(fps);
		strobe->strobe();
		printf("%s", strobe->getDebugInformation());

		assign(&output, strobe);

		assert(strobe->getPhaseSwitchInterval() == 0);
		assert(strobe->getStrobeMode() == 1);
		assert(output.fps_ >= 119 && output.fps_ <= 121);
		assert(output.eFPS == output.fps_);
		assert(output.totalFrameCount == output.negativeFrameCount + output.positiveFrameCount);
		assert(output.positiveFrameCount == output.positiveBlackFrameCount + output.positiveRenderedFrameCount);
		assert(output.negativeFrameCount == output.negativeBlackFrameCount + output.negativeRenderedFrameCount);
		assert(output.negativeBlackFrameCount == 0);
		assert(output.positiveBlackFrameCount == 0);
		assert(output.frequency == 0.0);
		assert(output.period >= DBL_MAX);
		assert(output.brightnessReduction == 0.0);
		assert(output.badnessReduced <= DBL_MIN);
	}
	strobe->enable();
	timer.restart();
	assert(strobe->isActive());
	assert(strobe->getStrobeMode() == 1);
	assert(strobe->getPhaseSwitchInterval() == 0);

	fflush(stdout);
	delete strobe;

	return 0;
}
