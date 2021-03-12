

internal bool32
CheckEdge(v2 V0, v2 V1, v2 P)
{
    // TODO(Eric): This function needs to assume the 'edge' is counter-clockwise
    // I _think_ that means that V1 must be greater than V0
    if (V0 > V1)
    {
        v2 Temp = V0;
        V0 = V1;
        V1 = Temp;
    }

    real32 Result = (V0.y - V1.y)*P.x + (V1.x - V0.x)*P.y + (V0.x*V1.y - V0.y*V1.x);

    if (Result >= 0) // If it's 0, it lies on the edge
        return true;
    return false;
}

// TODO(Eric): These two aren't used yet
// But, I think can be extended to any polygon?
// Or maybe convert/test all my asteroids into/as a bunch of triangles?

internal bool32
CheckP_WithinTriangle(v2 V0, v2 V1, v2 V2, v2 P)
{
    bool32 Result = false;

    Result = (CheckEdge(V0, V1, P) &&
              CheckEdge(V1, V2, P) &&
              CheckEdge(V2, V0, P));

    return(Result);
}

internal bool32
CheckCollisionCircles(v2 CenterA, real32 RadiusA, v2 CenterB, real32 RadiusB)
{
    bool32 Result = false;

    real32 DeltaX = CenterB.x - CenterA.x;
    real32 DeltaY = CenterB.y - CenterA.y;

    real32 Distance = SquareRoot(DeltaX*DeltaX + DeltaY*DeltaY); // NOTE(Eric): This is Inner (v2)

    if (Distance <= ((RadiusA) + (RadiusB)))
        Result = true;

    return(Result);
}

internal line_collision_result
CheckCollisionLines(v2 A, v2 B, v2 C, v2 D)
{
    // Checks collision between two lines (AB and CD)
    line_collision_result Result = {};

    real32 Denom = (((D.y - C.y) * (B.x - A.x)) - ((D.x - C.x) * (B.y - A.y)));
    real32 NumeratorA = (((D.x - C.x) * (A.y - C.y)) - ((D.y - C.y) * (A.x - C.x)));
    real32 NumeratorB = (((B.x - A.x) * (A.y - C.y)) - ((B.y - A.y) * (A.x - C.x)));

    if (Denom == 0.0f)
    {
        // Lines are parallel and don't collide
        return(Result);
    }

    real32 Ua = NumeratorA / Denom;
    real32 Ub = NumeratorB / Denom;

    if (Ua == Ub)
    {
        // Lines are conincident (they are exactly the same line)
        return(Result);
    }

    if (Ua < 0 || Ua > 1 || Ub < 0 || Ub > 1)
    {
        // The collision point isn't along either line segment
        return(Result);
    }

    // Collision point
    real32 CollisionX = A.x + (Ua * (B.x - A.x));
    real32 CollisionY = A.y + (Ua * (B.y - A.y));

    Result.IsColliding = true;
    Result.CollisionP = V2(CollisionX, CollisionY);

    return(Result);
}

internal line_collision_result
CheckCollisionBoundingBox(v2 A, v2 B, bounding_box Box)
{
    // Checks collision between line AB and a Rectangle
    // Result CollisionP is first hit
    line_collision_result Result = {};

    v2 Min = Box.Min;
    v2 Max = Box.Max;
    v2 TopLeft = V2(Min.x, Max.y);
    v2 BottomLeft = Min;
    v2 TopRight = Max;
    v2 BottomRight = V2(Max.x, Min.y);

    line_collision_result LeftBoxResult = CheckCollisionLines(A, B, BottomLeft, TopLeft);
    line_collision_result RightBoxResult = CheckCollisionLines(A, B, BottomRight, TopRight);
    line_collision_result TopBoxResult = CheckCollisionLines(A, B, TopLeft, TopRight);
    line_collision_result BottomBoxResult = CheckCollisionLines(A, B, BottomLeft, BottomRight);


    if (LeftBoxResult.IsColliding)
    {
        // Colliding with left edge of box - line is going right
        Result.CollisionP = LeftBoxResult.CollisionP;
        Result.NewEndP = V2(B.x - 2 * (B.x - Result.CollisionP.x), B.y);
        Result.IsColliding = true;
    }

    if (RightBoxResult.IsColliding)
    {
        // Colliding with right edge of box - line is going left

        // If the previous collision is further away, then this one is closer
        if (Length(Result.CollisionP - A) > Length(RightBoxResult.CollisionP - A))
        {
            Result.CollisionP = RightBoxResult.CollisionP;
            Result.NewEndP = V2(AbsoluteValue(B.x), B.y);
            Result.IsColliding = true;
        }

    }

    if (TopBoxResult.IsColliding)
    {
        // Colliding with top edge of box - line is going down
        if (Length(Result.CollisionP - A) > Length(TopBoxResult.CollisionP - A))
        {
            Result.CollisionP = TopBoxResult.CollisionP;
            Result.NewEndP = V2(B.x, AbsoluteValue(B.y)); // Absolute value is wrong here - it was assuming neg (below bounds)
            Result.IsColliding = true;
        }
    }

    if (BottomBoxResult.IsColliding)
    {
        // Colliding with bottom edge of box - line is going up
        if (Length(Result.CollisionP - A) > Length(BottomBoxResult.CollisionP - A))
        {
            Result.CollisionP = BottomBoxResult.CollisionP;
            Result.NewEndP = V2(B.x, B.y - 2 * (B.y - Result.CollisionP.y));
            Result.IsColliding = true;
        }
    }

    return(Result);
}

