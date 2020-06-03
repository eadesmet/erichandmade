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

struct line_collision_result
{
    bool32 IsColliding;
    v2 NewEndP;
    v2 CollisionP;
};

inline line_collision_result
CheckCollisionLine(game_state *GameState, v2 P1, v2 P2)
{
    // y = mx + b
    // m = slope of the line
    // m = y2 - y1 / x2 - x1;
    // b = y intercept : where on the y axis the line intercepts
    /*
So we have the two points, and the render size. I think we can calculate the collision point
instead of walking the line point by point.

Solve for X = 0 OR X = RenderWidth
Solve for Y = 0 OR Y = RenderHeight
whichever calculation works out is our collision point

*/
#if 1
    line_collision_result Result = {};
    Result.CollisionP = P1;
    Result.NewEndP = P2;
    
    real32 m = 0, b = 0, XAtZeroY = 0, XAtMaxY = 0, YAtMaxX = 0;
    if (P2.x - P1.x == 0)
    {
        // Slope is undefined
        // The y-intercept doesn't exist because it's parallel
        // X is always the same
        b = P1.y;
        XAtZeroY = P1.x;
        XAtMaxY = P1.x;
        YAtMaxX = P1.y;
    }
    else
    {
        m = Slope(P1, P2);
        
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
    }
    
    // FIRST AND SECOND ATTEMPT HERE: Left in, I might put them in the blog
    /*
                // Now that we have m and b of the line, plug in points along the edges for collision point?
                // Each line has 2 collisions, so we also have to determine which edge P2 is outside of
                if ((b >= 0 && b <= GameState->RenderHeight) &&
                    P2.x <= 0)
                {
                    // Colliding with left edge, within bounds at (0,b)
                    Result.CollisionP = V2(0, b);
                    Result.NewEndP = V2(AbsoluteValue(P2.x), P2.y);
                    Result.IsColliding = true;
                }
                else if ((XAtZeroY >= 0 && XAtZeroY <= GameState->RenderWidth)
                         && P2.y <= 0)
                {
                    // Colliding with bottom edge, within bounds at (XAtZeroY,0)
                    Result.CollisionP = V2(XAtZeroY, 0);
                    Result.NewEndP = V2(P2.x, AbsoluteValue(P2.y));
                    Result.IsColliding = true;
                }
                else if ((XAtMaxY >= 0 && XAtMaxY <= GameState->RenderWidth)
                         && P2.y >= GameState->RenderHeight)
                {
                    // Colliding with top edge, within bounds at (XAtMaxY,RenderHeight)
                    Result.CollisionP = V2(XAtMaxY, (real32)GameState->RenderHeight);
                    Result.NewEndP = V2(P2.x, P2.y - 2 * (P2.y - GameState->RenderHeight));
                    Result.IsColliding = true;
                }
                else if ((YAtMaxX >= 0 && YAtMaxX <= GameState->RenderHeight)
                         && P2.x >= GameState->RenderWidth)
                {
                    // Colliding with right edge, within bounds at (RenderWidth,YAtMaxX)
                    Result.CollisionP = V2((real32)GameState->RenderWidth, YAtMaxX);
                    Result.NewEndP = V2(P2.x - 2 * (GameState->RenderWidth - P2.x), P2.y);
                    Result.IsColliding = true;
                }
                */
    /*
    // END OF 5/18/20 STOPPED HERE:
    // I think we are almost there..
    // Coming back to this later with a fresh mind will help.
    
    // This only works if the first point is the Center!
    v2 Center = V2((real32)GameState->RenderWidth/2, (real32)GameState->RenderHeight/2);
    v2 TopRight = V2((real32)GameState->RenderWidth, (real32)GameState->RenderHeight);
    real32 DiagonalSlope = Slope(Center, TopRight);
    
    // This isn't quite right either 
    /*
    v2 TopRight = V2(P1.x+1.0f, P1.y+1.0f);
    real32 DiagonalSlope = Slope(P1, TopRight); /
    
    bool32 Steep = ((m > DiagonalSlope) || (m < -DiagonalSlope));
    
    bool32 Above = P2.y >= GameState->RenderHeight && Steep;
    bool32 Below = P2.y <= 0 && Steep;
    
    bool32 Right = P2.x >= GameState->RenderWidth && !Steep;
    bool32 Left = P2.x <= 0 && !Steep;
    */
    
    // My problem is when it's both points are colliding
    // I need to make sure it still picks up the second point
    
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
    
    return(Result);
    
#else
    
    
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
    
#endif
    
}

inline void
WalkLineCheckCollision(game_state *GameState, render_buffer* Render, v2 P1, v2 P2, u32 Thickness, v3 Color)
{
    // NOTE(Eric): P1 MUST be the source point! Walking the line FROM P1 to P2 - That's the direction!
#if 1
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
#else
    
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
#endif
}

inline void
CastAndRenderPlayerLine(game_state *GameState, render_buffer* Render, u32 Thickness, v3 Color)
{
    v2 ScreenP1 = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    
    real32 RayLength = 10000;
    
    v2 ScreenP2 = V2((RayLength * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                     (RayLength * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GamePointToScreenPoint(GameState, GameState->Player.CenterP);
    
    //RenderLine(Render, ScreenP1, ScreenP2, Thickness, Color);
    WalkLineCheckCollision(GameState, Render, ScreenP1, ScreenP2, Thickness, Color);
}
