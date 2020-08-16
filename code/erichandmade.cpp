
#include "erichandmade.h"
#include "random.h"
#include "tile.cpp"
#include "render.cpp"
#include "collision.cpp"
#include "walkline.cpp"

//~ NOTE(Eric): Init functions
internal void
InitPlayer(game_state *GameState)
{
    player Result = {};
    Result.CenterP = V2(0,0);//V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    Result.FacingDirectionAngle = 0;
    Result.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(Result.FacingDirectionAngle * Pi32/180)),
                       RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(Result.FacingDirectionAngle * Pi32/180))) + Result.CenterP;

    GameState->Player = Result;
}

internal asteroid
InitAsteroidA(v2 StartP, v2 EndP)
{
    asteroid A = {};
    A.StartP = StartP;
    A.CenterP = StartP;
    A.EndP = EndP;
    A.Points[0] = V2(0, 2); // Relative to the Center.
    A.Points[1] = V2(1, 3);
    A.Points[2] = V2(2, 0); // front
    A.Points[3] = V2(0, -2);
    A.Points[4] = V2(-2, 1);
    A.Points[5] = V2(-1, 1); // Then 5 connects to 0
    A.PointCount = 6;

    A.State = AsteroidState_Active;
    A.Speed = 30;
    A.Color = V3(1,1,1);

    return(A);
}

internal asteroid
InitAsteroidB(v2 StartP, v2 EndP)
{
    asteroid A = {};
    A.StartP = StartP;
    A.CenterP = StartP;
    A.EndP = EndP;
    A.Points[0] = V2(0, 4); // Relative to the Center.
    A.Points[1] = V2(1, 3);
    A.Points[2] = V2(2, 4);
    A.Points[3] = V2(4, 2);
    A.Points[4] = V2(3, 0);
    A.Points[5] = V2(3, -1);
    A.Points[6] = V2(2, -2);
    A.Points[7] = V2(3, -4);
    A.Points[8] = V2(1, -5);
    A.Points[9] = V2(-1, -3);
    A.Points[10] = V2(-2, -1);
    A.Points[11] = V2(-1, 3);
    A.PointCount = 12;

    A.State = AsteroidState_Active;
    A.Speed = 30;
    A.Color = V3(1,1,1);

    return(A);
}
#if 0
internal asteroid
InitAsteroidC()
{
    asteroid A = {};

    A.Points[0] = V2(0, 2); // Relative to the Center.
    A.Points[1] = V2(1, 3);
    A.Points[2] = V2(2, 0); // front
    A.Points[3] = V2(0, -2);
    A.Points[4] = V2(-2, 1);
    A.Points[5] = V2(-1, 1); // Then 5 connects to 0
    A.PointCount = 6;

    A.State = AsteroidState_Active;
    A.Speed = 30;
    A.Color = V3(1,1,1);

    return(A);
}
#endif

internal void
InitAsteroids(game_state *GameState)
{
    // TODO(Eric): Randomize all positions! (and size, and speed)
    random_series Series = RandomSeed(321);

    v2 CenterMap = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    v2 RenderMax = V2((real32)GameState->RenderWidth, (real32)GameState->RenderHeight);

    v2 P1 = V2(5, 0);
    v2 P2 = V2(50, 0);

    v2 P3 = V2(15, 0);
    v2 P4 = V2(50, 0);

    // TODO(Eric): Come up with a scheme to phase asteroids in and out of play.
    // We have the active/inactive, but we need to be constantly updating this array
    // and reusing slots.
    GameState->Asteroids[0] = InitAsteroidA(P1, P2);
    GameState->AsteroidCount++;

    GameState->Asteroids[1] = InitAsteroidB(P3, P4);
    GameState->AsteroidCount++;

}

internal void
MoveAsteroid(asteroid *Asteroid, real32 dt)
{
    v2 DeltaP = GamePointToScreenPoint(Asteroid->EndP - Asteroid->CenterP);
    real32 DistanceRemaining = Length(DeltaP);

    if (DistanceRemaining > 1)
    {
#if 1

        // TODO(Eric): Rewatch HH movement code to see what dP is and why we need it!
        // I think we do need it
        /*
        v2 DeltaNormal = Normalize(DeltaP);
        v2 AstDelta = (0.5f*Square(dt) + dP*dt)
        */
        // TODO(Eric): Better movement code!
        // Even if there is only a change in 1 axis, it still moves in both! (can't move straight horizontal)
        v2 ddP = {};
        ddP.x = (DeltaP.x)/(dt*600000);
        ddP.y = (DeltaP.y)/(dt*600000);

        real32 MoveX = ddP.x * (dt * Asteroid->Speed);
        real32 MoveY = ddP.y * (dt * Asteroid->Speed);

        Asteroid->CenterP.x += (real32)(MoveX);
        Asteroid->CenterP.y += (real32)(MoveY);
#else
        Asteroid->CenterP = Mix(Asteroid->CenterP, Asteroid->EndP, .03f);
#endif
    }
    else
    {
        Asteroid->CenterP = Asteroid->EndP;
        Asteroid->State = AsteroidState_Inactive;
    }
}