internal line_collision_result
CheckCollision_LineAsteroid(v2 A, v2 B, entity Asteroid)
{
    // Result CollisionP is first hit
    line_collision_result Result = {};

    v2 ClosestP = {};
    for (u32 Iteration = 0;
         Iteration <= 2;
         Iteration++)
    {
        for (u32 PointIndex = 0;
             PointIndex < Asteroid.PointCount;
             PointIndex++)
        {
            v2 P1 = {};
            v2 P2 = {};
            if (PointIndex != Asteroid.PointCount - 1)
            {
                P1 = GamePointToScreenPoint(Asteroid.CenterP + *(Asteroid.Points + PointIndex));
                P2 = GamePointToScreenPoint(Asteroid.CenterP + *(Asteroid.Points + PointIndex + 1));
            }
            else
            {
                P1 = GamePointToScreenPoint(Asteroid.CenterP + *(Asteroid.Points + PointIndex));
                P2 = GamePointToScreenPoint(Asteroid.CenterP + *Asteroid.Points);
            }

            line_collision_result LinesResult = CheckCollisionLines(A, B, P1, P2);
            if (LinesResult.IsColliding)
            {
                Result.IsColliding = LinesResult.IsColliding;

                real32 CollisionLength = Length(LinesResult.CollisionP);
                real32 CurrentClosestLength = Length(ClosestP);

                if (CollisionLength <= CurrentClosestLength || ClosestP == V2(0,0))
                {
                    Result.CollisionP = LinesResult.CollisionP;
                }
                Result.NewEndP = LinesResult.CollisionP; // TODO(Eric): Calculate the new EndP!!!
            }
        }

    }
    return (Result);
}

internal bool32
CheckCollision_AsteroidPlayer(entity *Ast, player *Player)
{
    bool32 Result = false;

    for (u32 PointIndex = 0;
         PointIndex < Ast->PointCount;
         PointIndex++)
    {
        v2 P1 = {};
        v2 P2 = {};
        if (PointIndex != Ast->PointCount - 1)
        {
            P1 = GamePointToScreenPoint(Ast->CenterP + *(Ast->Points + PointIndex));
            P2 = GamePointToScreenPoint(Ast->CenterP + *(Ast->Points + PointIndex + 1));
        }
        else
        {
            P1 = GamePointToScreenPoint(Ast->CenterP + *(Ast->Points + PointIndex));
            P2 = GamePointToScreenPoint(Ast->CenterP + *Ast->Points);
        }

        v2 Screen_PlayerBackLeftP = GamePointToScreenPoint(Player->BackLeftP, false);
        v2 Screen_PlayerBackRightP = GamePointToScreenPoint(Player->BackRightP, false);
        v2 Screen_PlayerFrontP = GamePointToScreenPoint(Player->FrontP, false);
#if 0
        line_collision_result LineResult = {};
        LineResult = CheckCollisionLines(P1, P2, Screen_PlayerBackLeftP, Screen_PlayerBackRightP);
        if (LineResult.IsColliding) Assert(false);
        LineResult = CheckCollisionLines(P1, P2, Screen_PlayerBackLeftP, Screen_PlayerFrontP);
        if (LineResult.IsColliding) Assert(false);
        LineResult = CheckCollisionLines(P1, P2, Screen_PlayerBackRightP, Screen_PlayerFrontP);
        if (LineResult.IsColliding) Assert(false);
#endif
    }

    return(Result);
}

