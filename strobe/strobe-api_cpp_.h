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
#include <cstring>
#include <cmath>
#include <cstdlib>


template<class T>
inline double _lossCalculator(T x, T y)
{
	return (x - y) * 100.0 / x;
}

inline StrobeAPI::StrobeAPI(int mode, int phaseSwitchInterval) : strobeMode(mode), switchInterval(phaseSwitchInterval)
{
	debugInformation = new char[4096];
	debugInformation[0] = 0;

	memset(&counter, 0, sizeof(counter));

	frameState = (FrameState)(PHASE_POSITIVE | FRAME_RENDER);
}

inline StrobeAPI::~StrobeAPI()
{
	delete debugInformation;
	debugInformation = nullptr;
}

inline int StrobeAPI::getPhaseSwitchInterval(void)
{
	return switchInterval;
}

inline void StrobeAPI::setStrobeMode(int mode)
{
	frameState = (FrameState)(PHASE_POSITIVE | FRAME_RENDER);
	strobeMode = mode;
}

inline void StrobeAPI::setPhaseSwitchInterval(int phaseSwitchInterval)
{
	frameState = (FrameState)(frameState & ~PHASE_INVERTED);
	switchInterval = phaseSwitchInterval;
}

inline char * StrobeAPI::getDebugInformation(void)
{
	generateDebugInformation();
	return debugInformation;
}

inline bool StrobeAPI::isPhaseInverted(void)
{
	if (frameState & PHASE_INVERTED)
		return true;
	else
		return false;
}

inline bool StrobeAPI::isFrameRendered(void)
{
	if (frameState & FRAME_RENDER)
		return true;
	else
		return false;
}

inline bool StrobeAPI::isPhasePositive(void)
{
	if (frameState & PHASE_POSITIVE)
		return true;
	else
		return false;
}

inline double StrobeAPI::effectiveFPS(void)
{
	int mode = strobeMode;
	double eFPS;

	if (mode > 0)
	{
		eFPS = (FPS()) / (mode + 1);
	}
	else if (mode < 0)
	{
		mode = abs(mode);
		eFPS = (FPS() * mode) / (mode + 1);
	}
	else
	{
		eFPS = FPS();
	}

	return eFPS;
}

inline void StrobeAPI::generateDiffBar(char * const dst, int size, DifferenceType type)
{
	int pNCounter = counter.positiveRenderedFrameCount;
	int pBCounter = counter.positiveBlackFrameCount;
	int nNCounter = counter.negativeRenderedFrameCount;
	int nBCounter = counter.negativeBlackFrameCount;
	int nCounter = counter.totalNegative();
	int pCounter = counter.totalPositive();

	char _barCounter = 0;
	int diff_NB = 0;
	int diff = 0;
	int _a, _b;
	bool Neg = false;

	switch (type)
	{
	case (PositiveDifference):
	{
		diff_NB = (pNCounter - pBCounter);

		if (pCounter)
			diff = (int)round(abs(diff_NB) * 100 / pCounter);

		break;
	}
	case (NegativeDifference):
	{
		diff_NB = (nNCounter - nBCounter);

		if (nCounter)
			diff = (int)round(abs(diff_NB) * 100 / nCounter);

		break;
	}
	case (TotalDifference):
	{
		if (nCounter && pCounter)
		{
			_a = (abs(pNCounter - pBCounter) * 100 / pCounter);
			_b = (abs(nNCounter - nBCounter) * 100 / nCounter);
			diff = abs(_a - _b);
		}
		break;
	}
	default:
		break;
	}

	if (diff_NB < 0)
		Neg = true;

	snprintf(dst, size, "[");

	// Fix print into itself
	for (_barCounter = 0; _barCounter <= 20; ++_barCounter)
	{
		if (_barCounter == 10)
		{
			snprintf(dst, size, "%s%s", dst, "O");
		}
		else if (_barCounter < 10)
		{
			if (type == 2)
			{
				if (100 - (_barCounter * 11) <= diff / 2)
					snprintf(dst, size, "%s%s", dst, "=");
				else
					snprintf(dst, size, "%s%s", dst, "-");
			}
			else
			{
				if (Neg)
				{
					if (100 - (_barCounter * 11) <= diff)
						snprintf(dst, size, "%s%s", dst, "=");
					else
						snprintf(dst, size, "%s%s", dst, "-");
				}
				else
				{
					snprintf(dst, size, "%s%s", dst, "-");
				}
			}
		}
		else if (_barCounter > 10)
		{
			if (type == 2)
			{
				if (((_barCounter - 11) * 11) >= diff / 2)
					snprintf(dst, size, "%s%s", dst, "-");
				else
					snprintf(dst, size, "%s%s", dst, "=");
			}
			else
			{
				if (Neg)
				{
					snprintf(dst, size, "%s%s", dst, "-");
				}
				else
				{
					if (((_barCounter - 11) * 11) >= diff)
						snprintf(dst, size, "%s%s", dst, "-");
					else
						snprintf(dst, size, "%s%s", dst, "=");
				}
			}
		}
	}
	if (type == 2)
	{
		snprintf(dst, size, "%s] * %d / 200", dst, diff);
	}
	else
	{
		snprintf(dst, size, "%s] * %d%%", dst, (Neg ? -diff : diff));
	}
}

