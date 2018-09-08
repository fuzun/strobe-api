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

bool assert_triggered = false;

#define assert_(expr) \
	if(!assert_triggered) assert_triggered = true; assert((expr));

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
	double totalFrameCount;
	double positiveFrameCount;
	double negativeFrameCount;
	double positiveBlackFrameCount;
	double negativeBlackFrameCount;
	double positiveRenderedFrameCount;
	double negativeRenderedFrameCount;
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

	strobe = new StrobeCore(1, 1);
	while (timer.elapsedSeconds() < 10)
	{
		fps = random(58, 62);
		strobe->setFPS(fps);
		strobe->strobe();
		printf("%s", strobe->getDebugInformation());

		assign(&output, strobe);

		assert_(strobe->getPhaseSwitchInterval() == 1);
		assert_(strobe->getStrobeMode() == 1);
		assert_(output.fps_ >= 58 && output.fps_ <= 62);
		assert_(output.eFPS == output.fps_ / 2);
		assert_(output.totalFrameCount == output.negativeFrameCount + output.positiveFrameCount);
		assert_(output.positiveFrameCount == output.positiveBlackFrameCount + output.positiveRenderedFrameCount);
		assert_(output.negativeFrameCount == output.negativeBlackFrameCount + output.negativeRenderedFrameCount);
		assert_(output.frequency > 0);
		assert_(fabs(output.frequency - output.eFPS) < 1);
		assert_(fabs(output.frequency - 1000 / output.period) < 0.1);
		assert_(output.brightnessReduction == (output.fps_ - output.eFPS) * 100.0 / output.fps_);
		assert_(output.badnessPWM == output.badness * output.period);
		assert_(output.badnessReducedPwm == output.badnessReduced * output.period);
	}
	fflush(stdout);
	delete strobe;
	
	strobe = new StrobeCore(2, 3);
	timer.restart();
	while (timer.elapsedSeconds() < 10)
	{
		fps = random(120, 144);
		strobe->setFPS(fps);
		strobe->strobe();
		printf("%s", strobe->getDebugInformation());

		assign(&output, strobe);

		assert_(strobe->getPhaseSwitchInterval() == 3);
		assert_(strobe->getStrobeMode() == 2);
		assert_(output.fps_ >= 120 && output.fps_ <= 144);
		assert_(output.eFPS == output.fps_ / 3);
		assert_(output.totalFrameCount == output.negativeFrameCount + output.positiveFrameCount);
		assert_(output.positiveFrameCount == output.positiveBlackFrameCount + output.positiveRenderedFrameCount);
		assert_(output.negativeFrameCount == output.negativeBlackFrameCount + output.negativeRenderedFrameCount);
		assert_(output.frequency > 0);
		assert_(fabs(output.frequency - 1000 / output.period) < 0.1);
		assert_(output.brightnessReduction == (output.fps_ - output.eFPS) * 100.0 / output.fps_);
		assert_(output.badnessPWM == output.badness * output.period);
		assert_(output.badnessReducedPwm == output.badnessReduced * output.period);
	}
	fflush(stdout);
	delete strobe;

	strobe = new StrobeCore(0, 0);
	timer.restart();
	while (timer.elapsedSeconds() < 10)
	{
		fps = random(29, 31);
		strobe->setFPS(fps);
		bool result = strobe->strobe();
		printf("%s", strobe->getDebugInformation());

		assign(&output, strobe);

		assert_(strobe->getPhaseSwitchInterval() == 0);
		assert_(strobe->getStrobeMode() == 0);
		assert_(result);
		assert_(output.fps_ >= 29 && output.fps_ <= 31);
		assert_(output.eFPS == output.fps_);
		assert_(output.totalFrameCount == output.negativeFrameCount + output.positiveFrameCount);
		assert_(output.positiveFrameCount == output.positiveBlackFrameCount + output.positiveRenderedFrameCount);
		assert_(output.negativeFrameCount == output.negativeBlackFrameCount + output.negativeRenderedFrameCount);
		assert_(output.negativeBlackFrameCount == 0);
		assert_(output.positiveBlackFrameCount == 0);
		assert_(output.frequency == 0.0);
		assert_(output.period >= DBL_MAX);
		assert_(output.brightnessReduction == 0.0);
		assert_(output.badnessReduced <= DBL_MIN);
	}
	fflush(stdout);
	delete strobe;

	strobe = new StrobeCore(-3, 2);
	timer.restart();
	while (timer.elapsedSeconds() < 10)
	{
		fps = random(950, 1050);
		strobe->setFPS(fps);
		strobe->strobe();
		printf("%s", strobe->getDebugInformation());

		assign(&output, strobe);

		assert_(strobe->getPhaseSwitchInterval() == 2);
		assert_(strobe->getStrobeMode() == -3);
		assert_(output.fps_ >= 950 && output.fps_ <= 1050);
		assert_(output.eFPS == output.fps_ * 3 / 4);
		assert_(output.totalFrameCount == output.negativeFrameCount + output.positiveFrameCount);
		assert_(output.positiveFrameCount == output.positiveBlackFrameCount + output.positiveRenderedFrameCount);
		assert_(output.negativeFrameCount == output.negativeBlackFrameCount + output.negativeRenderedFrameCount);
		assert_(output.frequency > 0);
		assert_(fabs(output.frequency - 1000 / output.period) < 0.1);
		assert_(output.brightnessReduction < 50.0);
		assert_(output.badnessPWM == output.badness * output.period);
		assert_(output.badnessReducedPwm == output.badnessReduced * output.period);
	}
	fflush(stdout);
	delete strobe;

	strobe = new StrobeCore(1, 0);
	strobe->disable();
	assert_(!strobe->isActive());
	timer.restart();
	while (timer.elapsedSeconds() < 10)
	{
		fps = random(119, 121);
		strobe->setFPS(fps);
		strobe->strobe();
		printf("%s", strobe->getDebugInformation());

		assign(&output, strobe);

		assert_(strobe->getPhaseSwitchInterval() == 0);
		assert_(strobe->getStrobeMode() == 1);
		assert_(output.fps_ >= 119 && output.fps_ <= 121);
		assert_(output.eFPS == output.fps_);
		assert_(output.totalFrameCount == output.negativeFrameCount + output.positiveFrameCount);
		assert_(output.positiveFrameCount == output.positiveBlackFrameCount + output.positiveRenderedFrameCount);
		assert_(output.negativeFrameCount == output.negativeBlackFrameCount + output.negativeRenderedFrameCount);
		assert_(output.negativeBlackFrameCount == 0);
		assert_(output.positiveBlackFrameCount == 0);
		assert_(output.frequency == 0.0);
		assert_(output.period >= DBL_MAX);
		assert_(output.brightnessReduction == 0.0);
		assert_(output.badnessReduced <= DBL_MIN);
	}
	strobe->enable();
	timer.restart();
	assert_(strobe->isActive());
	assert_(strobe->getStrobeMode() == 1);
	assert_(strobe->getPhaseSwitchInterval() == 0);

	fflush(stdout);
	delete strobe;

	if (assert_triggered)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
