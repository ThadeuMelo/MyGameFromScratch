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

float platformImpl(char *value);
void mainGameLoop(void);