inline double StrobeAPI::frequency(void)
{
	if (strobeMode)
		return (1 / ((1.0f / FPS()) * (abs(strobeMode) + 1)));
	else
		return 0.0;
}

inline double StrobeAPI::dutyCycle(void)
{
	return (((1.0f / (abs(strobeMode) + 1)) * 100) * (strobeMode < 0 ? -strobeMode : 1));
}

inline double StrobeAPI::positivePhaseShift(void)
{
	if (isPhaseInverted())
		return (1.0f / FPS()) * 1000;
	else
		return 0.0f;
}

inline double StrobeAPI::negativePhaseShift(void)
{
	return abs(strobeMode) * positivePhaseShift();
}

inline double StrobeAPI::period(void)
{
	return (1 / frequency() * 1000);
}

inline double StrobeAPI::geometricMean(double x, double y)
{
	return sqrt(abs(x * y));
}

inline double StrobeAPI::arithmeticMean(double x, double y)
{
	return (x + y) / 2;
}

inline double StrobeAPI::actualBrightnessReduction(void)
{
	return _lossCalculator<double>(FPS(), effectiveFPS());
}

inline double StrobeAPI::logarithmicBrightnessReduction(double base)
{
	return _lossCalculator<double>(log(base), log(base * effectiveFPS() / FPS()));
}

inline double StrobeAPI::squareBrightnessReduction(double base)
{
	return _lossCalculator<double>(sqrt(base), sqrt(base * effectiveFPS() / FPS()));
}

inline double StrobeAPI::cubeBrightnessReduction(double base)
{
	return _lossCalculator<double>(cbrt(base), cbrt(base * effectiveFPS() / FPS()));
}

inline double StrobeAPI::otherBrightnessReduction(double base, double (*reductionFunction)(double))
{
	return _lossCalculator<double>(reductionFunction(base), reductionFunction(base * effectiveFPS() / FPS()));
}

inline double StrobeAPI::badnessReduced(bool pwmInvolved)
{
	int pNCounter = counter.positiveRenderedFrameCount;
	int pBCounter = counter.positiveBlackFrameCount;
	int nNCounter = counter.negativeRenderedFrameCount;
	int nBCounter = counter.negativeBlackFrameCount;
	int nCounter = counter.totalNegative();
	int pCounter = counter.totalPositive();

	double badness, Diff;
	int diffP_NB, diffN_NB;
	double diffP = 0.0, diffN = 0.0;
	diffP_NB = (pNCounter - pBCounter);
	diffN_NB = (nNCounter - nBCounter);

	if (pCounter)
		diffP = abs(diffP_NB) * 100.0 / pCounter;

	if (nCounter)
		diffN = abs(diffN_NB) * 100.0 / nCounter;

	if (diffP_NB < 0.0)
		diffP = -diffP;
	if (diffN_NB < 0.0)
		diffN = -diffN;

	Diff = fabs(diffP - diffN);

	if (Diff < 0.0)
		Diff = 0.0;
	else if (Diff > 200.0)
		Diff = 200.0;

	badness = -log((200 - Diff) / (Diff));

	if (pwmInvolved)
		return (badness * period());
	else
		return badness;
}

