
#include "handmadehero.h"

/*
Controler/keyboard input
sound output
bitmap output
*/

internal void
mainGameLoop(void)
{

}

internal void GameUserInput(ButtonActions *actionButt, uint8 *XOffset, uint8 *YOffset, uint16 *sTone)
{
	
	/*if (actionButt->Up)
	{
		(*YOffset)++;
	}
	if (actionButt->Down)
	{
		(*YOffset)--;
	}
	if (actionButt->Left)
	{
		(*XOffset)++;
	}
	if (actionButt->Right)
	{

		(*XOffset)--;

	}
	*/
	static uint16 baseValue = 512;
	if (actionButt->ButB)
	{
		baseValue = 262;
	}
	if (actionButt->ButY)
	{
		baseValue = 880;
	}
	if (actionButt->ButX)
	{
		baseValue = 350;
	}
	
	if (actionButt->ButA)
		baseValue = 880;

	*XOffset -= actionButt->StickX / 4096;
	*YOffset += actionButt->StickY / 4096;

	*sTone = baseValue + (int)(256.0f*((real32)actionButt->StickY / 30000.0f));

	if (*sTone >= 20000)
		*sTone = 1000;

}
internal void GameOutputSound(game_sound_output_buffer *SoundBuffer, uint16 toneHz)
{
	local_persist real32 tSine;
		int16 toneVolume = 3000;
		if (toneHz == 0)
		{
			toneHz = 440;
		}
		int16 wavePeriod = SoundBuffer->samplesPerSecond/toneHz;
		int16 *sampleOut = SoundBuffer->samples;
		for (DWORD sampleIndex = 0; sampleIndex < SoundBuffer->sampleCount; ++sampleIndex)
		{
			real32 sineValue = sin(tSine);
			int16 sampleValue = (int16)(sineValue * toneVolume);
			*sampleOut++ = sampleValue;
			*sampleOut++ = sampleValue;
			tSine += 2.0f*Pi32*1.0f / (real32)wavePeriod;
		}
}

internal void 
RenderWierdGradient(game_Off_Screen_Buffer *Buffer, int XOffset, int YOffset)
{	
	int Pitch = Buffer->Width*(BYTES_PER_PIXEL);
	uint8 *Row = (uint8 *) Buffer->Memory;
	
	for(int Y = 0; Y < Buffer->Height; ++Y)
	{
		uint32 *Pixel = (uint32 *) Row;
		for(int X = 0; X < Buffer->Width; ++X)
		{
			uint8 Blue ;
			uint8 Green ;
			uint8 Red = 0;
			
			if (X > Buffer->Width/2){
				Blue = 0;
				Red = (X + XOffset);
			}else
			{
				Blue = (X + XOffset);
				Green = (Y + YOffset);
				Red = 0;
			}
			*Pixel++ = ((Red<<16)|(Green<<8)|Blue);
		}
		Row += Pitch;
	}
}

internal void GameUpdateAndRander(game_memory *Memory, game_input *Input, game_Off_Screen_Buffer *Buffer, 
									game_sound_output_buffer *SoundBuffer)
{

    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if(!Memory->IsInitialized)
    {
		char *Filename = "test.in";
        
        debug_read_file_result File = DEBUGPlatformReadEntireFile(Filename);
        if(File.Contents)
        {
            DEBUGPlatformWriteEntireFile("test_out.out", File.ContentsSize, File.Contents);
            DEBUGPlatformFreeFileMemory(File.Contents);
        }
        GameState->ToneHz = 256;

        // TODO(casey): This may be more appropriate to do in the platform layer
        Memory->IsInitialized = true;
    }

    game_controller_input *Input0 = &Input->Controllers[0];    
    if(Input0->IsAnalog)
    {
        // NOTE(casey): Use analog movement tuning
        GameState->BlueOffset -= (int)4.0f*(Input0->EndX);
		GameState->GreenOffset += (int)4.0f*(Input0->EndY);
        GameState->ToneHz = 256 + (uint16)(128.0f*(Input0->EndY));
    }
    else
    {
        // NOTE(casey): Use digital movement tuning
    }

    // Input.AButtonEndedDown;
    // Input.AButtonHalfTransitionCount;
    if(Input0->Down.EndedDown)
    {
		GameState->GreenOffset += 1;
    }
	
	GameOutputSound(SoundBuffer, GameState->ToneHz);
	RenderWierdGradient(Buffer, GameState->BlueOffset, GameState->GreenOffset);
}