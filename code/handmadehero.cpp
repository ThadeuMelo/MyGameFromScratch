
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

internal void GameUpdateAndRander(game_Off_Screen_Buffer *Buffer)
{

	RenderWierdGradient(Buffer, 0, 0);
}