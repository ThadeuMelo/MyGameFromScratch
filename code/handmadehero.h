#include <windows.h>
#include <stdint.h>
#include <xinput.h>
#include <dsound.h>
#include <math.h>

#define local_persist static
#define global_variable static
#define internal static

#define PIXEL_BIT_COUNT 32
#define BYTES_PER_PIXEL 4
#define Pi32 3.14159265359f

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float real32;
typedef double real64;

struct game_Off_Screen_Buffer
{
	void *Memory;
	int Width;
	int Height;
	int Pitch;
};

struct game_sound_output_buffer
{
	int sampleCount;
	real32 tSine;
	int32 samplesPerSecond;
	int16 *samples;
	
};

internal float platformImpl(char *value);
internal void mainGameLoop(void);
internal void  RenderWierdGradient(game_Off_Screen_Buffer *Buffer, int XOffset, int YOffset);
internal void GameUpdateAndRander(game_Off_Screen_Buffer *Buffer);
