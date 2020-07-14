#ifndef ERIC_HANDMADE_H
#define ERIC_HANDMADE_H

#include "platform.h"
#include <math.h>
#include "intrinsics.h"
#include "eric_math.h"
#include "tile.h"
#include "collision.h"


#define PLAYER_HALFWIDTH 10
#define PLAYER_WIDTH 20
#define PLAYER_LENGTH_TO_CENTER 25

struct player
{
    v2 CenterP; // Center of the Triangle
    v2 FrontP;  // Point of the front of the ship (where bullets will come out)

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
    v2 StartP;  // Start Position of movement
    v2 EndP;    // End Position of movement

    real32 Radius;
    real32 Speed;

    asteroid_state State;

    v3 Color;
};

struct poly
{
    v2 CenterP;
    u32 PointCount;
    v2 Points[16];
};

struct bounding_box
{
	v2 Min;
	v2 Max;
};

struct game_state
{
    s32 RenderWidth;
    s32 RenderHeight;
    real32 RenderHalfWidth;
    real32 RenderHalfHeight;
    v2 ScreenCenter;

    player Player;

    poly TestPoly;
    //bounding_box TestCollisionBox;

    u32 AsteroidCount; // Current Count, not total
    asteroid Asteroids[256];

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

#endif //ERIC_HANDMADE_H