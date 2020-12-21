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
    
    
    // NOTE(Eric): Moved from RenderPlayer - Calculating the Back points
    int OppositeAngle = (int)(Result.FacingDirectionAngle + 180) % 360;
    int OppositeAngleTop = (int)(OppositeAngle + 40) % 360;
    int OppositeAngleBottom = (int)(OppositeAngle - 40) % 360;
    
    Result.BackLeftP = (V2(RoundReal32(PLAYER_HALFWIDTH * Cos(OppositeAngleTop * Pi32/180)),
                           RoundReal32(PLAYER_HALFWIDTH * Sin(OppositeAngleTop * Pi32/180)))
                        + Result.CenterP);
    
    Result.BackRightP = (V2(RoundReal32(PLAYER_HALFWIDTH * Cos(OppositeAngleBottom * Pi32/180)),
                            RoundReal32(PLAYER_HALFWIDTH * Sin(OppositeAngleBottom * Pi32/180)))
                         + Result.CenterP);
    
    GameState->Player = Result;
}

internal asteroid
InitAsteroidA(v2 StartP, v2 EndP)
{
    asteroid A = {};
    A.CenterP = StartP;
    A.Mass = 1.0f;
    
    v2 Direction = Normalize(EndP - StartP);
    A.Speed = 2.0f;
    A.Velocity = Direction * A.Speed;
    
    A.Acceleration = V2(0.0f, 0.0f); // NOTE(Eric): Should be calculated later from dt and Velocity.
    
    A.Points[0] = V2(0, 2); // Relative to the Center.
    A.Points[1] = V2(1, 3);
    A.Points[2] = V2(2, 0); // front
    A.Points[3] = V2(0, -2);
    A.Points[4] = V2(-2, 1);
    A.Points[5] = V2(-1, 1); // Then 5 connects to 0
    A.PointCount = 6;
    
    A.State = AsteroidState_Active;
    A.Color = V3(1,1,1);
    
    return(A);
}

internal asteroid
InitAsteroidB(v2 StartP, v2 EndP)
{
    asteroid A = {};
    A.CenterP = StartP;
    A.Mass = 1.0f;
    
    v2 Direction = Normalize(EndP - StartP);
    A.Speed = 2.0f;
    A.Velocity = Direction * A.Speed;
    
    A.Acceleration = V2(0.0f, 0.0f); // NOTE(Eric): Should be calculated later from dt and Velocity.
    
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
    // TODO(Eric): Come up with a scheme to phase asteroids in and out of play.
    // We have the active/inactive, but we need to be constantly updating this array
    // and reusing slots.
    
    // TODO(Eric): Randomize all positions! (and size, and speed)
    random_series Series = RandomSeed(321);
    
    v2 CenterMap = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    v2 RenderMax = V2((real32)GameState->RenderWidth, (real32)GameState->RenderHeight);
    
    v2 P1 = V2(10, 0);
    v2 P2 = V2(40, 20);
    GameState->Asteroids[0] = InitAsteroidA(P1, P2);
    GameState->AsteroidCount++;
    
    v2 P3 = V2(30, 0);
    v2 P4 = V2(5, 10);
    GameState->Asteroids[1] = InitAsteroidB(P3, P4);
    GameState->AsteroidCount++;
    
    v2 P5 = V2(20, 20);
    v2 P6 = V2(20, -20);
    GameState->Asteroids[2] = InitAsteroidA(P5, P6);
    GameState->AsteroidCount++;
    
    v2 P7 = V2(-10, 20);
    v2 P8 = V2(10, 20);
    v2 P9 = V2(12, 25);
    GameState->Asteroids[3] = InitAsteroidA(P7, P9);
    GameState->AsteroidCount++;
    
    GameState->Asteroids[4] = InitAsteroidA(P8, P7);
    GameState->AsteroidCount++;
}

internal v2
ApplyForces(v2 Velocity, real32 Mass, real32 Drag)
{
    v2 Result = {};
    
    // NOTE(Eric): Example gravity along the Y-Axis
    // I'm picturing a future feature where there are 'black-holes' that pull in asteroids, etc.
    //v2 GravityAcc = V2(0.0f, -9.81f);
    
    v2 GravityAcc = V2(0.0f, 0.0f);
    v2 DragForce = 0.5f * Drag * (Velocity * LengthSq(Velocity));
    v2 DragAcc = DragForce / Mass;
    
    Result = GravityAcc - DragAcc;
    
    return (Result);
}

