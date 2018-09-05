


# StrobeAPI
**StrobeAPI** or **strobe-api** is a simple software based strobe (a motion blur elimination method) library written in C++. It uses "*black frame insertion*" technique.

Online demo for motion blur elimination through black frame insertion / replacement method can be found here: https://www.testufo.com/blackframes

Black frame insertion technique **greatly reduces motion blur** which is extremely important in modern video games. However it has some disadvantages such as reduced 'effective' fps and brightness. For example, least aggressive mode causes effective fps to drop to half and it also causes 35~% brightness reduction. StrobeAPI provides almost all necessary information regarding its side effects.

With this method, moving objects can get tracked much more easily. For example, https://www.testufo.com/photo#photo=toronto-map.png&pps=960&pursuit=0&height=0 in this demo, moving texts in the map can not be easily read in LCD / LED screen without built-in strobe support. But with strobing, they can clearly get read.

## How To Use?
To actually use StrobeAPI, you need to find an implementation for StrobeAPI. There is a simple but efficient implementation called "Strobe Core" which is already placed in the project source directory.
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

