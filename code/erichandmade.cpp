
#include "erichandmade.h"
#include "random.h"
#include "tile.cpp"
#include "render.cpp"

//~ NOTE(Eric): Init functions
internal void
InitPlayer(game_state *GameState)
{
    player Result = {};
    Result.CenterP = V2(0,0);//V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    Result.FacingDirectionAngle = 75;
    Result.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(Result.FacingDirectionAngle * Pi32/180)),
                       RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(Result.FacingDirectionAngle * Pi32/180))) + Result.CenterP;
    
    GameState->Player = Result;
}

internal void
InitAsteroids(game_state *GameState)
{
    // TODO(Eric): Randomize all positions! (and size, and speed)
    random_series Series = RandomSeed(321);
    
    v2 CenterMap = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    v2 RenderMax = V2((real32)GameState->RenderWidth, (real32)GameState->RenderHeight);
    
    v2 P1 = V2(100, 100);
    v2 P2 = V2(300, 200);
    v2 P3 = V2(100, 100);
    
    GameState->AsteroidCount = 0;
    
    asteroid Asteroid = {};
    Asteroid.CenterP = P1;
    Asteroid.StartP = Asteroid.CenterP;
    Asteroid.EndP = P2;
    Asteroid.Radius = 30;
    Asteroid.State = AsteroidState_Active;
    Asteroid.Speed = 30;
    Asteroid.Color = V3(1,1,1);
    GameState->Asteroids[0] = Asteroid;
    GameState->AsteroidCount++;
    
    Asteroid = {};
    Asteroid.CenterP = P2;
    Asteroid.StartP = Asteroid.CenterP;
    Asteroid.EndP = P2;
    Asteroid.Radius = 30;
    Asteroid.State = AsteroidState_Active;
    Asteroid.Speed = 30;
    Asteroid.Color = V3(1,1,1);
    GameState->Asteroids[1] = Asteroid;
    GameState->AsteroidCount++;
    /*
    Asteroid = {};
    Asteroid.CenterP = P2;
    Asteroid.StartP = Asteroid.CenterP;
    Asteroid.EndP = P1;
    Asteroid.Radius = ASTEROID_SMALL_R;
    Asteroid.State = ASTEROIDSTATE_ACTIVE;
    Asteroid.Speed = ASTEROIDSPEED_SLOW;
    Asteroid.Color = V3(1,1,1);
    GameState->Asteroids[2] = Asteroid;
    GameState->AsteroidCount++;
    */
}

