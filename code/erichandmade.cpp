

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
            if (x <= 10 || y <= 10 || x >= Render->Width - 10 || y >= Render-> Height - 10)
                *pixel++ = 0xCCCCCC;
            else
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
RenderLine(render_buffer* Render, v2 P1, v2 P2, int Thickness, u32 Color)
{
    // y = mx + b
    // m = slope of the line
    // m = y2 - y1 / x2 - x1;
    // b = y intercept : where on the y axis the line intercepts
    
    
    int x0 = Clamp(0, P1.x, Render->Width);
    int x1 = Clamp(0, P2.x, Render->Width);
    int y0 = Clamp(0, P1.y, Render->Height);
    int y1 = Clamp(0, P2.y, Render->Height);
    
    int LineHeight = P2.y - P1.y;
    int LineWidth = P2.x - P1.x;
    
    float M, B;
    if (LineWidth == 0)
    {
        M = 0;
        B = (float)x0;
        
        // Drawing a vertical line
        for (int y = y0; y < y1; ++y)
        {
            v2 PointToDraw = v2{x0, y};
            
            RenderSquare(Render, PointToDraw, v2{Thickness,Thickness}, Color);
        }
    }
    else
    {
        M = (float)LineHeight / (float)LineWidth;
        B = y0 - (M * x0);
        
        for (int x = x0; x < x1; ++x)
        {
            float Y = (M * x) + B;
            
            v2 PointToDraw = v2{x, (int)Y};
            
            RenderSquare(Render, PointToDraw, v2{Thickness,Thickness}, Color);
        }
    }
    
}

inline void
RenderPlayer(render_buffer* Render)
{
    int CenterX = Render->Width / 2;
    int CenterY = Render-> Height / 2;
    
    v2 P1 = v2{CenterX, CenterY};
    v2 P2 = v2{CenterX, CenterY + 20};
    v2 P3 = v2{CenterX + 40, CenterY + 10};
    
    RenderLine(Render, P1, P2, 1, 0xAAAAAA);
    RenderLine(Render, P1, P3, 1, 0xAAAAAA);
    RenderLine(Render, P2, P3, 1, 0xAAAAAA);
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // But step one should actually be: Create asteroids.
    //Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    
    //RenderWeirdBackground(Render);
    ClearBackground(Render);
    
    v2 LineP1 = v2{50, 50};
    v2 LineP2 = v2{180, 180};
    v2 LineP3 = v2{750, 300};
    
    
    RenderLine(Render, LineP1, LineP3, 1, 0x00AAFF);
    RenderLine(Render, LineP1, LineP2, 1, 0x00FF00);
    RenderLine(Render, LineP2, LineP3, 1, 0xFFFF00);
    RenderSquare(Render, LineP1, v2{5,5}, 0xFF0000);
    RenderSquare(Render, LineP3, v2{5,5}, 0xFF0000);
    RenderSquare(Render, LineP2, v2{5,5}, 0xFF0000);
    
    
    // Blue triangle test
    v2 T1 = v2{100, 160};
    v2 T2 = v2{145, 100};
    v2 T3 = v2{200, 255};
    RenderLine(Render, T1, T2, 1, 0x00AAFF);
    RenderLine(Render, T1, T3, 1, 0x00AAFF);
    RenderLine(Render, T2, T3, 1, 0x00AAFF);
    
    // We have our first player!
    RenderPlayer(Render);
    
    // Horizontal line test, scary red line
    v2 horz1 = v2{300, 500};
    v2 horz2 = v2{800, 500};
    RenderLine(Render, horz1, horz2, 3, 0xFF0000);
    
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


/*

Blog post 2 (3/9/2020):

I mostly fixed the line between the two points issue
(it was about 20 pixles too much in both x and y)
It was what I was using for B - The y-intercept is not just the y coord of the first point
it has to be calculated from the other variables.

however, the blue line is now too low?

My god..

Showed my co-worker the problem, and he solves the whole thing in 2 minutes.
A couple code changes, it's working.

We only need 1 loop, for all the x's, and we are calculating y.
The slope and y-intercept are constants, based off of the points.

Also, drew a little cheeky border

Next step.. transforms??
Drawing a triangle (player) and turning it


*/