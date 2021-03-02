#include "erichandmade.h"
#include "random.h"
#include "tile.cpp"
#include "render.cpp"
#include "collision.cpp"
#include "walkline.cpp"

#include "hash.h"

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

internal entity
InitAsteroidA(v2 StartP, v2 EndP)
{
    entity A = {};
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

    A.State = EntityState_Active;
    A.Type = EntityType_Asteroid;
    A.Color = V3(1,1,1);

    return(A);
}

internal entity
InitAsteroidB(v2 StartP, v2 EndP)
{
    entity A = {};
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

    A.State = EntityState_Active;
    A.Type = EntityType_Asteroid;
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
InitAsteroids(game_state *GameState, u32 Seed)
{
    // TODO(Eric): Come up with a scheme to phase asteroids in and out of play.
    // We have the active/inactive, but we need to be constantly updating this array
    // and reusing slots.

    random_series Series = RandomSeed(Seed*4);
/*
    v2 CenterMap = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    v2 RenderMax = V2((real32)GameState->RenderWidth, (real32)GameState->RenderHeight);
    s32 MinAsteroidCount = 30;
    s32 MaxAsteroidCount = 60;
    s32 MinPosition = -48;
    s32 MaxPosition = 48;
*/
    v2 MaxAsteroidSize = V2(9.0f, 9.0f); // NOTE(Eric): Shared with 'HandleAsteroidCollisions'. Make it global?

    u32 SlotCountX = RoundReal32ToUInt32(PixelsToMeters((real32)GameState->RenderWidth) / MaxAsteroidSize.x);
    u32 SlotCountY = RoundReal32ToUInt32(PixelsToMeters((real32)GameState->RenderHeight) / MaxAsteroidSize.y);
    u32 TotalSlotsCount = SlotCountX * SlotCountY;

    ast_slot *AstSlots = PushArray(&GameState->TransientArena, TotalSlotsCount, ast_slot);
    //ast_slot AstSlots[2048];
/*
    for (u32 Y = 0; Y < SlotCountY; ++Y)
    {
        for (u32 X = 0; X < SlotCountX; ++X)
        {
            ast_slot *TheSlot = AstSlots + Y*SlotCountX+X;
            TheSlot->Index = V2((real32)X,(real32)Y);
            TheSlot->Taken = false;
        }
    }
*/
    u32 NumOfAsteroids = 30;
    for (u8 AstIndex = 1;           // NOTE(Eric): 1-30
         AstIndex <= NumOfAsteroids;
         ++AstIndex)
    {
        b32 AsteroidPlaced = false;
        do
        {
            s32 RandI = RandomBetween(&Series, 0, SlotCountX);
            s32 RandJ = RandomBetween(&Series, 0, SlotCountY);
            ast_slot *Slot = AstSlots + RandJ*SlotCountX+RandI;
            if (!Slot->Taken)
            {
                Slot->Index = V2((real32)RandI, (real32)RandJ);
                Slot->Taken = true;
                AsteroidPlaced = true;
            }
        } while(!AsteroidPlaced); // NOTE(Eric): The number of times this runs is unknown!
    }

    // NOTE(Eric): This Romps anything in Entities beforehand, so Asteroids are always the first slots
    u32 EntityIndex = 0;
    for (u32 i = 0;
         i < SlotCountY;
         ++i)
    {
        for (u32 j = 0;
             j < SlotCountX;
             ++j)
        {
            ast_slot *Slot = AstSlots + i*SlotCountX+j;
            if (!Slot || !Slot->Taken) continue;

            // Convert the index (0 - SlotCount) into (-1/2 Slot Count - 1/2 Slot Count)
            real32 ConvertedI = j - (SlotCountX*.5f);
            real32 ConvertedJ = i - (SlotCountY*.5f);

            ConvertedI *= MaxAsteroidSize.y;
            ConvertedJ *= MaxAsteroidSize.x;

#if 0
            bounding_box Box = {};
            Box.Min = GamePointToScreenPoint(V2(ConvertedI, ConvertedJ));
            Box.Max = GamePointToScreenPoint(V2(ConvertedI+MaxAsteroidSize.x, ConvertedJ+MaxAsteroidSize.y));
            AddDebugRenderBox(GameState, Box);



            bounding_box CenterBox = {};
            CenterBox.Min = GamePointToScreenPoint(V2(ConvertedI+(MaxAsteroidSize.x*.5f),
                                                      ConvertedJ+(MaxAsteroidSize.y*.5f)));
            CenterBox.Max = GamePointToScreenPoint(V2(ConvertedI+(MaxAsteroidSize.x*.6f),
                                                      ConvertedJ+(MaxAsteroidSize.y*.6f)));
            CenterBox.Fill = true;
            AddDebugRenderBox(GameState, CenterBox);
#endif

            v2 P1 = V2(ConvertedI+(MaxAsteroidSize.x*.5f), ConvertedJ+(MaxAsteroidSize.y*.5f));

            real32 RandomX = RandomBetween(&Series, -MaxAsteroidSize.x*3, MaxAsteroidSize.x*3);
            real32 RandomY = RandomBetween(&Series, -MaxAsteroidSize.y*3, MaxAsteroidSize.y*3);
            v2 P2 = V2(RandomX, RandomY);

            if (EntityIndex % 2 == 0)
            {
                GameState->Entities[EntityIndex++] = InitAsteroidA(P1, P2);
            }
            else
            {
                GameState->Entities[EntityIndex++] = InitAsteroidB(P1, P2);
            }
            GameState->EntityCount++;
        }
    }



#if 0
    s32 Rand_NumOfAsteroids = RandomBetween(&Series, MinAsteroidCount, MaxAsteroidCount);
    for (s32 Index = 0;
         Index < Rand_NumOfAsteroids;
         ++Index)
    {
        v2 P1 = V2((real32)RandomBetween(&Series, MinPosition, MaxPosition),
                   (real32)RandomBetween(&Series, MinPosition, MaxPosition));

        v2 P2 = V2((real32)RandomBetween(&Series, MinPosition, MaxPosition),
                   (real32)RandomBetween(&Series, MinPosition, MaxPosition));

        for (u32 AsteroidIndex = 0;
             AsteroidIndex < GameState->EntityCount;
             AsteroidIndex++)
        {
            entity *Ast = GameState->Entities + AsteroidIndex;
            if (Ast->Type != EntityType_Asteroid) continue;

            // NOTE(Eric): Check to make sure they don't spawn inside eachother
            if((Ast->CenterP - P1) < MaxAsteroidSize)
            {
                // TODO(Eric): This still has a chance to spawn inside another!
                P1 += MaxAsteroidSize;
            }
        }

        if (Index % 2 == 0)
        {
            GameState->Entities[Index] = InitAsteroidA(P1, P2);
        }
        else
        {
            GameState->Entities[Index] = InitAsteroidB(P1, P2);
        }
        GameState->EntityCount++;

    }
#endif


#if 0
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

    v2 P22 = V2(-30, 0);
    v2 P33 = V2(30, 0);
    GameState->Asteroids[5] = InitAsteroidA(P22, P33);
    GameState->AsteroidCount++;
#else
    //v2 P22 = V2(-30, 0);
    //v2 P33 = V2(10, 0);
    //GameState->Asteroids[0] = InitAsteroidA(P22, P33);
    //GameState->AsteroidCount++;
#endif
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
MoveAsteroid(entity *Asteroid, real32 dt)
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
MoveProjectile(game_state *GameState, entity *Projectile, real32 dt)
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

        real32 MoveX = ddP.x * (dt * 500);
        real32 MoveY = ddP.y * (dt * 500);

        Projectile->Points[0].x += (real32)(MoveX);
        Projectile->Points[0].y += (real32)(MoveY);
        Projectile->Points[1].x += (real32)(MoveX);
        Projectile->Points[1].y += (real32)(MoveY);
    }
    else
    {
        Projectile->State = EntityState_InActive;

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

        GameState->Player.ShotTimer = 100; // Set to a high value so the player can shoot right away

        GameState->DebugBoxCount = 0;
        GameState->DebugLineCount = 0;

        InitAsteroids(GameState, RoundReal32ToInt32(5525*Input->dtForFrame));


        // NOTE(Eric): Example Transient memory - Array with just a pointer
        // Not using "SampleAsteroids" anywhere, I just wanted to get an example down.
        /*
        u32 SampleAsteroidCount = 256;
        entity *SampleAsteroids = PushArray(&GameState->TransientArena, SampleAsteroidCount, asteroid);
        for (int SampleAsteroidIndex = 0;
             SampleAsteroidIndex < ArrayCount(SampleAsteroids);
             SampleAsteroidIndex++)
        {
            asteroid *SampleAsteroid = SampleAsteroids + SampleAsteroidIndex;
            //InitializeAsteroid(SampleAsteroid, ...)
        }
        */
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
    if(Controller->ActionDown.IsDown)
    {
        IsPaused = !IsPaused;
    }

    if (IsPaused)
    {

    }
    else
    {
        // TODO(Eric): Player Movement does NOT feel good, and is based on framerate
        real32 PlayerAngleMoveAmount = 4;
        if(Controller->MoveUp.IsDown)
        {
            GameState->Player.FacingDirectionAngle -= PlayerAngleMoveAmount;
        }
        if(Controller->MoveDown.IsDown)
        {
            GameState->Player.FacingDirectionAngle += PlayerAngleMoveAmount;
        }
        if(Controller->MoveLeft.IsDown)
        {
            GameState->Player.FacingDirectionAngle += PlayerAngleMoveAmount;
        }
        if(Controller->MoveRight.IsDown)
        {
            GameState->Player.FacingDirectionAngle -= PlayerAngleMoveAmount;
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
        GameState->Player.ShotTimer += Input->dtForFrame;
        if (Controller->ActionUp.EndedDown &&
            GameState->Player.ShotTimer >= SHOT_RATE)
        {
            GameState->Player.ShotTimer = 0;

            real32 ProjectileSize = 1;
            entity NewProjectile = {};
            NewProjectile.Type = EntityType_Projectile;
            NewProjectile.State = EntityState_Active;
            NewProjectile.ShootDirectionAngle = GameState->Player.FacingDirectionAngle;

            NewProjectile.PointCount = 2;
            NewProjectile.Points[0] = GameState->Player.FrontP / METERS_TO_PIXELS;
            NewProjectile.Points[1] = V2(ProjectileSize * Cos(NewProjectile.ShootDirectionAngle * Pi32/180),
                                         ProjectileSize * Sin(NewProjectile.ShootDirectionAngle * Pi32/180))
                + NewProjectile.Points[0];

            GameState->Entities[GameState->EntityCount++] = NewProjectile;
        }

        //
        // NOTE(Eric): Update
        //

        // Update players position
        int OppositeAngle = (int)(GameState->Player.FacingDirectionAngle + 180) % 360;
        int OppositeAngleTop = (int)(OppositeAngle + 40) % 360;
        int OppositeAngleBottom = (int)(OppositeAngle - 40) % 360;

        GameState->Player.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                                      RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GameState->Player.CenterP;
        GameState->Player.BackLeftP = (V2(RoundReal32(PLAYER_HALFWIDTH * Cos(OppositeAngleTop * Pi32/180)),
                                          RoundReal32(PLAYER_HALFWIDTH * Sin(OppositeAngleTop * Pi32/180)))
                                       + GameState->Player.CenterP);
        GameState->Player.BackRightP = (V2(RoundReal32(PLAYER_HALFWIDTH * Cos(OppositeAngleBottom * Pi32/180)),
                                           RoundReal32(PLAYER_HALFWIDTH * Sin(OppositeAngleBottom * Pi32/180)))
                                        + GameState->Player.CenterP);


        for (u32 EntityIndex = 0;
             EntityIndex < GameState->EntityCount;
             EntityIndex++)
        {
            entity *Entity = GameState->Entities + EntityIndex;
            if (Entity->State == EntityState_InActive) continue;

            switch (Entity->Type)
            {
                case EntityType_Asteroid:
                {
                    MoveAsteroid(Entity, Input->dtForFrame);

                    // Handle Asteroid-Asteroid collisions, and Asteroid-Player collisions
                    HandleAsteroidColissions(GameState, Entity, Input->dtForFrame);
                    break;
                }
                case EntityType_Projectile:
                {
                    MoveProjectile(GameState, Entity, Input->dtForFrame);

                    for (u32 AsteroidIndex = 0;
                         AsteroidIndex < GameState->EntityCount;
                         AsteroidIndex++)
                    {
                        entity *Ast = GameState->Entities + AsteroidIndex;
                        if (Ast->Type != EntityType_Asteroid) continue;
                        if (Ast->State == EntityState_InActive) continue;
                        line_collision_result HitAsteroidResult = CheckCollision_LineAsteroid(GamePointToScreenPoint(Entity->Points[0]),
                                                                                              GamePointToScreenPoint(Entity->Points[1]), *Ast);
                        if (HitAsteroidResult.IsColliding)
                        {
                            // TODO(Eric): Phase out this asteroid and queue up generation of more
                            Ast->State = EntityState_InActive;

                            // NOTE(Eric): Destroying the projectile on impact
                            Entity->State = EntityState_InActive;

                            GameState->Player.Score++;
                        }
                    }
                    break;
                }
            }
        }
    }

    //
    // NOTE(Eric): Render
    //
    ClearBackground(Render);
    //RenderMap(GameState, Render, &GameState->Map);
    RenderUI(GameState, Render, Input->dtForFrame);
    RenderPlayer(GameState, Render, &GameState->Player, &GameState->Map);


    for (u32 EntityIndex = 0;
         EntityIndex < GameState->EntityCount;
         EntityIndex++)
    {
        entity *Entity = GameState->Entities + EntityIndex;
        if (Entity->State == EntityState_InActive) continue;

        switch (Entity->Type)
        {
            case EntityType_Asteroid:
            {
                RenderAsteroid(Render, &GameState->Map, Entity);
                RenderAsteroidPositions(Render, Entity);
                break;
            }
            case EntityType_Projectile:
            {
                RenderProjectile(Render, &GameState->Map, Entity);
                break;
            }
        }
    }

    // TODO(Eric): Idea! (2/24/2021): Once all of the asteroids are Inactive, call InitAsteroids again (with different seed)
    //  to start 'Level 2' !!
    // Then we can save level data and scores easy


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


