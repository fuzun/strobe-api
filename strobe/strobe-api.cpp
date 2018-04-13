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

#define _CRT_SECURE_NO_WARNINGS


#include "strobe-api.h"

#define lossCalculator( x, y ) ( ( ( x ) - ( y ) ) * 100.0 / ( x ) )

double strobe_api::StandardDeviation(const double *data, int n)
{
	double mean = 0.0, sum_deviation = 0.0;
	int i;
	for (i = 0; i < n; ++i)
	{
		mean += data[i];
	}
	mean = mean / n;
	for (i = 0; i < n; ++i)
		sum_deviation += (data[i] - mean) * (data[i] - mean);
	return sqrt(sum_deviation / n);
}

void strobe_api::showBlack(void)
{
	return;
}

void strobe_api::showNormal(void)
{
	return;
}

int strobe_api::getSTROBE_COOLDOWN(void)
{
	// NOT IMPLEMENTED HERE!
	return 1;
}

int strobe_api::getSTROBE(void)
{
	return 0;
}

double strobe_api::Cooldown()
{
	if (0 <= cdTimer)
	{
		return ((double)abs(getSTROBE_COOLDOWN()) - cdTimer);
	}
	else
	{
		return 0.0;
	}
}

bool strobe_api::isPhaseInverted()
{
	if (frameInfo & PHASE_INVERTED)
		return true;
	else
		return false;
}

bool strobe_api::isNormal()
{
	if (frameInfo & FRAME_RENDER)
		return true;
	else
		return false;
}

bool strobe_api::isPositive()
{
	if (frameInfo & PHASE_POSITIVE)
		return true;
	else
		return false;
}

double strobe_api::effectiveFPS()
{
	int strobeInterval = getSTROBE();
	double eFPS;
	if (strobeInterval > 0)
	{
		eFPS = (currentFPS()) / (strobeInterval + 1);
	}
	else if (strobeInterval < 0)
	{
		strobeInterval = abs(strobeInterval);
		eFPS = (currentFPS() * strobeInterval) / (strobeInterval + 1);
	}
	else
		eFPS = 0.0;
	return eFPS;
}

void strobe_api::GenerateDiffBar(char *src, int size, char type)
{
	char _barCounter = 0;
	int diff_NB = 0;
	int diff = 0;
	int _a, _b;
	bool Neg = false;

	switch (type)
	{
	case (0): // Positive Difference
	{
		diff_NB = (pNCounter - pBCounter);

		if (pCounter)
			diff = (int)round(abs(diff_NB) * 100 / pCounter);

		break;
	}
	case (1): // Negative Difference
	{
		diff_NB = (nNCounter - nBCounter);

		if (nCounter)
			diff = (int)round(abs(diff_NB) * 100 / nCounter);

		break;
	}
	case (2): // Difference of difference
	{
		if (nCounter && pCounter)
		{
			_a = (abs((int)pNCounter - (int)pBCounter) * 100 / pCounter);
			_b = (abs((int)nNCounter - (int)nBCounter) * 100 / nCounter);
			diff = abs((((int)(pNCounter - pBCounter) > 0) ? _a : (-_a)) - ((((int)(nNCounter - nBCounter) > 0) ? _b : (-_b)))); // Min 0 Max 200
		}
		break;
	}
	default:
		break;
	}

	if (diff_NB < 0)
		Neg = true;

	snprintf(src, size, "[");

	for (_barCounter = 0; _barCounter <= 20; ++_barCounter)
	{
		if (_barCounter == 10)
		{
			strncat(src, "O", 1);
		}
		else if (_barCounter < 10)
		{
			if (type == 2)
			{
				if (100 - (_barCounter * 11) <= diff / 2)
					strncat(src, "=", 1);
				else
					strncat(src, "-", 1);
			}
			else
			{
				if (Neg)
				{
					if (100 - (_barCounter * 11) <= diff)
						strncat(src, "=", 1);
					else
						strncat(src, "-", 1);
				}
				else
				{
					strncat(src, "-", 1);
				}
			}
		}
		else if (_barCounter > 10)
		{
			if (type == 2)
			{
				if (((_barCounter - 11) * 11) >= diff / 2)
					strncat(src, "-", 1);
				else
					strncat(src, "=", 1);
			}
			else
			{
				if (Neg)
				{
					strncat(src, "-", 1);
				}
				else
				{
					if (((_barCounter - 11) * 11) >= diff)
						strncat(src, "-", 1);
					else
						strncat(src, "=", 1);
				}
			}
		}
	}
	if (type == 2)
	{
		char temp[64]; // FIX HERE
		sprintf(temp, "] - %d / 200", diff);
		strncat(src, temp, strlen(temp));
	}
	else
	{
		char temp[64]; // FIX HERE
		sprintf(temp, "] - %d%%", (Neg ? -diff : diff));
		strncat(src, temp, strlen(temp));
	}
}

