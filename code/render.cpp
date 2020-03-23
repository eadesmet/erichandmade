

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
RenderSquare(render_buffer* Render, v2 Pos, v2 Size, real32 R, real32 G, real32 B)
{
    // Draw a rectangle
    // Our coordinate system is bottom-up
    v2 Pos2 = Pos + Size;
    
    Assert(Pos.x <= Render->Width);
    Assert(Pos2.x <= Render->Width);
    Assert(Pos.y <= Render->Height);
    Assert(Pos2.y <= Render->Height);
    
    u32 Color = ((RoundReal32ToUInt32(R * 255.0f) << 16) |
                 (RoundReal32ToUInt32(G * 255.0f) << 8) |
                 (RoundReal32ToUInt32(B * 255.0f) << 0));
    
#if 1
    for (real32 y = Pos.y; y < Pos2.y; y++)
    {
        u32* RectanglePixel = (u32*)Render->Pixels;
        RectanglePixel += RoundReal32ToInt32(Pos.x);
        RectanglePixel += RoundReal32ToInt32(y*Render->Width);
        //real32* RectanglePixel = (real32 *)Render->Pixels + RoundReal32ToInt32(Pos.x) + RoundReal32ToInt32(y*Render->Width);
        for (real32 x = Pos.x; x < Pos2.x; x++)
        {
            *RectanglePixel++ = Color;//(real32)Color;
        }
    }
#else
    // Handmade Hero's way of doing this
    u32 Pitch = Render->Width*4;
    u8 *Row = ((u8 *)Render->Pixels +
               (RoundReal32ToInt32(Pos.x*4)) +
               RoundReal32ToInt32(Pos.y*Pitch));
    
    for(int Y = RoundReal32ToInt32(Pos.y);
        Y < Pos2.y;
        ++Y)
    {
        u32 *Pixel = (u32 *)Row;
        for(int X = RoundReal32ToInt32(Pos.x);
            X < Pos2.x;
            ++X)
        {
            *Pixel++ = Color;
        }
        
        Row += Pitch;
    }
#endif
}

inline void
PlotLineLow(render_buffer* Render, v2 P1, v2 P2, real32 Thickness, real32 R, real32 G, real32 B)
{
    real32 DeltaX = P2.x - P1.x;
    real32 DeltaY = P2.y - P1.y;
    real32 yi = 1;
    
    if (DeltaY < 0)
    {
        yi = -1;
        DeltaY = -DeltaY;
    }
    
    real32 Delta = (2 * DeltaY) - DeltaX;
    real32 y = P1.y;
    
    for (real32 x = P1.x; x < P2.x; ++x)
    {
        RenderSquare(Render, V2(x, y), V2(Thickness,Thickness), R, G, B);
        if (Delta > 0)
        {
            y += yi;
            Delta = Delta - (2*DeltaX);
        }
        Delta = Delta + (2*DeltaY);
    }
}

inline void
PlotLineHigh(render_buffer* Render, v2 P1, v2 P2, real32 Thickness, real32 R, real32 G, real32 B)
{
    real32 DeltaX = P2.x - P1.x;
    real32 DeltaY = P2.y - P1.y;
    real32 xi = 1;
    
    if (DeltaX < 0)
    {
        xi = -1;
        DeltaX = -DeltaX;
    }
    
    real32 Delta = (2*DeltaX) - DeltaY;
    real32 x = P1.x;
    
    for (real32 y = P1.y; y < P2.y; ++y)
    {
        RenderSquare(Render, V2(x, y), V2(Thickness,Thickness), R, G, B);
        if (Delta > 0)
        {
            x+= xi;
            Delta = Delta - (2*DeltaY);
        }
        Delta = Delta + (2*DeltaX);
    }
    
}

