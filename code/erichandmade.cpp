

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
            // Draw a border, may or may not use something like this later.
#if 0
            if (x <= 10 || y <= 10 || x >= Render->Width - 10 || y >= Render-> Height - 10)
                *pixel++ = 0xCCCCCC;
            else
                *pixel++ = 0;
#endif
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
    
    // TODO(Eric): Finish converting this function to v2? Why do I need this?
    //v2 Pos1 = ClampV2(MinV2, Pos, v2{Render->Width, Render->Height});
    //v2 Pos2 = ClampV2(MinV2, Pos + Size, v2{Render->Width, Render->Height});
    
    Assert(x0 <= Render->Width);
    Assert(x1 <= Render->Width);
    Assert(y0 <= Render->Height);
    Assert(y1 <= Render->Height);
    
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
    
    if (LineHeight < 0 && LineWidth < 0)
    {
        x0 = Clamp(0, P2.x, Render->Width);
        x1 = Clamp(0, P1.x, Render->Width);
        y0 = Clamp(0, P2.y, Render->Height);
        y1 = Clamp(0, P1.y, Render->Height);
        
        LineHeight = P1.y - P2.y;
        LineWidth = P1.x - P2.x;
    }
    
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
RenderCircle(render_buffer* Render, v2 Center, real32 Radius, u32 Color)
{
    // X = Radius * cos(Angle * PI/180)
    // Y = Radius * sin(Angle * PI/180)
    
    for (int Angle = 0;
         Angle <= 365;
         ++Angle)
    {
        // Math is pretty great. This just works. Amazing.
        s32 X = RoundReal32ToInt32(Radius * Cos(Angle * Pi32/180)) + Center.x;
        s32 Y = RoundReal32ToInt32(Radius * Sin(Angle * Pi32/180)) + Center.y;
        
        RenderSquare(Render, V2(X, Y), V2(1,1), Color);
    }
    
    RenderSquare(Render, Center, V2(1,1), 0xFF0000);
    
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

// (render_buffer *Render, game_memory *Memory, game_input *Input)
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    //Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    
    // NOTE(Eric): Input.MouseX and Input.MouseY starts at the Top Left corner!
    // This doesn't match the coords of what render_buffer uses!
    
    // TODO(Eric): Next I think I will create Entities, so I can start trying out collision detection
    
    // TODO(Eric): It would be desirable to have each Entity have a single Position
    // Instead of having to think of moving all of the points around.
    
    // NOTE(Eric): Movement is essentially:
    // Getting the old position
    // Calculating their 'speed' based on dtForFrame
    // Setting their new position: old position + speed
    
    //RenderWeirdBackground(Render);
    ClearBackground(Render);
    
    // Diagonal line proving that we are starting in the lower left
    //RenderLine(Render, v2{0,0}, v2{Render->Width, Render->Height}, 3, 0x00FF00);
#if 1
    local_persist screen_map Map;
    if (!Map.IsInitialized)
    {
        u32 TileCountY = MAP_HEIGHT / TILE_SIZE;
        u32 TileCountX = MAP_WIDTH / TILE_SIZE;
        for (u32 IndexY = 0; IndexY <= TileCountY; ++IndexY)
        {
            for (u32 IndexX = 0; IndexX <= MAP_WIDTH / 20; ++IndexX)
            {
                v2 NewPoint = V2(OFFSET_X + (IndexX * TILE_SIZE), OFFSET_Y + (IndexY * TILE_SIZE));
                RenderSquare(Render, NewPoint, v2{2,2}, 0xFF0000);
                
                Map.Tiles[IndexY][IndexX].BottomLeft = NewPoint;
            }
        }
        
        // TODO(Eric): If this is true in it's current state, it only draws on the first frame!
        Map.IsInitialized = false;
    }
    
    // Check collision
    // If Line.Point1 is within a Tile's area, draw a background rect of that Tile
    v2 TestPoint = V2(360, 80);
    tile Tile = GetTileAtPosition(&Map, TestPoint);
    RenderSquare(Render, Tile.BottomLeft, v2{TILE_SIZE, TILE_SIZE}, 0x0000FF);
    
    RenderSquare(Render, TestPoint, v2{3,3}, 0x00FF00);
#endif
    
#if 0
    // Drawing a grid
    int GridSize = 20;
    for (int y = 0; y <= Render->Height; y += GridSize)
    {
        // Draw horizontal lines
        v2 P1 = v2{0, y};
        v2 P2 = v2{Render->Width, y};
        RenderLine(Render, P1, P2, 1, 0x444444);
    }
    for (int x = 0; x <= Render->Width; x += GridSize)
    {
        // Draw vertical lines
        v2 P1 = v2{x, 0};
        v2 P2 = v2{x, Render->Height};
        RenderLine(Render, P1, P2, 1, 0x444444);
    }
    
    
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
    
    // Horizontal line test, scary red line
    v2 horz1 = v2{300, 500};
    v2 horz2 = v2{800, 500};
    RenderLine(Render, horz1, horz2, 3, 0xFF0000);
    
    // Down line test
    v2 down1 = v2{50, 600};
    v2 down2 = v2{600, 100};
    RenderLine(Render, down1, down2, 2, 0xCCCCCC);
    
    // Start from left line test (should swap points)
    v2 left1 = v2{400, 400};
    v2 left2 = v2{150, 225};
    RenderLine(Render, left1, left2, 2, 0xCCCCCC);
#endif
    
    
    // We have our first player!
    RenderPlayer(Render);
    
    
    
    
    v2 Center = V2(600, 600);
    real32 Radius = 40;
    RenderCircle(Render, Center, Radius, 0x00FF00);
    
    
    v2 Center2 = V2(200, 300);
    real32 Radius2 = 88;
    RenderCircle(Render, Center2, Radius2, 0x00FF00);
    
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

Blog post Day 1, Part 2? (3/9/2020):

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

/*

Blog post 2 (3/16/2020);

I drew a grid, and I have my first drawing a circle.

Drawing a circle went super smoothly, suprisingly.
this one, I just looked up the math around it, and coded it.
super simple, and it just worked.

Fixed a bug in RenderLine - Checking if length+width are negative, I'm swapping the points

Our next big problem is actually going to be mapping out our positions,
meaning we have to store our current map, and where things are located
so that we can eventually do collision detection



*/


/*

Notes from Early HH:

CHange our color to 3 floating point values
These are easier to calculate and blend against
(Values between 0 and 1)
Straight up casting a float to an integer TRUNCATES the number: 1.89 -> 1.0

Remember there are steps to moving things..
It all has to be calculated up front, and then rendered with the new calculated positions.

*/