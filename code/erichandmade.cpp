

#include "erichandmade.h"

inline void
RenderWeirdBackground(render_buffer* Render)
{
    // Render weird background.
    unsigned int* pixel = (unsigned int*)Render->Pixels;
    for (int y = 0; y < Render->Height; y++)
    {
        for (int x = 0; x < Render->Width; x++)
        {
            *pixel++ = x*y;
        }
    }
    
}

inline void
ClearBackground(render_buffer *Render)
{
    unsigned int* pixel = (unsigned int*)Render->Pixels;
    for (int y = 0; y < Render->Height; y++)
    {
        for (int x = 0; x < Render->Width; x++)
        {
            *pixel++ = 0;
        }
    }
}

inline void
RenderSquare(render_buffer* Render, v2 Pos, v2 Size, u32 Color)
{
    // Draw a rectangle
    // Our coordinate system is bottom-up
    int x0 = Clamp(0, Pos.x, Render->Width);
    int x1 = Clamp(0, Pos.x + Size.x, Render->Width);
    int y0 = Clamp(0, Pos.y, Render->Height);
    int y1 = Clamp(0, Pos.y + Size.y, Render->Height);
    
    for (int y = y0; y < y1; y++)
    {
        u32* RectanglePixel = (u32*)Render->Pixels + x0 + y*Render->Width;
        for (int x = x0; x < x1; x++)
        {
            *RectanglePixel++ = Color;
        }
    }
    
}

inline void
RenderLine(render_buffer* Render, v2 P1, v2 P2, u32 Color)
{
    int x0 = Clamp(0, P1.x, Render->Width);
    int x1 = Clamp(0, P2.x, Render->Width);
    int y0 = Clamp(0, P1.y, Render->Height);
    int y1 = Clamp(0, P2.y, Render->Height);
    
    // NOTE(ERIC): For each pixel location, draw a square there that's 5x5 or something
    
    int Px = x0;
    int Py = y0;
    
    int LineHeight = P2.y - P1.y; // b
    int LineWidth = P2.x - P1.x;  // a
    
    float M = (float)LineHeight / (float)LineWidth;
    int B = P1.y;
    
    // y = mx + b
    // m = slope of the line
    // m = y2 - y1 / x2 - x1;
    // b = y intercept : where on the y axis the line intercepts
    
    // I have M, now just need to solve for x and y?
    // y = Mx;
    
    
    
    for (int y = y0; y < y1; y++)
    {
        
        for (int x = x0; x < x1; x++)
        {
            // if x == y, then we can draw a diagonal line.
            //int LineY = M * x;
            
            float Line = ((float)M * (float)x) + B;
            
            if (y <= Line)
            {
                RenderSquare(Render, v2{x, y}, v2{2,2}, Color);
            }
        }
    }
    
}


extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // But step one should actually be: Create asteroids.
    //Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    
    //RenderWeirdBackground(Render);
    ClearBackground(Render);
    
    // TODO(ERIC): Remedy hot-reloading??
    
    v2 LineP1 = v2{50, 50};
    v2 LineP2 = v2{180, 180};
    
    //RenderLine(Render, LineP1, LineP2, 0xFF00FF);
    
    v2 LineP3 = v2{750, 300};
    
    // NOTE(ERIC): This one shows a dotted line
    // My theory is because the pixels themselves are u32, and not real32
    RenderLine(Render, LineP1, LineP3, 0x00AAFF);
    
    
    //RenderSquare(Render, v2{300, 300}, v2{50,50}, 0xFF0000);
    
    v2 T1 = v2{100, 160};
    v2 T2 = v2{145, 100};
    v2 T3 = v2{200, 255};
    
    RenderLine(Render, T1, T2, 0x00AAFF);
    //RenderLine(Render, T1, T3, 0x00AAFF);
    //RenderLine(Render, T2, T3, 0x00AAFF);
    
    //RenderLine(Render, LineP2, LineP3, 0x0000FF);
    
}







/*

Blog post 1 (3/8/2020):

Thinking about how to draw the player (going to be a triangle).

I broke it down into simpler steps, step 1 being just 'draw a line'.
Then I figured I can just draw 3 lines on 3 points to get the triangle.
I also figured early on that I will 'draw rectangle' on each point.

Drawing a line took me a long time because I just don't know simple math..

Finally discovered y = mx + b, and the calculation for the slope of a line.

and now I'm drawing a line!! woohoo!

Oh, and early on I was thinking the hot reloading isn't working,
but it was all just because I wasn't clearing the render buffer between loads
so it would always have the old drawing there too.
Then created ClearBackground call every time.


*/