#if !defined(WIN32_HANDMADE_H)

#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <xinput.h>
#include <dsound.h>

struct Win32_Off_Screen_Buffer
{
	 BITMAPINFO Info;
	 void *Memory;
	 int Width;
	 int Height;
	 int Pitch;
	 int BytesPerPixel = 4;
};


struct Win32_Window_Dimension
{
	int Width;
	int Height;
};


struct Win32_output_sound
{
	int samplesPerSecond = 48000;
	int waveCounter = 0;
	int sTone = 120;
	int wavePeriod = samplesPerSecond / sTone;
	int bytesPerSample = sizeof(int16) * 2;
	DWORD secondaryBufferSize = samplesPerSecond*bytesPerSample;
	DWORD safetyBytes;
	uint32 runningSampleIndex = 0;
	real32 tSine;
	int latencySampleCount = samplesPerSecond / 60;

};

struct win32_debug_time_marker
{
    DWORD OutputPlayCursor;
    DWORD OutputWriteCursor;
    DWORD OutputLocation;
    DWORD OutputByteCount;
    DWORD ExpectedFlipPlayCursor;
    
    DWORD FlipPlayCursor;
    DWORD FlipWriteCursor;
};


#define WIN32_HANDMADE_H
#endif