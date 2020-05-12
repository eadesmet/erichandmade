

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
RenderSquare(render_buffer *Render, v2 Pos, u32 Size, v3 Color)
{
    // Draw a rectangle - Our coordinate system is bottom-up
    Pos.x = Clamp(0.0f, Pos.x, (real32)Render->Width);
    Pos.y = Clamp(0.0f, Pos.y, (real32)Render->Height);
    
    v2 Pos2 = V2(Pos.x + Size, Pos.y + Size);
    Pos2.x = Clamp(0.0f, Pos2.x, (real32)Render->Width);
    Pos2.y = Clamp(0.0f, Pos2.y, (real32)Render->Height);
    
    u32 ColorValue = GetColor(Color);
    
#if 1
    for (int y = RoundReal32ToInt32(Pos.y); y < Pos2.y; ++y)
    {
        u32* RectanglePixel = (u32 *)Render->Pixels + RoundReal32ToInt32(Pos.x) + (y*Render->Width);
        for (int x = RoundReal32ToInt32(Pos.x); x < Pos2.x; ++x)
        {
            *RectanglePixel++ = ColorValue;
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

//~ NOTE(Eric): Line
inline void
PlotLineLow(render_buffer *Render, v2 P1, v2 P2, u32 Thickness, v3 Color)
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
        RenderSquare(Render, V2(x, y), Thickness, Color);
        if (Delta > 0)
        {
            y += yi;
            Delta = Delta - (2*DeltaX);
        }
        Delta = Delta + (2*DeltaY);
    }
}

inline void
PlotLineHigh(render_buffer* Render, v2 P1, v2 P2, u32 Thickness, v3 Color)
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
        RenderSquare(Render, V2(x, y), Thickness, Color);
        if (Delta > 0)
        {
            x+= xi;
            Delta = Delta - (2*DeltaY);
        }
        Delta = Delta + (2*DeltaX);
    }
    
}

inline void
RenderLine(render_buffer* Render, v2 P1, v2 P2, u32 Thickness, v3 Color)
{
    // y = mx + b
    // m = slope of the line
    // m = y2 - y1 / x2 - x1;
    // b = y intercept : where on the y axis the line intercepts
    
    // NOTE(Eric): The RenderSquare is checking bounds
    // We don't have to check it every step of the way
#if 0
    Assert(P1.x <= Render->Width);
    Assert(P2.x <= Render->Width);
    Assert(P1.y <= Render->Height);
    Assert(P2.y <= Render->Height);
#endif
    
    // The change in Y is LESS than the change in X - Meaning the line is closer to X than Y axis
    // 'Low' is left and right near x-axis, 'High' is up and down near y-axis
    if (AbsoluteValue(P2.y - P1.y) < AbsoluteValue(P2.x - P1.x))
    {
        if (P1.x > P2.x)
        {
            PlotLineLow(Render, P2, P1, Thickness, Color);
        }
        else
        {
            PlotLineLow(Render, P1, P2, Thickness, Color);
        }
    }
    else
    {
        if (P1.y > P2.y)
        {
            PlotLineHigh(Render, P2, P1, Thickness, Color);
        }
        else
        {
            PlotLineHigh(Render, P1, P2, Thickness, Color);
        }
    }
    
}

//~ NOTE(Eric): Circle
inline void
RenderCircle(render_buffer* Render, v2 Center, real32 Radius, u32 Thickness, v3 Color)
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
        // NOTE(Eric): I could potentially fill in the circle by drawing lines between these points
        // Octant 1
        RenderSquare(Render, V2(Center.x + x, Center.y + y), Thickness, Color);
        // Octant 2
        RenderSquare(Render, V2(Center.x + y, Center.y + x), Thickness, Color);
        // Octant 3
        RenderSquare(Render, V2(Center.x + y, Center.y - x), Thickness, Color);
        // Octant 4
        RenderSquare(Render, V2(Center.x + x, Center.y - y), Thickness, Color);
        // Octant 5
        RenderSquare(Render, V2(Center.x - x, Center.y - y), Thickness, Color);
        // Octant 6
        RenderSquare(Render, V2(Center.x - y, Center.y - x), Thickness, Color);
        // Octant 7
        RenderSquare(Render, V2(Center.x - y, Center.y + x), Thickness, Color);
        // Octant 8
        RenderSquare(Render, V2(Center.x - x, Center.y + y), Thickness, Color);
    }
    
    // Center point
    //RenderSquare(Render, Center, V2(2,2), 0, 1, 0);
}

