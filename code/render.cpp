

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
    
    
    
    
#if 0
    real32 LineHeight = P2.y - P1.y;
    real32 LineWidth = P2.x - P1.x;
    
    
    if (LineHeight < 0 && LineWidth < 0)
    {
        P1.x = P2.x;
        P2.x = P1.x;
        P1.y = P2.y;
        P2.y = P1.y;
        
        LineHeight = P1.y - P2.y;
        LineWidth = P1.x - P2.x;
    }
    
    real32 Slope, YIntercept = 0.0f;
    if (LineWidth == 0)
    {
        YIntercept = (real32)P1.x;
        
        // Drawing a vertical line
        for (real32 y = P1.y; y < P2.y; ++y)
        {
            v2 PointToDraw = v2{P1.x, y};
            
            RenderSquare(Render, PointToDraw, v2{Thickness,Thickness}, R, G, B);
        }
    }
    else
    {
        Slope = LineHeight / LineWidth;
        YIntercept = P1.y - (Slope * P1.x);
        
        for (real32 x = P1.x; x < P2.x; ++x)
        {
            real32 Y = (Slope * x) + YIntercept;
            
            v2 PointToDraw = v2{x, Y};
            
            RenderSquare(Render, PointToDraw, v2{Thickness,Thickness}, R, G, B);
        }
        
        
    }
#endif
    
}

inline void
RenderCircle(render_buffer* Render, v2 Center, real32 Radius, real32 R, real32 G, real32 B)
{
    // X = Radius * cos(Angle * PI/180)
    // Y = Radius * sin(Angle * PI/180)
    
    // TODO(ERIC): This is broken too, fix this!
    
    for (int Angle = 0;
         Angle <= 365;
         ++Angle)
    {
        // Math is pretty great. This just works. Amazing.
        real32 X = (Radius * Cos(Angle * Pi32/180)) + Center.x;
        real32 Y = (Radius * Sin(Angle * Pi32/180)) + Center.y;
        
        RenderSquare(Render, V2(X, Y), V2(1,1), R, G, B);
    }
    
    RenderSquare(Render, Center, V2(1,1), 1, 0, 0);
    
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