internal asteroid_collision_result
CheckCollision_AsteroidAsteroid(entity *Ast, entity *AstTwo)
{
    asteroid_collision_result Result = {};

    // NOTE(Eric): Checking if each line of the first Asteroid collides with the Second
    for (u32 PointIndex = 0;
         PointIndex < Ast->PointCount;
         PointIndex++)
    {
        line_collision_result LineResult = {};

        if (PointIndex != Ast->PointCount - 1)
        {
            v2 P1 = GamePointToScreenPoint(Ast->CenterP + *(Ast->Points + PointIndex));
            v2 P2 = GamePointToScreenPoint(Ast->CenterP + *(Ast->Points + PointIndex + 1));
            LineResult = CheckCollision_LineAsteroid(P1, P2, *AstTwo);
        }
        else
        {
            v2 P1 = GamePointToScreenPoint(Ast->CenterP + *(Ast->Points + PointIndex));
            v2 P2 = GamePointToScreenPoint(Ast->CenterP + *Ast->Points);
            LineResult = CheckCollision_LineAsteroid(P1, P2, *AstTwo);
        }

        if (LineResult.IsColliding)
        {
            Result.IsColliding = LineResult.IsColliding;
            Result.CollisionP = LineResult.CollisionP;
            break;
        }
    }

    return(Result);
}

internal void
HandleAsteroidColissions(game_state *GameState, entity *TestAsteroid, real32 dt)
{
    // TODO(Eric): Also check whether they are inside game bounds
    // NOTE(Eric): This also has to account for whether it will eventually reach in-bounds
    //bool32 WithinGameBounds = IsWithinGameBounds(Asteroid);

    // NOTE(Eric): Some distance that will narrow down what Asteroids can collide with other Asteroids
    v2 MaxAsteroidSize = V2(9.0f, 9.0f);

    if (TestAsteroid->Type != EntityType_Asteroid) return; //NOTE(Eric): This is now redundant
    if (TestAsteroid->State == EntityState_InActive) return;

    bool32 IsCollidingPlayer = CheckCollision_AsteroidPlayer(TestAsteroid, &GameState->Player);
    if (IsCollidingPlayer)
    {
        // NOTE(Eric): Hit the player - Game over????
    }

    for (u32 AsteroidTwoIndex = 0;
         AsteroidTwoIndex < GameState->EntityCount;
         AsteroidTwoIndex++)
    {
        entity *AstTwo = GameState->Entities + AsteroidTwoIndex;

        if (AstTwo->Type != EntityType_Asteroid) continue;
        if (AstTwo == TestAsteroid) continue;
        if (AstTwo->State == EntityState_InActive) continue;
        if (TestAsteroid->CenterP - AstTwo->CenterP > MaxAsteroidSize) continue;

        asteroid_collision_result CollisionResult = CheckCollision_AsteroidAsteroid(TestAsteroid, AstTwo);
        if (CollisionResult.IsColliding)
        {
            // TODO(Eric): IsColliding is never actually used, just remove it
            TestAsteroid->IsColliding = true;
            AstTwo->IsColliding = true;

            v2 TestAsteroidOrig = TestAsteroid->Velocity;
            TestAsteroid->Velocity = Normalize(AstTwo->Velocity) * TestAsteroid->Speed;
            AstTwo->Velocity = Normalize(TestAsteroidOrig) * AstTwo->Speed;


            //bounding_box CollisionPointBox = {};
            //CollisionPointBox.Min = CollisionResult.CollisionP;
            //CollisionPointBox.Max = CollisionResult.CollisionP + V2(1,1);
            //AddDebugRenderBox(GameState, CollisionPointBox);

            break;
        }
        else
        {
            TestAsteroid->IsColliding = false;
        }
    }

}