//~ NOTE(Eric): Player
inline void
RenderPlayer(game_state *GameState, render_buffer* Render, player* Player, screen_map *Map)
{
    v2 Center = GamePointToScreenPoint(GameState, Player->CenterP);
    v2 Front = GamePointToScreenPoint(GameState, Player->FrontP);
    
    real32 OppositeAngle;
    real32 OppositeAngleTop;
    real32 OppositeAngleBottom;
    if (Player->FacingDirectionAngle > 180)
    {
        OppositeAngle = Player->FacingDirectionAngle - 180;
        OppositeAngleTop = OppositeAngle + 40;
        if (OppositeAngle <= 40)
            OppositeAngleBottom = 360 - (40 - OppositeAngle);
        else
            OppositeAngleBottom = OppositeAngle - 40;
    }
    else
    {
        OppositeAngle = Player->FacingDirectionAngle + 180;
        OppositeAngleBottom = OppositeAngle - 40;
        if (OppositeAngle >= 320)
            OppositeAngleTop = (OppositeAngle + 40) - 360;
        else
            OppositeAngleTop = OppositeAngle + 40;
    }
    
    v2 PlayerTopP = (V2(RoundReal32(PLAYER_HALFWIDTH * Cos(OppositeAngleTop * Pi32/180)),
                        RoundReal32(PLAYER_HALFWIDTH * Sin(OppositeAngleTop * Pi32/180)))
                     + Center);
    
    v2 PlayerBottomP = (V2(RoundReal32(PLAYER_HALFWIDTH * Cos(OppositeAngleBottom * Pi32/180)),
                           RoundReal32(PLAYER_HALFWIDTH * Sin(OppositeAngleBottom * Pi32/180)))
                        + Center);
    
    v3 PlayerColor = V3(0.8, 0.8, 0.8);
    
    RenderLine(Render, Front, PlayerTopP, 1, PlayerColor);
    RenderLine(Render, Front, PlayerBottomP, 1, PlayerColor);
    RenderLine(Render, PlayerBottomP, PlayerTopP, 1, PlayerColor);
    
    
    
#if 0
    // Center point
    RenderSquare(Render, Player->CenterP, 2, V3(0,1,0));
    
    // CenterBack point
    v2 RelationToOrigin = {};
    RelationToOrigin = Front - Center;
    RelationToOrigin *= .5;
    v2 CenterBack = (Center - RelationToOrigin);
    RenderSquare(Render, CenterBack, 3, V3(1,1,1));
    
    // Circle of FrontP
    RenderCircle(Render, Player->CenterP,
                 SquareRoot(Square(Player->CenterP.x - Player->FrontP.x) + Square(Player->CenterP.y - Player->FrontP.y)),
                 1,
                 V3(0,1,0));
    
    // Circle of points where Top and Bottom should lie
    RenderCircle(Render, Center, PLAYER_HALFWIDTH, 1, V3(0,0.5,1));
#endif
}

//~ NOTE(Eric): Asteroids
inline void
RenderAsteroid(render_buffer* Render, screen_map *Map, asteroid* Asteroid)
{
    if (Asteroid->State != AsteroidState_Inactive)
    {
        tile CollidingTile = GetTileAtPosition(Map, Asteroid->CenterP);
        if (CollidingTile.BottomLeft != V2(0,0)) // TODO(ERIC): do something here for a tile that wasn't found
            RenderSquare(Render, CollidingTile.BottomLeft, TILE_SIZE, V3(.5,.2,.5));
        
        RenderCircle(Render, Asteroid->CenterP, Asteroid->Radius, 1, Asteroid->Color);
    }
}

inline void
RenderAsteroidPositions(render_buffer* Render, asteroid* Asteroid)
{
    RenderSquare(Render, Asteroid->CenterP, 2, V3(0,1,0));
    
    RenderSquare(Render, Asteroid->StartP, 2, V3(0,0,1));
    RenderSquare(Render, Asteroid->EndP, 2, V3(0,1,0));
    
    RenderLine(Render, Asteroid->StartP, Asteroid->EndP, 2, V3(.8,.8,.8));
}


