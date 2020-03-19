#ifndef ERIC_HANDMADE_H
#define ERIC_HANDMADE_H

#include "platform.h"
#include <math.h>
#include "eric_math.h"
#include "intrinsics.h"
#include "tile.h"


global_variable v2 MinV2 = v2{0,0};
//global_variable v2 MaxV2 = v2{0,0};






/*
inline int
Clamp(int Min, int Value, int Max)
{
    if (Value < Min) return Min;
    if (Value > Max) return Max;
    return Value;
}
*/


struct game_state
{
    screen_map Map;
    
    // NOTE(Eric): TEMPORARY
    v2 PlayerPosition;
    v2 DeltaPlayerPosition;
};


#endif //ERIC_HANDMADE_H