
#include "handmadehero.h"


typedef unsigned long DWORD;



internal void GameOutputSound(game_sound_output_buffer *SoundBuffer, int toneHz)
{
	local_persist real32 tSine;
		int16 toneVolume = 3000;
		if (toneHz == 0)
		{
			toneHz = 440;
		}
		int16 wavePeriod = (int16)(SoundBuffer->samplesPerSecond/toneHz);
		int16 *sampleOut = (int16*)SoundBuffer->samples;
		for (DWORD sampleIndex = 0; sampleIndex < (DWORD)SoundBuffer->sampleCount; ++sampleIndex)
		{
			real32 sineValue = sin(tSine);
			int16 sampleValue = (int16)(sineValue * toneVolume);
			*sampleOut++ = sampleValue;
			*sampleOut++ = sampleValue;
			tSine += 2.0f*Pi32*1.0f / (real32)wavePeriod;
		}
}

internal void
GameGetSoundSamples(game_memory *Memory, game_sound_output_buffer *SoundBuffer)
{
	game_state *GameState = (game_state *)Memory->PermanentStorage;
	GameOutputSound(SoundBuffer, GameState->ToneHz);
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
			uint8 Blue = 0;
			uint8 Green = 0 ;
			uint8 Red = 0;
			
			if (X > Buffer->Width/2){
				Blue = 0;
				Red = (uint8)(X + XOffset);
				Green = (uint8)(Y + YOffset);
			}else
			{
				Blue = (uint8)(X + XOffset);
				Green = (uint8)(Y + YOffset);
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

    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) ==
           (ArrayCount(Input->Controllers[0].Buttons)));
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
	
    for(int ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ++ControllerIndex)
    {
		game_controller_input *Controller = GetController(Input, ControllerIndex);	
		if(Controller->IsAnalog)
		{
			// NOTE(casey): Use analog movement tuning
			GameState->BlueOffset -= (int)(4.0f*(Controller->StickAverageX));
			GameState->GreenOffset += (int)(4.0f*(Controller->StickAverageY));
			GameState->ToneHz = 256 + (int)(128.0f*(Controller->StickAverageY));
		}
		else
		{
			// NOTE(casey): Use digital movement tuning
			 if(Controller->MoveLeft.EndedDown)
				{
					GameState->BlueOffset += 1;
					//GameState->GreenOffset += 1;
				}
			if(Controller->MoveRight.EndedDown)
				{
					GameState->BlueOffset -= 1;
					//GameState->GreenOffset += 1;
				}
				
				if(Controller->MoveDown.EndedDown)
				{
					//GameState->BlueOffset += 1;
					GameState->GreenOffset -= 1;
				}
				if(Controller->MoveUp.EndedDown)
				{
					//GameState->BlueOffset += 1;
					GameState->GreenOffset += 1;
				}
			  
		}

		// Input.AButtonEndedDown;
		// Input.AButtonHalfTransitionCount;
		if(Controller->ActionDown.EndedDown)
		{
			GameState->GreenOffset =  GameState->BlueOffset;
		}
	}
	GameOutputSound(SoundBuffer, GameState->ToneHz);
	RenderWierdGradient(Buffer, GameState->BlueOffset, GameState->GreenOffset);
}