internal void
CheckCollisionLine_GameBounds(game_state *GameState, line_collision_result *Result, v2 P1, v2 P2)
{
    real32 m = 0, b = 0, XAtZeroY = 0, XAtMaxY = 0, YAtMaxX = 0;
    real32 ChangeInX = 0, ChangeInY = 0;
    if (P2.x - P1.x == 0)
    {
        // Slope is undefined
        // The y-intercept doesn't exist because it's parallel
        // X is always the same
        b = P1.y;
        XAtZeroY = P1.x;
        XAtMaxY = P1.x;
        YAtMaxX = P1.y;

        ChangeInX = 0;
        ChangeInY = P2.y - P1.y; // This is probably incorrect.?
    }
    else
    {
        m = Slope(P1, P2);

        ChangeInX = P2.x - P1.x;
        ChangeInY = P2.y - P1.y;
        // ChangeInY * x - ChangeInX * y + ChangeInX * b = 0
        // X intercept
        real32 C = ChangeInX * m;
        // Solve for X:
        // 0 = ChangeInY * x - C
        // C = ChangeInY * X
        // C / ChangeInY = X
        real32 XIntercept = C / ChangeInY;

        // Solve for Y:
        // 0 = -(ChangeInX * Y) + C
        // -C = -(ChangeInX * Y)
        // C = ChangeInX * Y
        // C / ChangeInX = Y
        real32 YIntercept = C / ChangeInX;

        // Solve for Y when Max X:
        // 0 = (ChangeInY * MaxX) -  (ChangeInX * Y) + C
        // -(ChangeInY * MaxX) - C = -(ChangeInX * Y)
        // -(ChangeInY * MaxX) - C / -ChangeInX = Y
        real32 MaxX_Y = (-(ChangeInY * GameState->RenderWidth) - C) / -ChangeInX;

        // Solve for X when Max Y:
        // 0 = (ChangeInY * X) -  (ChangeInX * MaxY) + C
        // ChangeInX * MaxY - C = ChangeInY * X
        // ChangeInX * MaxY - C / ChangeInY = X
        real32 MaxY_X = ((ChangeInX * GameState->RenderHeight - C) / ChangeInY);



        //--------------

        // P1.y = m(P1.x) + b
        // P1.y - m(P1.x) = b
        b = P1.y - (m * P1.x); // Point (0,b) = y-intercept

        // Y = 0, solve for X:
        // 0 = mx + b
        // -b = mx
        // -b/m = x
        XAtZeroY = -b/m;

        // Y = RenderHeight, solve for X:
        // Height = mx + b
        // Height-b = mx
        // (Height-b)/m = x
        XAtMaxY = (GameState->RenderHeight - b) / m;

        // X = RenderWidth, solve for Y:
        // Y = m(RenderWidth) + b
        YAtMaxX = (m * GameState->RenderWidth) + b;

        bool test = false;
    }

    bool32 Above = XAtMaxY >= 0 && XAtMaxY <= GameState->RenderWidth && P1.y < P2.y && P2.y >= GameState->RenderHeight;
    bool32 Below = XAtZeroY >= 0 && XAtZeroY <= GameState->RenderWidth && P1.y > P2.y && P2.y <= 0;

    bool32 Right = YAtMaxX >= 0 && YAtMaxX <= GameState->RenderHeight && P1.x < P2.x && P2.x >= GameState->RenderWidth;
    bool32 Left = b >= 0 && b <= GameState->RenderHeight && P1.x > P2.x && P2.x <= 0;




    if (Above)
    {
        // Colliding with Top
        v2 NewCollisionP = V2(XAtMaxY, (real32)GameState->RenderHeight);

        Result->CollisionP = NewCollisionP;
        Result->NewEndP = V2(P2.x, P2.y - 2 * (P2.y - GameState->RenderHeight));
        Result->IsColliding = true;
    }
    else if (Below)
    {
        // Colliding with Bottom
        v2 NewCollisionP = V2(XAtZeroY, 0);

        Result->CollisionP = NewCollisionP;
        Result->NewEndP = V2(P2.x, AbsoluteValue(P2.y));
        Result->IsColliding = true;
    }
    else if (Left)
    {
        // Colliding with Left
        v2 NewCollisionP = V2(0, b);

        Result->CollisionP = NewCollisionP;
        Result->NewEndP = V2(AbsoluteValue(P2.x), P2.y);
        Result->IsColliding = true;
    }
    else if (Right)
    {
        // Colliding with Right
        v2 NewCollisionP = V2((real32)GameState->RenderWidth, YAtMaxX);

        Result->CollisionP = NewCollisionP;
        Result->NewEndP = V2(P2.x - 2 * (P2.x - GameState->RenderWidth), P2.y);
        Result->IsColliding = true;
    }
}