double strobe_api::Frequency()
{
	return (1 / ((1.0f / currentFPS()) * (abs(getSTROBE()) + 1)));
}

double strobe_api::DutyCycle()
{
	int strobeInterval = getSTROBE();
	return (((1.0f / (abs(strobeInterval) + 1)) * 100) * (strobeInterval < 0 ? -strobeInterval : 1));
}

double strobe_api::PositivePhaseShift()
{
	if (!!(frameInfo & PHASE_INVERTED))
		return (1.0f / currentFPS()) * 1000;
	else
		return 0.0f;
}

double strobe_api::NegativePhaseShift()
{
	if (!!(frameInfo & PHASE_INVERTED))
		return abs(getSTROBE()) * (1.0f / currentFPS()) * 1000;
	else
		return 0.0;
}

double strobe_api::Period()
{
	return (((1.0f / currentFPS()) * (abs(getSTROBE()) + 1)) * 1000);
}

double strobe_api::GeometricMean(double x, double y)
{
	return sqrt(abs(x * y));
}

double strobe_api::ArithmeticMean(double x, double y)
{
	return (x + y) / 2;
}

double strobe_api::ActualBrightnessReduction()
{
	return lossCalculator(currentFPS(), effectiveFPS());
}

double strobe_api::LogarithmicBrightnessReduction(double base)
{
	return lossCalculator(log(base), log(base * effectiveFPS() / currentFPS()));
}

double strobe_api::SquareBrightnessReduction(double base)
{
	return lossCalculator(sqrt(base), sqrt(base * effectiveFPS() / currentFPS()));
}

double strobe_api::CubeBrightnessReduction(double base)
{
	return lossCalculator(cbrt(base), cbrt(base * effectiveFPS() / currentFPS()));
}

double strobe_api::Badness_Reducted(bool PWMInvolved)
{
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

	if (PWMInvolved)
		return (badness * Period());
	else
		return badness;
}

double strobe_api::Badness(bool PWMInvolved)
{
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
		-log(((absoluteDifference + GeometricMean((100.0 - diffP), (100.0 - diffN))) / (absoluteDifference + GeometricMean(diffP, diffN))));

	if (PWMInvolved)
		return (badness * Period());
	else
		return badness;
}

