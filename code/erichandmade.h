#ifndef ERIC_HANDMADE_H
#define ERIC_HANDMADE_H

#include "platform.h"
#include <math.h>
#include "eric_math.h"
#include "intrinsics.h"
#include "tile.h"


global_variable v2 MinV2 = v2{0,0};
//global_variable v2 MaxV2 = v2{0,0};


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
    ASTEROIDSTATE_INACTIVE = 0x0,
    ASTEROIDSTATE_ACTIVE = 0x1,
};

// Asteroid Sized (Radius)
#define ASTEROID_SMALL_R 20
#define ASTEROID_MEDIUM_R 40
#define ASTEROID_LARGE_R 80

// Asteroid Speed
#define ASTEROIDSPEED_SLOW 3;

struct asteroid
{
    v2 CenterP;
    real32 Radius;
    
    v2 StartP;
    v2 EndP;
    real32 Speed;
    
    asteroid_state State;
    
    v3 Color;
    
    // NOTE(Eric): Testing this out, not sure what I'm doing
    // This is the Position in the Tile, relative to Tile.BottomLeft
    // Values must be Greater than 0, Less than TileSize
    // If they are out of those bounds, needs to change Tiles
    v2 TileRelP;
    
    // X and Y Index of which Tile we are on
    v2 TileIndex;
};

struct collision_hash
{
    asteroid Asteroids[128]; // Size here is the max number of Asteroids in a single bucket
    
    collision_hash *NextInHash;
};

struct game_state
{
    // NOTE(Eric): These are to compare against the Render width/height to update the array
    s32 RenderWidth;
    s32 RenderHeight;
    real32 RenderHalfWidth;
    real32 RenderHalfHeight;
    
    player Player;
    
    u32 AsteroidCount;
    asteroid Asteroids[256];
    
    // NOTE(Eric): I beleive this has to come last in game_state so it doesn't collide
    // since it's varying in size
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

inline v2
Mix(v2 A, v2 B, real32 Amount)
{
    v2 Result = {};
    Result.x = (1-Amount) * A.x + Amount * B.x;
    Result.y = (1-Amount) * A.y + Amount * B.y;
    
    return(Result);
}

#endif //ERIC_HANDMADE_H