//~ NOTE(Eric): Map
inline void
RenderMap(game_state *GameState, render_buffer* Render, screen_map *Map)
{
    v3 MapGridColor = V3(0.2,0.2,0.2);
    for (u32 IndexY = 0; IndexY < Map->TileCountY; ++IndexY)
    {
        for (u32 IndexX = 0; IndexX < Map->TileCountX; ++IndexX)
        {
            //RenderSquare(Render, Map->Tiles[IndexY][IndexX].BottomLeft, v2{2,2}, 1, 1, 0);
            
            // NOTE(Eric): Render Outline of a Square - Could be a handy function to have
            v2 BottomLeft = Map->Tiles[IndexY*Map->TileCountX + IndexX].BottomLeft;
            RenderLine(Render, BottomLeft,
                       V2(BottomLeft.x, BottomLeft.y+TILE_SIZE),
                       1, MapGridColor);
            RenderLine(Render, BottomLeft,
                       V2(BottomLeft.x+TILE_SIZE, BottomLeft.y),
                       1, MapGridColor);
            RenderLine(Render, V2(BottomLeft.x,BottomLeft.y+TILE_SIZE),
                       V2(BottomLeft.x+TILE_SIZE, BottomLeft.y+TILE_SIZE),
                       1, MapGridColor);
            RenderLine(Render, V2(BottomLeft.x+TILE_SIZE,BottomLeft.y),
                       V2(BottomLeft.x+TILE_SIZE, BottomLeft.y+TILE_SIZE),
                       1, MapGridColor);
        }
    }
    
    // Crosshairs
    v3 CrosshairColor = V3(0.5,0.5,0.5);
    RenderLine(Render,
               V2(0, GameState->RenderHalfHeight),
               V2((real32)GameState->RenderWidth, GameState->RenderHalfHeight), 1, CrosshairColor);
    
    RenderLine(Render,
               V2(GameState->RenderHalfWidth, 0),
               V2(GameState->RenderHalfWidth, (real32)GameState->RenderHeight), 1, CrosshairColor);
    
    RenderPlayer(GameState, Render, &GameState->Player, &GameState->Map);
}





//~ NOTE(Eric): Debug Rendering
inline void
RenderCollidingTiles(render_buffer *Render, colliding_tiles_result CollidingTiles)
{
    if (CollidingTiles.Count > 0)
    {
        for(u32 Index = 0; Index < CollidingTiles.Count; ++Index)
        {
            v2 Position = CollidingTiles.Tiles[Index].BottomLeft;
            RenderSquare(Render, Position, TILE_SIZE, V3(.5,.7,.5));
        }
    }
}


#define LINE_HIT_TOLERANCE 0.9f

