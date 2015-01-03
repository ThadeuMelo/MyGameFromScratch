#if !defined(WIN32_HANDMADE_H)
struct Win32_Off_Screen_Buffer
{
	 BITMAPINFO Info;
	 void *Memory;
	 int Width;
	 int Height;
	 int Pitch;
};


struct Win32_Window_Dimension
{
	int Width;
	int Height;
};


struct Win32_output_sound
{
	int samplesPersecond = 48000;
	int waveCounter = 0;
	int sTone = 120;
	int wavePeriod = samplesPersecond / sTone;
	int bytesPersample = sizeof(int16) * 2;
	int SecondaryBufferSize = samplesPersecond*bytesPersample;
	uint32 runningSampleIndex = 0;
	real32 tSine;
	int latancySampleCount = samplesPersecond / 60;

};

#define WIN32_HANDMADE_H
#endif