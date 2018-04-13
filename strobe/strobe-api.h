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

#pragma once

#ifndef STROBE_DISABLED

#include <string>

using namespace std;

class strobe_api
{
private:
	double nexttime, lasttime, framerate;
	size_t mark;
	size_t PositiveNormal, PositiveBlack, NegativeNormal, NegativeBlack;
	char strobemethod[128];

protected:
	
	virtual int getSTROBE_COOLDOWN(void);
	virtual int getSTROBE(void);

	typedef enum
	{
		TotalFrame,
		PositiveFrame,
		PositiveNormalFrame,
		PositiveBlackFrame,
		NegativeFrame,
		NegativeNormalFrame,
		NegativeBlackFrame
	} counterType;

	double ActualBrightnessReduction();
	double LogarithmicBrightnessReduction(double base);
	double SquareBrightnessReduction(double base);
	double CubeBrightnessReduction(double base);
	
	double Badness(bool PWMInvolved);
	double Badness_Reducted(bool PWMInvolved);

	double Frequency();
	double DutyCycle();
	double PositivePhaseShift();
	double NegativePhaseShift();
	double Period();

	bool isPhaseInverted();
	bool isNormal();
	bool isPositive();
	double effectiveFPS();

	virtual void showNormal(void);
	virtual void showBlack(void);
	virtual double currentFPS(void);

	void GenerateDebugStatistics(char *src, int size);
	void GenerateDiffBar(char *src, int size, char type);
	double GeometricMean(double x, double y);
	double ArithmeticMean(double x, double y);
	double StandardDeviation(const double *data, int n);
	double Cooldown();

	int strobeMethod;
	int swapInterval;
	int cooldownDelay;

	size_t FrameCounter(counterType);

	void ProcessFrame();

	typedef enum
	{
		PHASE_POSITIVE = 1 << 0, // Phase: Positive
		PHASE_INVERTED = 1 << 1, // Phase: Inverted
		FRAME_RENDER = 1 << 2  // Frame: Rendered
	} fstate_e; // Frame State

	size_t fCounter;                       // Frame counter
	size_t pCounter, pNCounter, pBCounter; // Positive phase counters
	size_t nCounter, nNCounter, nBCounter; // Negative phase counters
	__int64 elapsedTime;
	double deviation;     // deviation
	double cdTimer;       // Cooldown timer
	bool cdTriggered; // Cooldown trigger status
	fstate_e frameInfo;   // Frame info

	strobe_api();

};

#endif