internal void
MoveAsteroid(asteroid *Asteroid, real32 dt)
{
    v2 DeltaP = Asteroid->EndP - Asteroid->CenterP;
    real32 Distance = SquareRoot(LengthSq(DeltaP));
    
    if (Distance > 1)
    {
#if 1
        v2 ddP = {};
        
        ddP.x = (DeltaP.x/(dt*60));
        ddP.y = (DeltaP.y/(dt*60));
        
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

internal bool32
CheckCollisionAsteroids(asteroid Ast1, asteroid Ast2)
{
    bool32 Result = false;
    
    real32 DeltaX = Ast2.CenterP.x - Ast1.CenterP.x;
    real32 DeltaY = Ast2.CenterP.y - Ast1.CenterP.y;
    
    real32 Distance = SquareRoot(DeltaX*DeltaX + DeltaY*DeltaY);
    
    if (Distance <= ((Ast1.Radius) + (Ast2.Radius)))
        Result = true;
    
    return(Result);
}


//~NOTE(Eric): Game Update and Render

// (render_buffer *Render, game_memory *Memory, game_input *Input)
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if (!Memory->IsInitialized)
    {
        GameState->RenderWidth = Render->Width;
        GameState->RenderHeight = Render->Height;
        GameState->RenderHalfWidth = (real32)GameState->RenderWidth / 2.0f;
        GameState->RenderHalfHeight = (real32)GameState->RenderHeight / 2.0f;
        
        InitMap(GameState);
        InitPlayer(GameState);
        InitAsteroids(GameState);
        
        poly6 TestPoly = {};
        TestPoly.CenterP = V2(-18, -8); // In Meters
        TestPoly.OuterPoints[0] = V2(0, 2); // Relative to the Center.
        TestPoly.OuterPoints[1] = V2(1, 3);
        TestPoly.OuterPoints[2] = V2(2, 0); // front
        TestPoly.OuterPoints[3] = V2(0, -2);
        TestPoly.OuterPoints[4] = V2(-2, 1);
        TestPoly.OuterPoints[5] = V2(-1, 1); // Then 5 connects to 0
        
        GameState->TestPoly = TestPoly;
        
        
        Memory->IsInitialized = true;
    }
    
    if (GameState->RenderWidth != Render->Width ||
        GameState->RenderHeight != Render->Height)
    {
        GameState->RenderWidth = Render->Width;
        GameState->RenderHeight = Render->Height;
        GameState->RenderHalfWidth = (real32)GameState->RenderWidth * .5f;
        GameState->RenderHalfHeight = (real32)GameState->RenderHeight * .5f;
        
        InitMap(GameState);
        GameState->Player.CenterP = V2(0,0); //V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
        GameState->Player.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                                      RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GameState->Player.CenterP;
    }
    
    game_controller_input *Controller = GetController(Input, 0);
    v2 ddP = {};
    // TODO(Eric): Player Movement does NOT feel good, and is based on framerate
    real32 PlayerAngleMoveAmount = 5;
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
    
    // Update TestPoly's position
    GameState->TestPoly.CenterP.x += Input->dtForFrame;
    
    //
    // NOTE(Eric): Render
    //
    ClearBackground(Render);
    RenderMap(GameState, Render, &GameState->Map);
    
    
    
    v2 TestNewCoord = V2(-20,40);
    v2 ScreenNewCoord = GamePointToScreenPoint(GameState, TestNewCoord);
    RenderSquare(Render, ScreenNewCoord, 12, V3(0,0.5f,1));
    
    v2 TestMetersCoord = V2(-4,4); // Should be the same Y, but to the left a bit more
    v2 ScreenNewCoord2 = GamePointToScreenPoint(GameState, MetersToGamePoint(GameState, TestMetersCoord));
    RenderSquare(Render, ScreenNewCoord2, 12, V3(0,0.5f,1));
    
    v2 Origin = GamePointToScreenPoint(GameState, V2(0,0));
    RenderSquare(Render, Origin, 12, V3(1,0.5f,1));
    
    // Render TestPoly
    poly6 TestPoly = GameState->TestPoly;
    for (u32 PolyPIndex = 0; PolyPIndex < ArrayCount(TestPoly.OuterPoints); PolyPIndex++)
    {
        if (PolyPIndex != 5)
        {
            v2 P1 = GamePointToScreenPoint(GameState, MetersToGamePoint(GameState, TestPoly.CenterP + TestPoly.OuterPoints[PolyPIndex]));
            v2 P2 = GamePointToScreenPoint(GameState, MetersToGamePoint(GameState, TestPoly.CenterP + TestPoly.OuterPoints[PolyPIndex+1]));
            RenderLine(Render, P1, P2, 2, V3(.7,.5,.2));
        }
        else
        {
            v2 P1 = GamePointToScreenPoint(GameState, MetersToGamePoint(GameState, TestPoly.CenterP + TestPoly.OuterPoints[PolyPIndex]));
            v2 P2 = GamePointToScreenPoint(GameState, MetersToGamePoint(GameState, TestPoly.CenterP + TestPoly.OuterPoints[0]));
            RenderLine(Render, P1, P2, 2, V3(.7,.5,.2));
        }
    }
    
    
    
    // "Ray" from the player's front
    CastAndRenderPlayerLine(GameState, Render, 1, V3(.3, .8, .8));
    
    
    
    
}


