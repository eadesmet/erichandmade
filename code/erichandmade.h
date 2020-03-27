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
    
    // NOTE(Eric): Probably not needed
    real32 FacingDirectionAngle;
};

struct game_state
{
    screen_map Map;
    
    // NOTE(Eric): TEMPORARY
    v2 PlayerPosition;
    v2 DeltaPlayerPosition;
    
    player Player;
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




#endif //ERIC_HANDMADE_H