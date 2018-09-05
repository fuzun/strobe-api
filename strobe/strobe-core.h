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

#ifndef STROBECORE_H
#define STROBECORE_H

#ifndef STROBE_DISABLED

#include "strobe-api.h"

class StrobeCore : public StrobeAPI
{
private:
	double fps;
	double timerSnapshot;
	bool active;
	int modeSnapshot;

public:
	StrobeCore(int mode = 1, int phaseSwitchInterval = 0);
	~StrobeCore();

	bool strobe(void) override;

	double FPS() override;
	int getStrobeMode(void) override;
	void setFPS(double newFPS) override;
	void setMode(int mode) override;
	void enable(void) override;
	void disable(void) override;
	void setActive(bool status) override;
	bool isActive(void) override;
};

#include "strobe-core_cpp_.h"

#endif

#endif
