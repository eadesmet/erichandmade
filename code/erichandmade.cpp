

#include "erichandmade.h"


extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{

	//Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

	// Render weird background.
	unsigned int* pixel = (unsigned int*)Render->Pixels;
    for (int y = 0; y < Render->Height; y++)
    {
        for (int x = 0; x < Render->Width; x++)
        {
            *pixel++ = x*y;
        }
    }


    // Draw a rectangle
    // Our coordinate system is bottom-up
    int x0 = Clamp(0, 60, Render->Width);
    int x1 = Clamp(0, 120, Render->Width);
    int y0 = Clamp(0, 60, Render->Height);
    int y1 = Clamp(0, 120, Render->Height);

    u32 Color = 0xFF0000;
    
    for (int y = y0; y < y1; y++)
    {
        u32* RectanglePixel = (u32*)Render->Pixels + x0 + y*Render->Width;
        for (int x = x0; x < x1; x++)
        {
            *RectanglePixel++ = Color;
        }
    }

    // I think what I should do next is try to load some BMPs
    // To make the game, we're not going to be drawing every pixel manually
    // We will have to start loading assets at some point.

    // But step one should actually be: Create asteroids.

    // Step 0.1 - Figure out why this uses 20% CPU, and HandmadeHero doesn't.
    // (Pong also uses 20%)
    // I guess since we are drawing every pixel as much as possible, without the GPU..
    // It's supposed to have high CPU usage
    // 

}