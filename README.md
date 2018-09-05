

# StrobeAPI
strobe-api is a simple software based strobe (a motion blur elimination method) library written in C++. It uses "black frame insertion" technique.

Online demo for motion blur elimination through black frame insertion / replacement method can be found here: https://www.testufo.com/blackframes

## How To Use?
To use this project, you need to find an implementation for StrobeAPI. There is a simple but efficient implementation called "Strobe Core" which is already placed in the project source directory.
StrobeAPI is a header-only library so it only consists of header files. test.cpp in the source directory is used for testing and not necessary for production. However, it can be kept without any problem as long as you don't include it in your project.
To compile StrobeAPI, a modern C++ compiler should be enough.
For integration examples, see test.cpp and Flappy-Bird-Qt project linked at the bottom of this page.

### Example

    #include "strobe-core.h"
    ...
    extern StrobeAPI *strobe;
    
    int main()
    {
    ...
	    StrobeAPI *strobe = new StrobeCore(1 /* Strobe Mode 1: Applies RENDER - BLACK sequence */, 0 /* Interval for phase switching. Use only if image retention occurs */);
	...
    }
    
    void SwapBuffers()
    {
	...
	    // At the end of swap buffers code, a bool value coming from the strobe() function should be used to determine whether to show actual rendered frame or to show completely black frame
	    // StrobeAPI automatically tracks previous and upcoming frames so do not intercept once it starts keep tracking.
	    bool showRenderedFrame = strobe->strobe();
	    
	    if(showRenderedFrame)
		    // Do the stuff as usual
	    else
		    // Show black frame
	}
	void fps()
	{
	...
		strobe->setFPS(fps);
	}
	char * strobeDebugOutput()
	{
		return strobe->getDebugInformation();
	}

## Example Run
Strobe method = 2,
Strobe swap interval = 2,
Strobe cooldown delay = 3,
FPS = randomly generated in the range of (99, 101)

![Simulation](https://vgy.me/An4BfE.png)

## Projects using StrobeAPI
 - https://github.com/fuzun/Flappy-Bird-Qt
 - https://github.com/fuzun/xash3d-strobe (Uses old C version of StrobeAPI)