inline void
RenderLine(render_buffer* Render, v2 P1, v2 P2, real32 Thickness, real32 R, real32 G, real32 B)
{
    // y = mx + b
    // m = slope of the line
    // m = y2 - y1 / x2 - x1;
    // b = y intercept : where on the y axis the line intercepts
    
    Assert(P1.x <= Render->Width);
    Assert(P2.x <= Render->Width);
    Assert(P1.y <= Render->Height);
    Assert(P2.y <= Render->Height);
    
    if (AbsoluteValue(P2.y - P1.y) < AbsoluteValue(P2.x - P1.x))
    {
        if (P1.x > P2.x)
        {
            PlotLineLow(Render, P2, P1, Thickness, R, G, B);
        }
        else
        {
            PlotLineLow(Render, P1, P2, Thickness, R, G, B);
        }
    }
    else
    {
        if (P1.y > P2.y)
        {
            PlotLineHigh(Render, P2, P1, Thickness, R, G, B);
        }
        else
        {
            PlotLineHigh(Render, P1, P2, Thickness, R, G, B);
        }
    }
    
}

inline void
RenderCircle(render_buffer* Render, v2 Center, real32 Radius, real32 R, real32 G, real32 B)
{
    // X = Radius * cos(Angle * PI/180)
    // Y = Radius * sin(Angle * PI/180)
    
    // Bresenham circle drawing algorithm
    real32 x = 0;
    real32 y = Radius;
    real32 DecisionParameter = 3 - (2*Radius);
    
    while (x <= y)
    {
        if (DecisionParameter < 0)
        {
            x = x + 1;
            y = y;
            DecisionParameter = DecisionParameter + (4 * x) + 6;
        }
        else if (DecisionParameter >= 0)
        {
            x = x + 1;
            y = y - 1;
            DecisionParameter = DecisionParameter + 4 * (x - y) + 10;
        }
        
        // Octant 1
        RenderSquare(Render, V2(Center.x + x, Center.y + y), V2(1,1), R, G, B);
        // Octant 2
        RenderSquare(Render, V2(Center.x + y, Center.y + x), V2(1,1), R, G, B);
        // Octant 3
        RenderSquare(Render, V2(Center.x + y, Center.y - x), V2(1,1), R, G, B);
        // Octant 4
        RenderSquare(Render, V2(Center.x + x, Center.y - y), V2(1,1), R, G, B);
        // Octant 5
        RenderSquare(Render, V2(Center.x - x, Center.y - y), V2(1,1), R, G, B);
        // Octant 6
        RenderSquare(Render, V2(Center.x - y, Center.y - x), V2(1,1), R, G, B);
        // Octant 7
        RenderSquare(Render, V2(Center.x - y, Center.y + x), V2(1,1), R, G, B);
        // Octant 8
        RenderSquare(Render, V2(Center.x - x, Center.y + y), V2(1,1), R, G, B);
    }
    
    // Center point
    RenderSquare(Render, Center, V2(2,2), 0, 1, 0);
}

inline void
RenderPlayer(render_buffer* Render)
{
    real32 CenterX = (real32)(Render->Width / 2);
    real32 CenterY = (real32)(Render-> Height / 2);
    
    v2 P1 = v2{CenterX, CenterY};
    v2 P2 = v2{CenterX, CenterY + 20};
    v2 P3 = v2{CenterX + 40, CenterY + 10};
    
    RenderLine(Render, P1, P2, 1, 0.8, 0.8, 0.8);
    RenderLine(Render, P1, P3, 1, 0.8, 0.8, 0.8);
    RenderLine(Render, P2, P3, 1, 0.8, 0.8, 0.8);
}

inline void
RenderMap(render_buffer* Render, screen_map *Map)
{
    for (u32 IndexY = 0; IndexY <= TILE_COUNT_Y; ++IndexY)
    {
        for (u32 IndexX = 0; IndexX <= TILE_COUNT_X; ++IndexX)
        {
            RenderSquare(Render, Map->Tiles[IndexY][IndexX].BottomLeft, v2{2,2}, 1, 0, 0);
        }
    }
}