inline double StrobeAPI::badness(bool pwmInvolved)
{
	int pNCounter = counter.positiveRenderedFrameCount;
	int pBCounter = counter.positiveBlackFrameCount;
	int nNCounter = counter.negativeRenderedFrameCount;
	int nBCounter = counter.negativeBlackFrameCount;
	int nCounter = counter.totalNegative();
	int pCounter = counter.totalPositive();

	int diffP_NB, diffN_NB;
	double diffP = 0.0, diffN = 0.0;
	double absoluteDifference = 0.0;
	double badness = 0.0;

	diffP_NB = (pNCounter - pBCounter);
	diffN_NB = (nNCounter - nBCounter);

	if (pCounter)
		diffP = abs(diffP_NB) * 100.0 / pCounter;

	if (nCounter)
		diffN = abs(diffN_NB) * 100.0 / nCounter;

	absoluteDifference = fabs(diffP - diffN);
	if (absoluteDifference > 100.0)
		absoluteDifference = 100.0;

	badness =
		-log(((absoluteDifference + geometricMean((100.0 - diffP), (100.0 - diffN))) / (absoluteDifference + geometricMean(diffP, diffN))));

	if (pwmInvolved)
		return (badness * period());
	else
		return badness;
}

inline int StrobeAPI::frameCount(CounterType type)
{
	int pNCounter = counter.positiveRenderedFrameCount;
	int pBCounter = counter.positiveBlackFrameCount;
	int nNCounter = counter.negativeRenderedFrameCount;
	int nBCounter = counter.negativeBlackFrameCount;
	int fCounter = counter.total();
	int nCounter = counter.totalNegative();
	int pCounter = counter.totalPositive();

	switch (type)
	{
	case (PositiveFrame):
	{
		return pCounter;
		break;
	}
	case (PositiveRenderedFrame):
	{
		return pNCounter;
		break;
	}
	case (PositiveBlackFrame):
	{
		return pBCounter;
		break;
	}
	case (NegativeFrame):
	{
		return nCounter;
		break;
	}
	case (NegativeRenderedFrame):
	{
		return nNCounter;
		break;
	}
	case (NegativeBlackFrame):
	{
		return nBCounter;
		break;
	}
	case (TotalFrame):
	default:
		return fCounter;
		break;
	}
}