inline v2
CheckCollisionLineLow(game_state *GameState, v2 P1, v2 P2)
{
    v2 CollisionPoint = {};
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
        // TODO(Eric): Check point (x,y) for collision, set collision point
        // TODO(Eric): For now, just make it collide with the walls and we know that from gamestate
        if (((x <= GameState->RenderWidth+LINE_HIT_TOLERANCE &&
             x >= GameState->RenderWidth-LINE_HIT_TOLERANCE) ||
            (y <= GameState->RenderHeight+LINE_HIT_TOLERANCE &&
             y >= GameState->RenderHeight-LINE_HIT_TOLERANCE)
            ||
            (x <= 0+LINE_HIT_TOLERANCE && x >= 0-LINE_HIT_TOLERANCE) ||
            (y <= 0+LINE_HIT_TOLERANCE && y >= 0-LINE_HIT_TOLERANCE))
            &&
            (x >= -LINE_HIT_TOLERANCE && y >= -LINE_HIT_TOLERANCE)
            &&
            (x != P1.x))
        {
            x = Clamp((real32)0, x, (real32)GameState->RenderWidth);
            y = Clamp((real32)0, y, (real32)GameState->RenderHeight);
            CollisionPoint = V2(x,y);
            break;
        }
        
        if (Delta > 0)
        {
            y += yi;
            Delta = Delta - (2*DeltaX);
        }
        Delta = Delta + (2*DeltaY);
    }
    return(CollisionPoint);
}
inline v2
CheckCollisionLineHigh(game_state *GameState, v2 P1, v2 P2)
{
    v2 CollisionPoint = {};
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
        // TODO(Eric): Check point (x,y) for collision, set collision point
        // TODO(Eric): For now, just make it collide with the walls and we know that from gamestate
        if (
             // X is Right on the MAX X Edge (Right)
            ((x <= GameState->RenderWidth+LINE_HIT_TOLERANCE &&
              x >= GameState->RenderWidth-LINE_HIT_TOLERANCE) 
              ||
             // Y is Right on the MAX Y Edge (Left)
             (y <= GameState->RenderHeight+LINE_HIT_TOLERANCE &&
              y >= GameState->RenderHeight-LINE_HIT_TOLERANCE)
              ||
              // X is on the Left Edge 
             (x <= 0+LINE_HIT_TOLERANCE && x >= 0-LINE_HIT_TOLERANCE) 
             ||
             // Y is on the Bottom Edge
             (y <= 0+LINE_HIT_TOLERANCE && y >= 0-LINE_HIT_TOLERANCE))

             &&
             (y != P1.y))
        {
            x = Clamp((real32)0, x, (real32)GameState->RenderWidth);
            y = Clamp((real32)0, y, (real32)GameState->RenderHeight);
            CollisionPoint = V2(x,y);
            break;
        }
        
        if (Delta > 0)
        {
            x+= xi;
            Delta = Delta - (2*DeltaY);
        }
        Delta = Delta + (2*DeltaX);
    }
    return(CollisionPoint);
}
inline v2
CheckCollisionLine(game_state *GameState, v2 P1, v2 P2)
{
    // NOTE(Eric): This function is only to differentiate between Low vs High functions to make it easier on us
    v2 Result = {};
    if (AbsoluteValue(P2.y - P1.y) < AbsoluteValue(P2.x - P1.x))
    {
        if (P1.x > P2.x)
        {
            Result = CheckCollisionLineLow(GameState, P2, P1);
        }
        else
        {
            Result = CheckCollisionLineLow(GameState, P1, P2);
        }
    }
    else
    {
        if (P1.y > P2.y)
        {
            Result = CheckCollisionLineHigh(GameState, P2, P1);
        }
        else
        {
            Result = CheckCollisionLineHigh(GameState, P1, P2);
        }
    }
    return(Result);
}

