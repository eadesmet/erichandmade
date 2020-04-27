
#include "erichandmade.h"
#include "random.h"
#include "tile.cpp"
#include "render.cpp"

//~ NOTE(Eric): Init functions
internal void
InitPlayer(game_state *GameState)
{
    player Result = {};
    Result.CenterP = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    Result.FacingDirectionAngle = 180;
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
    Asteroid.State = ASTEROIDSTATE_ACTIVE;
    Asteroid.Speed = 30;
    Asteroid.Color = V3(1,1,1);
    GameState->Asteroids[0] = Asteroid;
    GameState->AsteroidCount++;
    
    Asteroid = {};
    Asteroid.CenterP = P2;
    Asteroid.StartP = Asteroid.CenterP;
    Asteroid.EndP = P2;
    Asteroid.Radius = 30;
    Asteroid.State = ASTEROIDSTATE_ACTIVE;
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
        Asteroid->State = ASTEROIDSTATE_INACTIVE;
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
        
        Memory->IsInitialized = true;
    }
    
    if (GameState->RenderWidth != Render->Width ||
        GameState->RenderHeight != Render->Height)
    {
        GameState->RenderWidth = Render->Width;
        GameState->RenderHeight = Render->Height;
        GameState->RenderHalfWidth = (real32)GameState->RenderWidth / 2.0f;
        GameState->RenderHalfHeight = (real32)GameState->RenderHeight / 2.0f;
        
        InitMap(GameState);
        GameState->Player.CenterP = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
        GameState->Player.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                                      RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GameState->Player.CenterP;
    }
    
    game_controller_input *Controller = GetController(Input, 0);
    v2 ddP = {};
    // TODO(Eric): Player Movement does NOT feel good, and is based on framerate
    if(Controller->MoveUp.EndedDown)
    {
        GameState->Player.FacingDirectionAngle -= 10;
        ddP.y = 1.0f;
    }
    if(Controller->MoveDown.EndedDown)
    {
        GameState->Player.FacingDirectionAngle += 10;
        ddP.y = -1.0f;
    }
    if(Controller->MoveLeft.EndedDown)
    {
        GameState->Player.FacingDirectionAngle += 10;
        ddP.x = -1.0f;
    }
    if(Controller->MoveRight.EndedDown)
    {
        GameState->Player.FacingDirectionAngle -= 10;
        ddP.x = 1.0f;
    }
    
    // Update players position
    GameState->Player.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                                  RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GameState->Player.CenterP;
    
    //
    // NOTE(Eric): Render
    //
    ClearBackground(Render);
    RenderMap(Render, &GameState->Map);
    
    
    // Crosshairs
    v3 CrosshairColor = V3(0.5,0.5,0.5);
    RenderLine(Render,
               V2(0, GameState->RenderHalfHeight),
               V2((real32)GameState->RenderWidth, GameState->RenderHalfHeight), 1, CrosshairColor);
    
    RenderLine(Render,
               V2(GameState->RenderHalfWidth, 0),
               V2(GameState->RenderHalfWidth, (real32)GameState->RenderHeight), 1, CrosshairColor);
    
    RenderPlayer(Render, &GameState->Player, &GameState->Map);
}


