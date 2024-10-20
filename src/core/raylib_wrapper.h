#if defined(_WIN32)           
	#define NOGDI             // All GDI defines and routines
	#define NOUSER            // All USER defines and routines
    #define NOSOUND 
    #undef PlaySound
#endif
typedef struct tagMSG *LPMSG;

#if defined(_WIN32)           // raylib uses these names as function parameters
	#undef near
	#undef far
#endif

#include "raylib.h"