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

#ifndef STROBEAPI_H
#define STROBEAPI_H

#ifndef STROBE_DISABLED

#include <chrono>

class StrobeAPI
{
protected:
	class StrobeTimer
	{
	public:
		inline StrobeTimer()
		{
			startTime = std::chrono::system_clock::now();
		}

		inline double elapsedMilliseconds()
		{
			std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();

			return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		}

		inline double elapsedSeconds()
		{
			return elapsedMilliseconds() / 1000.0;
		}

	private:
		std::chrono::time_point<std::chrono::system_clock> startTime;
	};

	StrobeTimer strobeTimer;

public:

	enum CounterType
	{
		TotalFrame,
		PositiveFrame,
		PositiveRenderedFrame,
		PositiveBlackFrame,
		NegativeFrame,
		NegativeRenderedFrame,
		NegativeBlackFrame
	};

	enum FrameState
	{
		PHASE_POSITIVE = 1 << 0, // Phase: Positive
		PHASE_INVERTED = 1 << 1, // Phase: Inverted
		FRAME_RENDER = 1 << 2,  // Frame: Rendered

		_unassigned1 = (PHASE_POSITIVE | PHASE_INVERTED),
		_unassigned2 = (PHASE_POSITIVE | FRAME_RENDER),
		_unassigned3 = (PHASE_INVERTED | FRAME_RENDER),
		_unassigned4 = (PHASE_POSITIVE | PHASE_INVERTED | FRAME_RENDER)
	};

private:

	enum DifferenceType
	{
		PositiveDifference,
		NegativeDifference,
		TotalDifference
	};

	struct Counter
	{
		int positiveRenderedFrameCount;
		int positiveBlackFrameCount;
		int negativeRenderedFrameCount;
		int negativeBlackFrameCount;

		inline int total(void)
		{
			int totalFrames = 0;
			totalFrames += positiveBlackFrameCount;
			totalFrames += positiveRenderedFrameCount;
			totalFrames += negativeBlackFrameCount;
			totalFrames += negativeRenderedFrameCount;
			return totalFrames;
		}

		inline int totalPositive(void)
		{
			int totalFrames = 0;
			totalFrames += positiveBlackFrameCount;
			totalFrames += positiveRenderedFrameCount;
			return totalFrames;
		}

		inline int totalNegative(void)
		{
			int totalFrames = 0;
			totalFrames += negativeBlackFrameCount;
			totalFrames += negativeRenderedFrameCount;
			return totalFrames;
		}

		inline int totalBlack(void)
		{
			int totalFrames = 0;
			totalFrames += positiveBlackFrameCount;
			totalFrames += negativeBlackFrameCount;
			return totalFrames;
		}

		inline int totalRendered(void)
		{
			int totalFrames = 0;
			totalFrames += positiveRenderedFrameCount;
			totalFrames += negativeRenderedFrameCount;
			return totalFrames;
		}
	};

	char *debugInformation;

	void generateDiffBar(char * const dst, int size, DifferenceType type);
	void generateDebugInformation(void);
	
protected:
	Counter counter;

	FrameState frameState;   // Frame info

	bool processFrame(void);
	
	int strobeMode;
	int switchInterval;

	double geometricMean(double x, double y);
	double arithmeticMean(double x, double y);

public:
	StrobeAPI(int mode, int phaseSwitchInterval);
	virtual ~StrobeAPI();

	bool isPhaseInverted(void);
	bool isFrameRendered(void);
	bool isPhasePositive(void);

	double actualBrightnessReduction(void);
	double logarithmicBrightnessReduction(double base);
	double squareBrightnessReduction(double base);
	double cubeBrightnessReduction(double base);
	double otherBrightnessReduction(double base, double (*reductionFunction)(double));

	double badness(bool PWMInvolved);
	double badnessReduced(bool PWMInvolved);

	double effectiveFPS(void);
	double frequency(void);
	double dutyCycle(void);
	double positivePhaseShift(void);
	double negativePhaseShift(void);
	double period(void);

	int frameCount(CounterType type);

	char * getDebugInformation(void);

	virtual int getStrobeMode(void);
	virtual int getPhaseSwitchInterval(void);

	virtual bool strobe(void) = 0;
	virtual double FPS() = 0;
	virtual void setFPS(double newFPS) = 0;

	virtual void setMode(int mode);
	virtual void enable(void) = 0;
	virtual void disable(void) = 0;
	virtual void setActive(bool status) = 0;

	virtual bool isActive(void);

	void setStrobeMode(int mode);
	void setPhaseSwitchInterval(int phaseSwitchInterval);
};

#include "strobe-api_cpp_.h"

#endif

#endif
