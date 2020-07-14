struct line_collision_result
{
    bool32 IsColliding;
    v2 NewEndP;
    v2 CollisionP;
};

inline line_collision_result
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
        Result.IsColliding = false;
        return(Result);
    }

    real32 Ua = NumeratorA / Denom;
    real32 Ub = NumeratorB / Denom;

    if (Ua == Ub)
    {
        // Lines are conincident (they are exactly the same line)
        Result.IsColliding = false;
        return(Result);
    }

    // Collision point
    real32 CollisionX = A.x + (Ua * (B.x - A.x));
    real32 CollisionY = A.y + (Ua * (B.y - A.y));

    Result.IsColliding = true;
    Result.CollisionP = V2(CollisionX, CollisionY);

    return(Result);

#if 0
    v2 CmP = C-A;
    v2 r = B-A;
    v2 s = D-C;

    real32 CmPxr = (CmP.x * r.y) - (CmP.y * r.x);
    real32 CmPxs = (CmP.x * s.y) - (CmP.y * s.x);
    real32 rxs = r.x * s.y - r.y * s.x;

    if (CmPxr == 0.0f)
    {
        // Lines are collinear
        Result.IsColliding = ((C.x - A.x < 0.0f) != (C.x - B.x < 0.0f)) || ((C.y - A.y < 0.0f) != (C.y - B.y < 0.0f));
    }

    if (rxs == 0.0f)
    {
        Result.IsColliding = false; // Lines are parallel
    }

    real32 rxsr = 1.0f / rxs;
    real32 t = CmPxs * rxsr;
    real32 u = CmPxr * rxsr;

    v2 CollidingP = A + t*r;
    Result.IsColliding = (t >= 0.0f) && (t <= 1.0f) && (u >= 0.0f) && (u <= 1.0f)
                && CollidingP != A; // To avoid collision with first point being counted

    Result.CollisionP = CollidingP;
    return (Result);
#endif

    
}

inline line_collision_result
CheckCollisionBoundingBox(v2 A, v2 B, bounding_box Box)
{
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

    // TODO(Eric): Somehow check which collision is the closest to A (without a ton of logic)
    // Assumptions we can make:
    //    a line intersecting a box will always hit two points
    //      (unless the line is short and would have ended inside the box....)
    //    The direction of the line will determine which two sides are possible?
    //       down-right: left+bottom or top+right - so it would be left or top, and it can't be both of those.
    //                     or left+right..

    // (7/14): Really, it's whichever coordinate is closer to the source point
    // if left.X is closer to A.x than right.X, it's hitting left side first (horizontally)
    // Then check if the y is loser to A.x
    // Then it's a matter of checking which is closer, horizontally or vertically    
    // If it's closer to left.x, then if the collision point is greater, than its the top or bottom, NOT left


    // TODO(ERic): End of 7/9: stopped here. See TODO above
    v2 Norm = Normalize(B-A);
    // (-,-) : down left
    // (+,+) : up right
    if (Norm.x <= 0 && Norm.y <= 0)
    {
        // down left - colliding with top or right
        if (TopBoxResult.IsColliding)
        {
            // Colliding with top edge of box - line is going down
            Result.CollisionP = TopBoxResult.CollisionP;
            Result.NewEndP = V2(B.x, AbsoluteValue(B.y));
            Result.IsColliding = true;
        }

    }

    


    if (LeftBoxResult.IsColliding)
    {
        // Colliding with left edge of box - line is going right
        Result.CollisionP = LeftBoxResult.CollisionP;
        Result.NewEndP = V2(B.x - 2 * (B.x - Result.CollisionP.x), B.y);
        Result.IsColliding = true;
    }
    else if (RightBoxResult.IsColliding)
    {
        // Colliding with right edge of box - line is going left
        Result.CollisionP = RightBoxResult.CollisionP;
        Result.NewEndP = V2(AbsoluteValue(B.x), B.y);
        Result.IsColliding = true;
    }
    else if (TopBoxResult.IsColliding)
    {
        // Colliding with top edge of box - line is going down
        Result.CollisionP = TopBoxResult.CollisionP;
        Result.NewEndP = V2(B.x, AbsoluteValue(B.y));
        Result.IsColliding = true;
    }
    else if (BottomBoxResult.IsColliding)
    {
        // Colliding with bottom edge of box - line is going up
        Result.CollisionP = BottomBoxResult.CollisionP;
        Result.NewEndP = V2(B.x, B.y - 2 * (B.y - Result.CollisionP.y));
        Result.IsColliding = true;
    }

    return(Result);
}

inline line_collision_result
CheckCollisionLine(game_state *GameState, v2 P1, v2 P2)
{
    line_collision_result Result = {};
    Result.CollisionP = P1;
    Result.NewEndP = P2;
    
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
        
        Result.CollisionP = NewCollisionP;
        Result.NewEndP = V2(P2.x, P2.y - 2 * (P2.y - GameState->RenderHeight));
        Result.IsColliding = true;
    }
    else if (Below)
    {
        // Colliding with Bottom
        v2 NewCollisionP = V2(XAtZeroY, 0);
        
        Result.CollisionP = NewCollisionP;
        Result.NewEndP = V2(P2.x, AbsoluteValue(P2.y));
        Result.IsColliding = true;
    }
    else if (Left)
    {
        // Colliding with Left
        v2 NewCollisionP = V2(0, b);
        
        Result.CollisionP = NewCollisionP;
        Result.NewEndP = V2(AbsoluteValue(P2.x), P2.y);
        Result.IsColliding = true;
    }
    else if (Right)
    {
        // Colliding with Right
        v2 NewCollisionP = V2((real32)GameState->RenderWidth, YAtMaxX);
        
        Result.CollisionP = NewCollisionP;
        Result.NewEndP = V2(P2.x - 2 * (P2.x - GameState->RenderWidth), P2.y);
        Result.IsColliding = true;
    }

    // Checking collision with bounding_box in GameState
    bounding_box TestBox = GameState->TestCollisionBox;

    line_collision_result BoxResult = CheckCollisionBoundingBox(P1, P2, TestBox);
    if (BoxResult.IsColliding)
    {
        Result = BoxResult;
    }

    

    
    
    return(Result);
}

inline void
WalkLineCheckCollision(game_state *GameState, render_buffer* Render, v2 P1, v2 P2, u32 Thickness, v3 Color)
{
    // NOTE(Eric): P1 MUST be the source point! Walking the line FROM P1 to P2 - That's the direction!
    
    line_collision_result CollisionResult = CheckCollisionLine(GameState, P1, P2);
    if (CollisionResult.IsColliding)
    {
        RenderLine(Render, P1, CollisionResult.CollisionP, Thickness, Color);
        WalkLineCheckCollision(GameState, Render, CollisionResult.CollisionP, CollisionResult.NewEndP, Thickness, Color);
    }
    else
    {
        RenderLine(Render, CollisionResult.CollisionP, CollisionResult.NewEndP, Thickness, Color);
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