inline void StrobeAPI::generateDebugInformation(void)
{
	char * const dst = debugInformation;

	int pNCounter = counter.positiveRenderedFrameCount;
	int pBCounter = counter.positiveBlackFrameCount;
	int nNCounter = counter.negativeRenderedFrameCount;
	int nBCounter = counter.negativeBlackFrameCount;
	int fCounter = counter.total();
	int nCounter = counter.totalNegative();
	int pCounter = counter.totalPositive();

	char modeDescription[128];
	modeDescription[0] = 0;
	char diffBarP[128], diffBarN[128], diffBarT[128];

	int diffP_NB, diffN_NB;
	double diffP = 0.0, diffN = 0.0;
	
	diffP_NB = (pNCounter - pBCounter);
	diffN_NB = (nNCounter - nBCounter);

	if (pCounter)
		diffP = abs(diffP_NB) * 100.0 / pCounter;

	if (nCounter)
		diffN = abs(diffN_NB) * 100.0 / nCounter;

	generateDiffBar(diffBarP, sizeof(diffBarP), PositiveDifference);
	generateDiffBar(diffBarN, sizeof(diffBarN), NegativeDifference);
	generateDiffBar(diffBarT, sizeof(diffBarT), TotalDifference);

	// Fix print into itself later!
	if (!strlen(modeDescription) && strobeMode < 10)
	{
		if (strobeMode != 0)
		{
			snprintf(modeDescription, sizeof(modeDescription), (strobeMode > 0 ? "%d [RENDER" : "%d [BLACK"), strobeMode);
			for (int k = 1; k <= abs(strobeMode); ++k)
			{
				snprintf(modeDescription, sizeof(modeDescription), "%s%s", modeDescription, (strobeMode > 0 ? " - BLACK" : " - RENDER"));
			}
			snprintf(modeDescription, sizeof(modeDescription), "%s]", modeDescription);
		}
		else
		{
			snprintf(modeDescription, sizeof(modeDescription), "[RENDER]");
		}
	}

	snprintf(dst, \
		4096, \
		"%.2f FPS\n" \
		"%.2f eFPS\n" \
		"Strobe Mode: %s\n" \
		"Phase Switch Interval: %d\n" \
		"Elapsed Time: %.2f\n" \
		"isPhaseInverted = %d\n" \
		"Total Frame Count: %d\n" \
		"(+) Phase Frame Count: %d\n" \
		" Rendered Frame Count: %d\n" \
		" Black Frame Count: %d\n" \
		"(-) Phase Frame Count: %d\n" \
		" Rendered Frame Count: %d\n" \
		" Black Frame Count: %d\n" \
		"=====ANALYSIS=====\n" \
		"PWM Simulation:\n" \
		" |-> Frequency: %.2f Hz\n" \
		" |-> Duty Cycle: %.2f%%\n" \
		" |-> Phase Shift: +%.4f msec || -%.4f msec\n" \
		" |-> Period: %.2f msec\n" \
		"Brightness Reduction:\n" \
		" |-> [LINEAR] Actual Reduction: %.2f%%\n" \
		" |-> [LOG] Realistic Reduction (350 cd/m2 base): %.2f%%\n" \
		" |-> [SQUARE] Realistic Reduction (350 cd/m2 base): %.2f%%\n" \
		" |-> [CUBE] Realistic Reduction (350 cd/m2 base): %.2f%%\n" \
		"Difference (+): %s\n" \
		"Difference (-): %s\n" \
		"Difference (TOTAL): %s\n" \
		"Geometric Mean: %.4f\n" \
		"Geometric / Arithmetic Mean Difference: %.4f\n" \
		"[EXPERIMENTAL] Badness: %.4f\n" \
		"[EXPERIMENTAL] Badness x PWM Period: %.4f\n" \
		"[EXPERIMENTAL] Badness (Reduced): %.4f\n" \
		"[EXPERIMENTAL] Badness (Reduced) x PWM Period: %.4f\n" \
		"=====ANALYSIS=====\n", \
		FPS(), \
		effectiveFPS(), \
		modeDescription, \
		switchInterval, \
		strobeTimer.elapsedSeconds(), \
		isPhaseInverted(), \
		fCounter, \
		pCounter, \
		pNCounter, \
		pBCounter, \
		nCounter, \
		nNCounter, \
		nBCounter, \
		frequency(), \
		dutyCycle(), \
		positivePhaseShift(), \
		negativePhaseShift(), \
		period(), \
		actualBrightnessReduction(), \
		logarithmicBrightnessReduction(350.0), \
		squareBrightnessReduction(350.0), \
		cubeBrightnessReduction(350.0), \
		diffBarP, \
		diffBarN, \
		diffBarT, \
		geometricMean(diffP, diffN), \
		arithmeticMean(diffP, diffN) - geometricMean(diffP, diffN), \
		badness(false), \
		badness(true), \
		badnessReduced(false), \
		badnessReduced(true) \
		);
}

inline bool StrobeAPI::isActive()
{
	return true;
}

inline void StrobeAPI::setMode(int mode)
{
	strobeMode = mode;
}

inline int StrobeAPI::getStrobeMode(void)
{
	return strobeMode;
}

inline bool StrobeAPI::processFrame(void)
{
	if ((frameState & FRAME_RENDER) || !isActive())
	{
		if (frameState & PHASE_POSITIVE)
			++counter.positiveRenderedFrameCount;
		else
			++counter.negativeRenderedFrameCount;

		return true;
	}
	else
	{
		if (frameState & PHASE_POSITIVE)
			++counter.positiveBlackFrameCount;
		else
			++counter.negativeBlackFrameCount;

		return false;
	}
}