internal void
MoveAsteroid(asteroid *Asteroid, real32 dt)
{
    
    
    //if (WithinGameBounds)
    {
        // NOTE(Eric): Vertlet Integration
        {
            Asteroid->CenterP = Asteroid->CenterP + Asteroid->Velocity * dt + Asteroid->Acceleration * (dt * dt * 0.5f);
            
            // NOTE(Eric): Optional, for gravity, etc.
            //Asteroid->Acceleration = ApplyForces(Asteroid->Velocity, Asteroid->Mass, 0.1f);
            
            Asteroid->Velocity = Asteroid->Velocity + (Asteroid->Acceleration + Asteroid->Acceleration) * (dt * 0.5f);
        }
        
        // NOTE(Eric): Semi-implicit Euler
        {
            //Asteroid->Velocity = Asteroid->Velocity + (Asteroid->Force / Asteroid->Mass) * dt;
            //Asteroid->CenterP = Asteroid->CenterP + (Asteroid->Velocity * dt);
        }
    }
#if 0
    else
    {
        Asteroid->State = AsteroidState_Inactive;
    }
#endif
}

internal void
MoveProjectile(game_state *GameState, projectile *Projectile, real32 dt)
{
    // NOTE(Eric): This is really 'UpdateProjectile', not _just_ move
    
    real32 Distance = 1000;
    v2 TargetP = V2(Distance * Cos(Projectile->ShootDirectionAngle * Pi32/180), 
                    Distance * Sin(Projectile->ShootDirectionAngle * Pi32/180));
    real32 DistanceRemaining = Length(TargetP);
    
    // NOTE(Eric): If it's still on the screen?
    if (DistanceRemaining > 1)
    {
        v2 ddP = {};
        ddP.x = (TargetP.x)/(dt*600000);
        ddP.y = (TargetP.y)/(dt*600000);
        
        real32 MoveX = ddP.x * (dt * 100);
        real32 MoveY = ddP.y * (dt * 100);
        
        Projectile->P1.x += (real32)(MoveX);
        Projectile->P1.y += (real32)(MoveY);
        Projectile->P2.x += (real32)(MoveX);
        Projectile->P2.y += (real32)(MoveY);
    }
    else
    {
        Projectile->IsActive = false;
        
        // TODO(Eric): Remove projectile from GameState->Projectiles and decrement Count
        // Or move it to another routine that cleans up the entire array?
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
        InitializeArena(&GameState->TransientArena, Memory->TransientStorageSize, 
                        (u8 *)Memory->TransientStorage);
        
        GameState->RenderWidth = Render->Width;
        GameState->RenderHeight = Render->Height;
        GameState->RenderHalfWidth = (real32)GameState->RenderWidth * 0.5f;
        GameState->RenderHalfHeight = (real32)GameState->RenderHeight * 0.5f;
        ScreenCenter = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
        
        InitMap(GameState);
        InitPlayer(GameState);
        
        GameState->AsteroidCount = 0;
        InitAsteroids(GameState);
        
        GameState->ProjectileCount = 0;
        
        GameState->DebugBoxCount = 0;
        GameState->DebugLineCount = 0;
        
        
        // NOTE(Eric): Example Transient memory - Array with just a pointer
        // Not using "SampleAsteroids" anywhere, I just wanted to get an example down.
        u32 SampleAsteroidCount = 256;
        asteroid *SampleAsteroids = PushArray(&GameState->TransientArena, SampleAsteroidCount, asteroid);
        for (int SampleAsteroidIndex = 0;
             SampleAsteroidIndex < ArrayCount(SampleAsteroids);
             SampleAsteroidIndex++)
        {
            asteroid *SampleAsteroid = SampleAsteroids + SampleAsteroidIndex;
            //InitializeAsteroid(SampleAsteroid, ...)
        }
        
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
    if(Controller->MoveUp.IsDown)
    {
        GameState->Player.FacingDirectionAngle -= PlayerAngleMoveAmount;
        ddP.y = 1.0f;
    }
    if(Controller->MoveDown.IsDown)
    {
        GameState->Player.FacingDirectionAngle += PlayerAngleMoveAmount;
        ddP.y = -1.0f;
    }
    if(Controller->MoveLeft.IsDown)
    {
        GameState->Player.FacingDirectionAngle += PlayerAngleMoveAmount;
        ddP.x = -1.0f;
    }
    if(Controller->MoveRight.IsDown)
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
    
    // NOTE(Eric): Projectiles
    if (Controller->ActionUp.EndedDown)
    {
        // TODO(Eric): Make a timer/delay on when a new projectile can be shot
        real32 ProjectileSize = 1;
        projectile NewProjectile = {};
        NewProjectile.IsActive = true;
        NewProjectile.ShootDirectionAngle = GameState->Player.FacingDirectionAngle;
        
        // TODO(Eric): Calculate tip of player for start position
        NewProjectile.P1 = GameState->Player.FrontP / METERS_TO_PIXELS;
        NewProjectile.P2 = V2(ProjectileSize * Cos(NewProjectile.ShootDirectionAngle * Pi32/180), 
                              ProjectileSize * Sin(NewProjectile.ShootDirectionAngle * Pi32/180))
            + NewProjectile.P1;
        
        GameState->Projectiles[GameState->ProjectileCount++] = NewProjectile;
    }
    
    
    // Update players position
    GameState->Player.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                                  RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GameState->Player.CenterP;
    // NOTE(Eric): Update players Back points. Move this to a function?
    int OppositeAngle = (int)(GameState->Player.FacingDirectionAngle + 180) % 360;
    int OppositeAngleTop = (int)(OppositeAngle + 40) % 360;
    int OppositeAngleBottom = (int)(OppositeAngle - 40) % 360;
    
    GameState->Player.BackLeftP = (V2(RoundReal32(PLAYER_HALFWIDTH * Cos(OppositeAngleTop * Pi32/180)),
                                      RoundReal32(PLAYER_HALFWIDTH * Sin(OppositeAngleTop * Pi32/180)))
                                   + GameState->Player.CenterP);
    
    GameState->Player.BackRightP = (V2(RoundReal32(PLAYER_HALFWIDTH * Cos(OppositeAngleBottom * Pi32/180)),
                                       RoundReal32(PLAYER_HALFWIDTH * Sin(OppositeAngleBottom * Pi32/180)))
                                    + GameState->Player.CenterP);
    
    
    
    // NOTE(Eric): I'm not sure where to do this.
    HandleAsteroidColissions(GameState, Input->dtForFrame);
    
    // Update Asteroids
    for (u32 AsteroidIndex = 0;
         AsteroidIndex < GameState->AsteroidCount;
         AsteroidIndex++)
    {
        asteroid *Ast = GameState->Asteroids + AsteroidIndex;
        
        MoveAsteroid(Ast, Input->dtForFrame);
    }
    
    // Update Projectiles
    for (u32 ProjectileIndex = 0;
         ProjectileIndex < GameState->ProjectileCount;
         ProjectileIndex++)
    {
        projectile *Projectile = GameState->Projectiles + ProjectileIndex;
        
        MoveProjectile(GameState, Projectile, Input->dtForFrame);
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
        RenderAsteroidPositions(Render, Ast);
    }
    
    // Render Projectiles
    for (u32 ProjectileIndex = 0;
         ProjectileIndex < GameState->ProjectileCount;
         ProjectileIndex++)
    {
        projectile *Projectile = GameState->Projectiles + ProjectileIndex;
        RenderProjectile(Render, &GameState->Map, Projectile);
    }
    
    // Render DEBUG things
    for (u32 DebugBoxIndex = 0;
         DebugBoxIndex < GameState->DebugBoxCount;
         DebugBoxIndex++)
    {
        bounding_box *Box = GameState->DebugBoxes + DebugBoxIndex;
        RenderWireBoundingBox(Render, Box);
    }
    for (u32 DebugLineIndex = 0;
         DebugLineIndex < GameState->DebugLineCount;
         DebugLineIndex++)
    {
        line *Line = GameState->DebugLines + DebugLineIndex;
        RenderDebugLine(Render, Line);
    }

    Assert(GameState->Map.TileCountX < 50);
    
    // "Ray" from the player's front
    //CastAndRenderPlayerLine(GameState, Render, 1, V3(.3, .8, .8));
    
}


