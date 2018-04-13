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

#include "strobe-api.h"

#define DEVIATION_LIMIT 1.0
#define DEVIATION_SIZE 120

class strobe_core : strobe_api
{
private:
	__int64 recentTime, recentTime2;
	double delta[DEVIATION_SIZE];
	size_t fCounterSnapshot;
	char debugStr[2048];
	int offsetX;
	double nexttime, lasttime;
	int strobeInterval;
	__int64 initialTime;
	void showBlack(void);
	void showNormal(void);

public:
	void strobe();
	void debugHandler();
	int getSWAPINTERVAL();
	int getSTROBE_COOLDOWN(void);
	int getSTROBE(void);
	double currentFPS(void);
	bool active;
	strobe_core();
};

#endif