inline void
WalkLineCheckCollision(game_state *GameState, render_buffer* Render, v2 P1, v2 P2, u32 Thickness, v3 Color)
{
    // NOTE(Eric): P1 MUST be the source point! Walking the line FROM P1 to P2 - That's the direction!
    
    // NOTE(Eric): I believe having this as a variable will let us set this to whatever we want later.
    // Meaning - colliding with other things.
    v2 ZeroV2 = V2(0,0);
    
    //if (AbsoluteValue(P2.y - P1.y) < AbsoluteValue(P2.x - P1.x))
    if (P2.y < P1.y) // Going Down
    {
        if (P2.x < P1.x) // Going Left
        {
            // Down and to the Left
            v2 CollisionPoint = CheckCollisionLine(GameState, P1, P2);
            if (CollisionPoint.y > ZeroV2.y)
            {
                // Colliding with Left Boundary
                RenderLine(Render, P1, CollisionPoint, Thickness, Color);
                
                // Colliding with x = 0, so x is negative and just needs to swap
                v2 NewEndPoint = V2(AbsoluteValue(P2.x), P2.y);
                
                WalkLineCheckCollision(GameState, Render, CollisionPoint, NewEndPoint, Thickness, Color);
            }
            else if (CollisionPoint.y >= -LINE_HIT_TOLERANCE && CollisionPoint.x > ZeroV2.x)
            {
                // Colliding with Bottom Boundary
                RenderLine(Render, P1, CollisionPoint, Thickness, Color);
                
                // Colliding with y = 0, so y is negative and just needs to swap
                v2 NewEndPoint = V2(P2.x, AbsoluteValue(P2.y));
                
                WalkLineCheckCollision(GameState, Render, CollisionPoint, NewEndPoint, Thickness, Color);
            }
            else
            {
                // Line didn't collide, so render the last line
                RenderLine(Render, P2, P1, Thickness, Color);
            }
        }
        else // Going Right
        {
            // Down and to the Right
            v2 CollisionPoint = CheckCollisionLine(GameState, P1, P2);
            if (CollisionPoint.y > ZeroV2.y && CollisionPoint.x >= GameState->RenderWidth)
            {
                // Colliding with Right Boundary
                RenderLine(Render, P1, CollisionPoint, Thickness, Color);
                
                // Colliding with x >= MAX, so nudge x back in
                v2 NewEndPoint = V2(P2.x - 2 * (GameState->RenderWidth - P2.x), P2.y);
                
                WalkLineCheckCollision(GameState, Render, CollisionPoint, NewEndPoint, Thickness, Color);
                
            }
            else if (CollisionPoint.y >= -LINE_HIT_TOLERANCE && CollisionPoint.x > ZeroV2.x)
            {
                // Colliding with Bottom Boundary
                RenderLine(Render, P1, CollisionPoint, Thickness, Color);
            
                // Colliding with y = 0, so swap Y
                v2 NewEndPoint = V2(P2.x, AbsoluteValue(P2.y));
                
                WalkLineCheckCollision(GameState, Render, CollisionPoint, NewEndPoint, Thickness, Color);
            }
            else
            {
                RenderLine(Render, P1, P2, Thickness, Color);
            }
        }
    }
    else // Going Up
    {
        if (P2.x < P1.x) // Going Left
        {
            // Going Up and Left
            v2 CollisionPoint = CheckCollisionLine(GameState, P1, P2);
            if (CollisionPoint.y > ZeroV2.y && CollisionPoint.x >= -LINE_HIT_TOLERANCE && CollisionPoint.x <= LINE_HIT_TOLERANCE)
            {
                // Colliding with Left Boundary
                RenderLine(Render, P1, CollisionPoint, Thickness, Color);
                
                // Colliding with x = 0, so x is negative and just needs to swap
                v2 NewEndPoint = V2(AbsoluteValue(P2.x), P2.y);
                
                WalkLineCheckCollision(GameState, Render, CollisionPoint, NewEndPoint, Thickness, Color);
            }
            else if (CollisionPoint.y >= GameState->RenderHeight-LINE_HIT_TOLERANCE && CollisionPoint.x >= ZeroV2.x)
            {
                // Colliding with Top Boundary
                RenderLine(Render, P1, CollisionPoint, Thickness, Color);
                
                // Colliding with y = MAX, change Y to be lower
                v2 NewEndPoint = V2(P2.x, P2.y - 2 * (P2.y - GameState->RenderHeight));
                
                WalkLineCheckCollision(GameState, Render, CollisionPoint, NewEndPoint, Thickness, Color);
            }
            else
            {
                RenderLine(Render, P1, P2, Thickness, Color);
            }
        }
        else // Going Right
        {
            // Going Up and Right
            v2 CollisionPoint = CheckCollisionLine(GameState, P1, P2);
            if (CollisionPoint.y > ZeroV2.y && CollisionPoint.x >= GameState->RenderWidth-LINE_HIT_TOLERANCE)
            {
                // Colliding with Right Boundary
                RenderLine(Render, P1, CollisionPoint, Thickness, Color);
                
                // Colliding with x >= MAX, so nudge x back in
                v2 NewEndPoint = V2(P2.x - 2 * (GameState->RenderWidth - P2.x), P2.y);
                
                WalkLineCheckCollision(GameState, Render, CollisionPoint, NewEndPoint, Thickness, Color);
            }
            else if (CollisionPoint.y >= GameState->RenderHeight-LINE_HIT_TOLERANCE && CollisionPoint.x >= ZeroV2.x)
            {
                // Colliding with Top Boundary
                RenderLine(Render, P1, CollisionPoint, Thickness, Color);
                
                // Colliding with y = MAX, change Y to be lower
                v2 NewEndPoint = V2(P2.x, P2.y - 2 * (P2.y - GameState->RenderHeight));
                
                WalkLineCheckCollision(GameState, Render, CollisionPoint, NewEndPoint, Thickness, Color);
            }
            else
            {
                RenderLine(Render, P1, P2, Thickness, Color);
            }
        }
    }
}

inline void
CastAndRenderPlayerLine(game_state *GameState, render_buffer* Render, u32 Thickness, v3 Color)
{
    v2 ScreenP1 = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    
    real32 RayLength = 1000;
    
    v2 ScreenP2 = V2((RayLength * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                     (RayLength * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GamePointToScreenPoint(GameState, GameState->Player.CenterP);
    
    //RenderLine(Render, ScreenP1, ScreenP2, Thickness, Color);
    WalkLineCheckCollision(GameState, Render, ScreenP1, ScreenP2, Thickness, Color);
    
    
}
