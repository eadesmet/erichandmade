
internal line_collision_result
CheckCollisionLine(game_state *GameState, v2 P1, v2 P2)
{
    line_collision_result Result = {};
    Result.CollisionP = P1;
    Result.NewEndP = P2;

    CheckCollisionLine_GameBounds(GameState, &Result, P1, P2);

#if 0
    // Checking collision with bounding_box in GameState
    bounding_box TestBox = GameState->TestCollisionBox;
    line_collision_result BoxResult = CheckCollisionBoundingBox(P1, P2, TestBox);
    if (BoxResult.IsColliding && BoxResult.CollisionP != P1)
    {
        Result = BoxResult;
    }
#else


    poly TestPoly = GameState->TestPoly;
    line_collision_result PolyResult = CheckCollisionPoly(GameState->ScreenCenter, P1, P2, TestPoly);
    if (PolyResult.IsColliding && PolyResult.CollisionP != P1)
    {
        Result = PolyResult;
    }
#endif

    return(Result);
}

internal void
WalkLineCheckCollision(game_state *GameState, render_buffer* Render, v2 P1, v2 P2, u32 Thickness, v3 Color)
{
    // NOTE(Eric): P1 MUST be the source point! Walking the line FROM P1 to P2 - That's the direction!

    line_collision_result CollisionResult = CheckCollisionLine(GameState, P1, P2);
    if (CollisionResult.IsColliding)
    {
        RenderSquare(Render, CollisionResult.CollisionP, 3, V3(1,1,0));

        RenderLine(Render, P1, CollisionResult.CollisionP, Thickness, Color);
        WalkLineCheckCollision(GameState, Render, CollisionResult.CollisionP, CollisionResult.NewEndP, Thickness, Color);
    }
    else
    {
        RenderLine(Render, CollisionResult.CollisionP, CollisionResult.NewEndP, Thickness, Color);
    }

}

internal void
CastAndRenderPlayerLine(game_state *GameState, render_buffer* Render, u32 Thickness, v3 Color)
{
    v2 ScreenP1 = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);

    real32 RayLength = 1000;

    v2 ScreenP2 = V2((RayLength * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                     (RayLength * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GamePointToScreenPoint(GameState->ScreenCenter, GameState->Player.CenterP);

    //RenderLine(Render, ScreenP1, ScreenP2, Thickness, Color);
    WalkLineCheckCollision(GameState, Render, ScreenP1, ScreenP2, Thickness, Color);
}
