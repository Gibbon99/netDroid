#pragma once

#ifdef __WIN32__
    #include "SDL.h"
#else
    #include "SDL2/SDL.h"
#endif

class droidTime
        {
public:
    Uint32 getTicks();

private:

};
