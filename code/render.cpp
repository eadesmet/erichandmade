

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
            *Pixel++ = ColorValue;
        }

        Row += Pitch;
    }
#endif
}

inline void
RenderRect(render_buffer *Render, v2 Min, v2 Max, v3 Color)
{
    // Draw a rectangle - Our coordinate system is bottom-up
    v2 Pos = Min;
    Pos.x = Clamp(0.0f, Min.x, (real32)Render->Width);
    Pos.y = Clamp(0.0f, Min.y, (real32)Render->Height);

    v2 Pos2 = Max;
    Pos2.x = Clamp(0.0f, Pos2.x, (real32)Render->Width);
    Pos2.y = Clamp(0.0f, Pos2.y, (real32)Render->Height);

    u32 ColorValue = GetColor(Color);

    for (int y = RoundReal32ToInt32(Pos.y); y < Pos2.y; ++y)
    {
        u32* RectanglePixel = (u32 *)Render->Pixels + RoundReal32ToInt32(Pos.x) + (y*Render->Width);
        for (int x = RoundReal32ToInt32(Pos.x); x < Pos2.x; ++x)
        {
            *RectanglePixel++ = ColorValue;
        }
    }
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

#if 1
    // NOTE(Eric): This removes the need for PlotLineLow and PlotLineHigh!
    // Testing out streamlined version of bresenham's line algorithm
    int DeltaX = (int)AbsoluteValue(P2.x - P1.x);
    int IncrementX = P1.x < P2.x ? 1 : -1;
    int DeltaY = (int)-(AbsoluteValue(P2.y - P1.y));
    int IncrementY = P1.y < P2.y ? 1 : -1;
    int ErrorAmount = DeltaX + DeltaY;
    int E2 = 0;

    int x0 = (int)P1.x;
    int x1 = (int)P2.x;
    int y0 = (int)P1.y;
    int y1 = (int)P2.y;

    while(true)
    {
        RenderSquare(Render, V2((real32)x0,(real32)y0), Thickness, Color);

        // Checking if it's within 1, so possibly the last pixel doesn't get rendered
        // When this was a straight up equality check, it _sometimes_ caused an infinite loop
        if (x0+1 >= x1 && x0-1 <= x1 &&
            y0+1 >= y1 && y0-1 <= y1)
        {
            break;
        }

        E2 = 2 * ErrorAmount;
        if (E2 >= DeltaY)
        {
            ErrorAmount += DeltaY;
            x0 += IncrementX;
        }
        if (E2 <= DeltaX)
        {
            ErrorAmount += DeltaX;
            y0 += IncrementY;
        }
    }



#else
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
#endif
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
    v3 PlayerColor = V3(0.8, 0.8, 0.8);

    v2 ScreenFront = GamePointToScreenPoint(Player->FrontP, false);
    v2 ScreenTopP = GamePointToScreenPoint(Player->BackLeftP, false);;
    v2 ScreenBottomP = GamePointToScreenPoint(Player->BackRightP, false);;

    RenderLine(Render, ScreenFront, ScreenTopP, 1, PlayerColor);
    RenderLine(Render, ScreenFront, ScreenBottomP, 1, PlayerColor);
    RenderLine(Render, ScreenBottomP, ScreenTopP, 1, PlayerColor);

#if 0
    v2 Center = GamePointToScreenPoint(Player->CenterP, false);
    v2 Front = GamePointToScreenPoint(Player->FrontP, false);

    int OppositeAngle;
    int OppositeAngleTop;
    int OppositeAngleBottom;

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

    // NOTE(Eric): This can probably be even simpler: given Angle of front, FrontP, and a length, calculate points
    OppositeAngle = (int)(Player->FacingDirectionAngle + 180) % 360;
    OppositeAngleTop = (int)(OppositeAngle + 40) % 360;
    OppositeAngleBottom = (int)(OppositeAngle - 40) % 360;


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
#endif


#if 0
    // Center point
    RenderSquare(Render, Center, 2, V3(0,1,0));

    // FrontP
    RenderSquare(Render, Front, 2, V3(0,1,0));

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
RenderAsteroid(render_buffer* Render, screen_map *Map, entity* Asteroid)
{
    if (Asteroid->State != EntityState_InActive)
    {
        for (u32 PointIndex = 0;
             PointIndex < Asteroid->PointCount;
             PointIndex++)
        {
            if (PointIndex != Asteroid->PointCount - 1)
            {
                v2 P1 = GamePointToScreenPoint(Asteroid->CenterP + *(Asteroid->Points + PointIndex));
                v2 P2 = GamePointToScreenPoint(Asteroid->CenterP + *(Asteroid->Points + PointIndex + 1));
                RenderLine(Render, P1, P2, 2, V3(.7,.5,.2));
            }
            else
            {
                v2 P1 = GamePointToScreenPoint(Asteroid->CenterP + *(Asteroid->Points + PointIndex));
                v2 P2 = GamePointToScreenPoint(Asteroid->CenterP + *Asteroid->Points);
                RenderLine(Render, P1, P2, 2, V3(.7,.5,.2));
            }
        }
    }
}

inline void
RenderAsteroidPositions(render_buffer* Render, entity* Asteroid)
{
    v2 Screen_AsteroindCenterP = GamePointToScreenPoint(Asteroid->CenterP);
    // NOTE(Eric): Center point
    RenderSquare(Render, Screen_AsteroindCenterP, 2, V3(0,1,0));

    // NOTE(Eirc): Direction vector
    v2 Screen_VelocityNormal = Screen_AsteroindCenterP + (Asteroid->Velocity*10);
    RenderLine(Render, Screen_AsteroindCenterP, Screen_VelocityNormal, 2, V3(0,1,0));

    // NOTE(Eric): Circle of 'MaxAsteroidSize'
    RenderCircle(Render, GamePointToScreenPoint(Asteroid->CenterP), 4.5f*METERS_TO_PIXELS, 1, v3{1,1,1});
}

//~ NOTE(Eric): Projectiles
inline void
RenderProjectile(render_buffer* Render, screen_map *Map, entity* Projectile)
{
    //real32 ProjectileSize = 10;
    v3 ProjectileColor = V3(1.0f, 1.0f, 0.0f);

#if 0
    for (u32 PointIndex = 0;
             PointIndex < Projectile->PointCount;
             PointIndex++)
    {
        if (PointIndex != Projectile->PointCount - 1)
        {
            v2 P1 = GamePointToScreenPoint(*(Projectile->Points + PointIndex));
            v2 P2 = GamePointToScreenPoint(*(Projectile->Points + PointIndex + 1));
            RenderLine(Render, P1, P2, 2, ProjectileColor);
        }
        else
        {
            v2 P1 = GamePointToScreenPoint(*(Projectile->Points + PointIndex));
            v2 P2 = GamePointToScreenPoint(*(Projectile->Points + PointIndex + 1));
            RenderLine(Render, P1, P2, 2, ProjectileColor);
        }

    }
#else
    // NOTE(Eric): Assuming the Projectile is only two points
    v2 P1 = GamePointToScreenPoint(*(Projectile->Points));
    v2 P2 = GamePointToScreenPoint(*(Projectile->Points + 1));
    RenderLine(Render, P1, P2, 2, ProjectileColor);
#endif
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
            v2 BottomLeft = Map->Tiles[IndexY*Map->TileCountX + IndexX].BottomLeft;

            // NOTE(Eric): This is just to render a chessboard-like map
            if (((IndexX % 2) && (IndexY % 2)) || (!(IndexY % 2) && !(IndexX % 2)))
            {
                // NOTE(Eric): Render Outline of a Square - Could be a handy function to have
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
            else
            {
                RenderSquare(Render, BottomLeft, TILE_SIZE, MapGridColor);
            }
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

inline void
RenderWireBoundingBox(render_buffer *Render, v2 Min, v2 Max)
{
    v2 TopLeft = V2(Min.x, Max.y);
    v2 BottomLeft = Min;
    v2 TopRight = Max;
    v2 BottomRight = V2(Max.x, Min.y);

    RenderLine(Render, BottomLeft, BottomRight, 1, V3(1,1,1));
    RenderLine(Render, BottomLeft, TopLeft, 1, V3(1,1,1));
    RenderLine(Render, TopRight, TopLeft, 1, V3(1,1,1));
    RenderLine(Render, TopRight, BottomRight, 1, V3(1,1,1));

}

inline void
RenderWireBoundingBox(render_buffer *Render, bounding_box *Box)
{
    v2 TopLeft = V2(Box->Min.x, Box->Max.y);
    v2 BottomLeft = Box->Min;
    v2 TopRight = Box->Max;
    v2 BottomRight = V2(Box->Max.x, Box->Min.y);

    v3 Color = {};
    if (Box->Color == V3(0,0,0))
        Color = v3{1,1,1};

    RenderLine(Render, BottomLeft, BottomRight, 1, Color);
    RenderLine(Render, BottomLeft, TopLeft, 1, Color);
    RenderLine(Render, TopRight, TopLeft, 1, Color);
    RenderLine(Render, TopRight, BottomRight, 1, Color);

    if (Box->Fill)
    {
        RenderRect(Render, BottomLeft, TopRight, v3{0,1,1});
    }

}

inline void
RenderDebugLine(render_buffer *Render, line *Line)
{
    RenderLine(Render, Line->P1, Line->P2, 1, Line->Color);
}

//~ NOTE(Eric): UI Rendering
inline void
RenderUI(game_state *GameState, render_buffer *Render, real32 dt)
{
    // TODO(Eric): I should scale all of these positions so I can move this 'UI' wherever I want.

    // NOTE(Eric): Shot Reload Timer
    real32 ShotTimer = GameState->Player.ShotTimer;
    real32 ShotRate = (real32)SHOT_RATE;
    real32 ShotMaxX = ShotRate*5*20;
    v2 Min = {20,20};
    v2 Max = {ShotMaxX,40};
    bounding_box ShotTimerOutline = {Min,Max};
    RenderWireBoundingBox(Render, &ShotTimerOutline);

    real32 val = Lerp(dt, ShotRate, ShotTimer);
    real32 ShotProgressX = Lerp(Min.x, ShotMaxX, val);

    v2 ProgMin = {25,25};
    v2 ProgMax = v2{Clamp(ProgMin.x, ShotProgressX, Max.x-5), 35};
    RenderRect(Render, ProgMin, ProgMax, v3{0,1,1});


    // NOTE(Eric): Score
    bounding_box ScoreBox = {{Max.x+20, Min.y},{Max.x+300, Max.y}};
    RenderWireBoundingBox(Render, &ScoreBox);
    for (u32 Score = 0; Score < GameState->Player.Score; ++Score)
    {
        real32 SpacingX = ((real32)Score * 5)+5;
        real32 SpacingY = 5;
        RenderLine(Render, {ScoreBox.Min.x+SpacingX, ScoreBox.Min.y+SpacingY},
                           {ScoreBox.Min.x+SpacingX, ScoreBox.Max.y-SpacingY}, 1, {1,0,0});
    }

    if (IsPaused)
    {
        // TODO(Eric): Maybe someday get some text rendering in here.
        RenderRect(Render, {ScoreBox.Max.x+20, ScoreBox.Min.y},
                           {ScoreBox.Max.x+300, ScoreBox.Max.y}, v3{1,0,0});
    }

    // NOTE(Eric): DEBUGGING: Show Scale of 1 meter
    //RenderLine(Render, GamePointToScreenPoint(V2(0,-10)),
    //                   GamePointToScreenPoint(V2(1,-10)), 3, v3{1,0,1});

}



#if 0
// TODO(Eric): MOVE THESE FUNCTION SOMEWHERE ELSE. Also, is there a better way?
inline v2
FindMinPolyP(poly Poly)
{
    real32 MinX = Poly.Points[0].x, MinY = Poly.Points[0].y;
    for (u32 PolyPIndex = 0; PolyPIndex < Poly.PointCount; PolyPIndex++)
    {
        if (Poly.Points[PolyPIndex].x < MinX)
            MinX = Poly.Points[PolyPIndex].x;
        if (Poly.Points[PolyPIndex].y < MinY)
            MinY = Poly.Points[PolyPIndex].y;
    }

    return (V2(MinX, MinY));
}

inline v2
FindMaxPolyP(poly Poly)
{
    real32 MaxX = Poly.Points[0].x, MaxY = Poly.Points[0].y;
    for (u32 PolyPIndex = 0; PolyPIndex < Poly.PointCount; PolyPIndex++)
    {
        if (Poly.Points[PolyPIndex].x > MaxX)
            MaxX = Poly.Points[PolyPIndex].x;
        if (Poly.Points[PolyPIndex].y > MaxY)
            MaxY = Poly.Points[PolyPIndex].y;
    }

    return (V2(MaxX, MaxY));
}
#endif