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

global_variable v2 ScreenCenter;

struct line
{
    v2 P1;
    v2 P2;
    v3 Color;
};

struct player
{
    v2 CenterP; // Center of the Triangle
    v2 FrontP;  // Point of the front of the ship (where bullets will come out)
    
    // NOTE(Eric): Added because we need them for collision. Consider creating line struct.
    v2 BackLeftP;
    v2 BackRightP;
    
    real32 FacingDirectionAngle;
};

enum asteroid_state
{
    AsteroidState_Inactive = 0x0,
    AsteroidState_Active = 0x1,
};

struct asteroid
{
    v2 CenterP; // 'Center' of the asteroid
    
    // NOTE(Eric): Force = Mass * Acceleration   |    Acceleration = Force / Mass
    // NOTE(Eric): Change in Acceleration = Velocity * dt
    // NOTE(Eric): Change in Velocity = Acceleration * dt
    
    v2 Velocity;
    v2 Acceleration;
    real32 Mass;
    
    real32 Speed; // NOTE(Eric): Velocity = Direction * Speed
    
    b32 IsColliding;
    
    u32 PointCount;
    v2 Points[16];
    
    asteroid_state State;
    
    v3 Color;
};

// NOTE(Eric): Trying out what players shooting might be like, no idea what I'm doing yet.
struct projectile
{
    // TODO(Eric): I think these should store their own IDs, so we can actually manage the arrays
    real32 ShootDirectionAngle;
    
    v2 P1;
    v2 P2;
    
    b32 IsActive;
};

struct bounding_box
{
	v2 Min;
	v2 Max;
};

struct game_state
{
    memory_arena TransientArena;
    
    s32 RenderWidth;
    s32 RenderHeight;
    real32 RenderHalfWidth;
    real32 RenderHalfHeight;
    
    player Player;
    
    u32 AsteroidCount; // Current Count, not total
    asteroid Asteroids[256];
    
    u32 ProjectileCount; // Current Count, not total
    projectile Projectiles[256];
    
    u32 DebugBoxCount;
    bounding_box DebugBoxes[256];
    
    u32 DebugLineCount;
    line DebugLines[256];
    
    // NOTE(Eric): Must be last
    screen_map Map;
};




//~ NOTE(Eric): Functions
inline int
Clamp(int Min, int Value, int Max)
{
    if (Value < Min) return Min;
    if (Value > Max) return Max;
    return Value;
}

inline real32
Clamp(real32 Min, real32 Value, real32 Max)
{
    if (Value < Min) return Min;
    if (Value > Max) return Max;
    return Value;
}

inline u32
GetColor(v3 Values)
{
    u32 Result = ((RoundReal32ToUInt32(Values.r * 255.0f) << 16) |
                  (RoundReal32ToUInt32(Values.g * 255.0f) << 8) |
                  (RoundReal32ToUInt32(Values.b * 255.0f) << 0));
    return(Result);
}

// NOTE(Eric): Linear Interpolation or lerp
inline real32
Mix(real32 A, real32 B, real32 Amount)
{
    real32 Result = (1-Amount) * A + Amount * B;
    
    return(Result);
}
inline real32
Lerp(real32 A, real32 B, real32 Amount)
{
    real32 Result = (1-Amount) * A + Amount * B;
    
    return(Result);
}
inline v2
Mix(v2 A, v2 B, real32 Amount)
{
    v2 Result = {};
    Result.x = (1-Amount) * A.x + Amount * B.x;
    Result.y = (1-Amount) * A.y + Amount * B.y;
    
    return(Result);
}
inline v2
Lerp(v2 A, v2 B, real32 Amount)
{
    v2 Result = {};
    Result.x = (1-Amount) * A.x + Amount * B.x;
    Result.y = (1-Amount) * A.y + Amount * B.y;
    
    return(Result);
}

//~ NOTE(Eric): Debug Rendering
inline void
AddDebugRenderBox(game_state *GameState, bounding_box Box)
{
    //Assert(GameState->DebugBoxCount < ArrayCount(GameState->DebugBoxes));
    if (GameState->DebugBoxCount < 255)
        GameState->DebugBoxes[++GameState->DebugBoxCount] = Box;
}

inline void
AddDebugRenderLine(game_state *GameState, line Line)
{
    //Assert(GameState->DebugBoxCount < ArrayCount(GameState->DebugBoxes));
    if (GameState->DebugLineCount < 255)
        GameState->DebugLines[++GameState->DebugLineCount] = Line;
}



#endif //ERIC_HANDMADE_H