//~NOTE(Eric): Game Update and Render

// (render_buffer *Render, game_memory *Memory, game_input *Input)
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

    // TODO(Eric): Come up with a way to use TransientStorage (SimRegions? Phasing Asteroids in and out?)

    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if (!Memory->IsInitialized)
    {
        GameState->RenderWidth = Render->Width;
        GameState->RenderHeight = Render->Height;
        GameState->RenderHalfWidth = (real32)GameState->RenderWidth * 0.5f;
        GameState->RenderHalfHeight = (real32)GameState->RenderHeight * 0.5f;
        ScreenCenter = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);

        InitMap(GameState);
        InitPlayer(GameState);

        GameState->AsteroidCount = 0;
        InitAsteroids(GameState);

        Memory->IsInitialized = true;
    }

    if (GameState->RenderWidth != Render->Width ||
        GameState->RenderHeight != Render->Height)
    {
        GameState->RenderWidth = Render->Width;
        GameState->RenderHeight = Render->Height;
        GameState->RenderHalfWidth = (real32)GameState->RenderWidth * 0.5f;
        GameState->RenderHalfHeight = (real32)GameState->RenderHeight * 0.5f;
        ScreenCenter = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);

        InitMap(GameState);
        GameState->Player.CenterP = V2(0,0); //V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
        GameState->Player.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                                      RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GameState->Player.CenterP;
    }

    game_controller_input *Controller = GetController(Input, 0);
    v2 ddP = {};
    // TODO(Eric): Player Movement does NOT feel good, and is based on framerate
    real32 PlayerAngleMoveAmount = 1;
    if(Controller->MoveUp.EndedDown)
    {
        GameState->Player.FacingDirectionAngle -= PlayerAngleMoveAmount;
        ddP.y = 1.0f;
    }
    if(Controller->MoveDown.EndedDown)
    {
        GameState->Player.FacingDirectionAngle += PlayerAngleMoveAmount;
        ddP.y = -1.0f;
    }
    if(Controller->MoveLeft.EndedDown)
    {
        GameState->Player.FacingDirectionAngle += PlayerAngleMoveAmount;
        ddP.x = -1.0f;
    }
    if(Controller->MoveRight.EndedDown)
    {
        GameState->Player.FacingDirectionAngle -= PlayerAngleMoveAmount;
        ddP.x = 1.0f;
    }

    if (GameState->Player.FacingDirectionAngle > 360.0f)
    {
        GameState->Player.FacingDirectionAngle = PlayerAngleMoveAmount;
    }
    else if (GameState->Player.FacingDirectionAngle < 0.0f)
    {
        GameState->Player.FacingDirectionAngle = 360.0f - PlayerAngleMoveAmount;
    }


    // Update players position
    GameState->Player.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                                  RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GameState->Player.CenterP;

    // Update Asteroids
    for (u32 AsteroidIndex = 0;
         AsteroidIndex < GameState->AsteroidCount;
         AsteroidIndex++)
    {
        asteroid *Ast = GameState->Asteroids + AsteroidIndex;

        MoveAsteroid(Ast, Input->dtForFrame);
    }

    //
    // NOTE(Eric): Render
    //
    ClearBackground(Render);
    RenderMap(GameState, Render, &GameState->Map);
    RenderPlayer(GameState, Render, &GameState->Player, &GameState->Map);


    // Render Asteroids
    for (u32 AsteroidIndex = 0;
         AsteroidIndex < GameState->AsteroidCount;
         AsteroidIndex++)
    {
        asteroid *Ast = GameState->Asteroids + AsteroidIndex;
        RenderAsteroid(Render, &GameState->Map, Ast);
    }



    // "Ray" from the player's front
    CastAndRenderPlayerLine(GameState, Render, 1, V3(.3, .8, .8));

}