size_t strobe_api::FrameCounter(counterType type)
{
	switch (type)
	{
	case (PositiveFrame):
	{
		return pCounter;
		break;
	}
	case (PositiveNormalFrame):
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
	case (NegativeNormalFrame):
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

double strobe_api::currentFPS()
{
	// NOT IMPLEMENTED
	return 0;
}

void strobe_api::GenerateDebugStatistics(char *src, int size)
{
	char diffBarP[128], diffBarN[128], diffBarT[128];
	size_t nPositiveNormal, nPositiveBlack, nNegativeNormal, nNegativeBlack;
	int diffP_NB, diffN_NB;
	double diffP = 0.0, diffN = 0.0;
	double cooldown = cdTimer;

	diffP_NB = (pNCounter - pBCounter);
	diffN_NB = (nNCounter - nBCounter);

	if (pCounter)
		diffP = abs(diffP_NB) * 100.0 / pCounter;

	if (nCounter)
		diffN = abs(diffN_NB) * 100.0 / nCounter;

	GenerateDiffBar(diffBarP, sizeof(diffBarP), 0);
	GenerateDiffBar(diffBarN, sizeof(diffBarN), 1);
	GenerateDiffBar(diffBarT, sizeof(diffBarT), 2);

	nPositiveNormal = FrameCounter( PositiveNormalFrame);
	nPositiveBlack = FrameCounter( PositiveBlackFrame);
	nNegativeNormal = FrameCounter( NegativeNormalFrame);
	nNegativeBlack = FrameCounter( NegativeBlackFrame);

	char strcooldown[64];
	if (cooldown >= 0.0 && cdTriggered)
	{
		snprintf(strcooldown, sizeof(strcooldown), "%.2f secs\n[STROBING DISABLED] ", (double)getSTROBE_COOLDOWN() - cooldown);
	}
	else
		snprintf(strcooldown, sizeof(strcooldown), "0");

	snprintf(src,
		size,
		"%.2f FPS\n%.2f eFPS\n"
		"Elapsed Time: %lld\n"
		"isPhaseInverted = %d\n"
		"Total Frame Count: %zu\n"
		"(+) Phase Frame Count: %zu\n"
		"Normal Frame Count: %zu\n"
		"Black Frame Count: %zu\n"
		"(-) Phase Frame Count: %zu\n"
		"Normal Frame Count: %zu\n"
		"Black Frame Count: %zu\n"
		"=====ANALYSIS=====\n"
		"PWM Simulation:\n"
		" |-> Frequency: %.2f Hz\n"
		" |-> Duty Cycle: %.2f%%\n"
		" |-> Current Phase Shift: +%.4f msec || -%.4f msec\n"
		" |-> Period: %.4f msec\n"
		"Brightness Reduction:\n"
		" |-> [LINEAR] Actual Reduction: %.2f%%\n"
		" |-> [LOG] Realistic Reduction (400 cd/m2 base): %.2f%%\n"
		" |-> [SQUARE] Realistic Reduction (400 cd/m2 base): %.2f%%\n"
		" |-> [CUBE] Realistic Reduction (400 cd/m2 base): %.2f%%\n"
		"Difference (+): %s\nDifference (-): %s\nDifference (TOTAL): %s\n"
		"Geometric Mean: %.4f\n"
		"G/A Difference: %.4f\n"
		"[EXPERIMENTAL] Badness: %.4f\n"
		"[EXPERIMENTAL] Badness x PWM Period: %.4f\n"
		"[EXPERIMENTAL] Badness (Reducted): %.4f\n"
		"[EXPERIMENTAL] Badness (Reducted) x PWM Period: %.4f\n"
		"Stability:\n"
		" |-> Standard Deviation: %.3f\n"
		" |-> Cooldown: %s\n"
		"=====ANALYSIS=====\n",
		currentFPS(),
		effectiveFPS(),
		elapsedTime,
		isPhaseInverted(),
		FrameCounter(TotalFrame),
		FrameCounter(PositiveFrame),
		nPositiveNormal,
		nPositiveBlack,
		FrameCounter(NegativeFrame),
		nNegativeNormal,
		nNegativeBlack,
		Frequency(),
		DutyCycle(),
		PositivePhaseShift(),
		NegativePhaseShift(),
		Period(),
		ActualBrightnessReduction(),
		LogarithmicBrightnessReduction(400.0),
		SquareBrightnessReduction(400.0),
		CubeBrightnessReduction(400.0),
		diffBarP,
		diffBarN,
		diffBarT,
		GeometricMean(diffP, diffN),
		ArithmeticMean(diffP, diffN) - GeometricMean(diffP, diffN),
		Badness(false),
		Badness(true),
		Badness_Reducted(false),
		Badness_Reducted(true),
		deviation,
		strcooldown); // 

	PositiveNormal = FrameCounter(PositiveNormalFrame);
	PositiveBlack = FrameCounter(PositiveBlackFrame);
	NegativeNormal = FrameCounter(NegativeNormalFrame);
	NegativeBlack = FrameCounter(NegativeBlackFrame);
}

void strobe_api::ProcessFrame()
{
	if (cdTriggered != false)
	{
		frameInfo = (fstate_e)(FRAME_RENDER | (frameInfo & PHASE_POSITIVE));
	}

	if (frameInfo & FRAME_RENDER) // Show normal
	{
		if (frameInfo & PHASE_POSITIVE)
			++pNCounter;
		else
			++nNCounter;
		showNormal();
	}
	else // Show black
	{
		if (frameInfo & PHASE_POSITIVE)
			++pBCounter;
		else
			++nBCounter;
		showBlack();
	}
	++fCounter;
}

strobe_api::strobe_api()
{
		nexttime = 0;
		lasttime = 0;
		framerate = 0;
		mark = 0;
		PositiveNormal = 0;
		PositiveBlack = 0;
		NegativeBlack = 0;
		NegativeNormal = 0;
		fCounter = 0;
		pCounter = 0;
		pNCounter = 0;
		pBCounter = 0;
		nCounter = 0;
		nNCounter = 0;
		nBCounter = 0;
		elapsedTime = 0;
		deviation = 0;
		cdTimer = 0;
		cdTriggered = 0;
		frameInfo = (fstate_e)(PHASE_POSITIVE | FRAME_RENDER);
}


#endif
