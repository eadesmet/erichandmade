#ifndef ERIC_HANDMADE_H
#define ERIC_HANDMADE_H

#include "platform.h"
#include "intrinsics.h"
#include "eric_math.h"
#include "tile.h"
#include "collision.h"


#define PLAYER_HALFWIDTH 10
#define PLAYER_WIDTH 20
#define PLAYER_LENGTH_TO_CENTER 25

#define SHOT_RATE 1

global_variable v2 ScreenCenter;
global_variable b32 IsPaused = true;

struct line
{
    v2 P1;
    v2 P2;
    v3 Color;
};

enum cell_flag
{
    Cell_None,
    Cell_Crossed,
    Cell_Overlapping
};

struct cell
{
    line Left;
    line Right;
    line Top;
    line Bottom;
    
    cell_flag Flag;
};

struct player
{
    v2 CenterP; // Center of the Triangle
    v2 FrontP;  // Point of the front of the ship (where bullets will come out)
    
    // NOTE(Eric): Added because we need them for collision. Consider creating line struct.
    v2 BackLeftP;
    v2 BackRightP;
    
    real32 FacingDirectionAngle;
    
    real32 ShotTimer;
    u32 Score;
};

enum entity_type
{
	EntityType_None,
    
	EntityType_Asteroid,
	EntityType_Projectile,
	EntityType_DebugLine,
    
	EntityType_Count
};

enum entity_state
{
	EntityState_InActive,
	EntityState_Active,
    
	EntityState_Count
};

struct entity
{
	v2 CenterP;
    
	// NOTE(Eric): Force = Mass * Acceleration   |    Acceleration = Force / Mass
    // NOTE(Eric): Change in Acceleration = Velocity * dt
    // NOTE(Eric): Change in Velocity = Acceleration * dt
	v2 Velocity;
	v2 Acceleration;
	f32 Mass;
	f32 Speed; // NOTE(Eric): Velocity = Direction * Speed
    
	b32 IsColliding;
    
	u32 PointCount;
	v2 Points[16];
    
	entity_type Type;
	entity_state State;
    
	// TODO(Eric): This should be Temporary! We can probably caluclate it from the points
	real32 ShootDirectionAngle;
    
    real32 RotationRate; // NOTE(Eric): Asteroids points (range roughly -0.1, +0.1)
    
	v3 Color;
};

struct bounding_box
{
	v2 Min;
	v2 Max;
    
    b32 IsGamePoints; // NOTE(Eric): Just for rendering purposes
    
	b32 Fill;
	v3 Color;
};

struct game_state
{
    memory_arena TransientArena;
    
    s32 RenderWidth;
    s32 RenderHeight;
    real32 RenderHalfWidth;
    real32 RenderHalfHeight;
    
    player Player;
    
    u32 EntityCount;
    entity Entities[256];
    
    u32 DebugBoxCount;
    bounding_box DebugBoxes[512];
    
    u32 DebugLineCount;
    line DebugLines[256];
    
    // TEMP
    real32 Time;
    
    bounding_box MousePos; // TODO(Eric): Could be a part of debugboxes, but it's kinda different
    
    // NOTE(Eric): Must be last
    screen_map Map;
};




//~ NOTE(Eric): Functions
inline bounding_box
GetBoxFromPolygon(entity E)
{
    // NOTE(Eric): Where would I put a function like this?!
    bounding_box Result = {};
    
    v2 Min = V2(50000.0f, 50000.0f);
    v2 Max = V2(-50000.0f, -50000.0f);
    
    for (u32 PointIndex = 0;
         PointIndex < E.PointCount;
         PointIndex++)
    {
        v2 P = *(E.Points + PointIndex);
        
        if (P.x <= Min.x)
            Min.x = P.x;
        if (P.y <= Min.y)
            Min.y = P.y;
        
        if (P.x >= Max.x)
            Max.x = P.x;
        if (P.y >= Max.y)
            Max.y = P.y;
    }
    
    Result.Min = Min + E.CenterP;
    Result.Max = Max + E.CenterP;
    
    Result.IsGamePoints = true;
    
    return(Result);
}

// TODO(Eric): Why are these here?
//~ NOTE(Eric): Debug Rendering
inline void
AddDebugRenderBox(game_state *GameState, bounding_box Box)
{
    //Assert(GameState->DebugBoxCount < ArrayCount(GameState->DebugBoxes));
    if (GameState->DebugBoxCount < ArrayCount(GameState->DebugBoxes)-1)
        GameState->DebugBoxes[GameState->DebugBoxCount++] = Box;
}

inline void
AddDebugRenderLine(game_state *GameState, line Line)
{
    //Assert(GameState->DebugBoxCount < ArrayCount(GameState->DebugBoxes));
    if (GameState->DebugLineCount < ArrayCount(GameState->DebugLines)-1)
        GameState->DebugLines[++GameState->DebugLineCount] = Line;
}



#endif //ERIC_HANDMADE_H