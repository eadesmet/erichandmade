
#include "platform.h"
#include <math.h>
#include "eric_math.h"
#include "intrinsics.h"

global_variable v2 MinV2 = v2{0,0};
//global_variable v2 MaxV2 = v2{0,0};

#define TILE_SIZE 20
#define MAP_WIDTH 800
#define MAP_HEIGHT 600

#define TILE_COUNT_X MAP_WIDTH / TILE_SIZE
#define TILE_COUNT_Y MAP_HEIGHT / TILE_SIZE

#define OFFSET_X 40
#define OFFSET_Y 40

struct tile
{
    v2 BottomLeft;
};

struct screen_map
{
    tile Tiles[TILE_COUNT_Y][TILE_COUNT_X];
    
    bool32 IsInitialized;
};

inline tile
GetTileAtPosition(screen_map *Map, v2 Pos)
{
    int IndexX = Pos.x / TILE_SIZE;
    int IndexY = Pos.y / TILE_SIZE;
    
    return (Map->Tiles[IndexY][IndexX]);
}

// NOTE(ERIC): Functions in a header file???
inline game_controller_input *GetController(game_input *Input, int unsigned ControllerIndex)
{
    Assert(ControllerIndex < ArrayCount(Input->Controllers));
    
    game_controller_input *Result = &Input->Controllers[ControllerIndex];
    return(Result);
}



inline int
Clamp(int Min, int Value, int Max)
{
    if (Value < Min) return Min;
    if (Value > Max) return Max;
    return Value;
}

inline real32
ClampReal32(real32 Min, real32 Value, real32 Max)
{
    if (Value < Min) return Min;
    if (Value > Max) return Max;
    return Value;
}

inline v2
ClampV2(v2 Min, v2 Value, v2 Max)
{
    if (Value.x < Min.x) Value.x = Min.x;
    if (Value.y < Min.y) Value.y = Min.y;
    if (Value.x > Max.x) Value.x = Max.x;
    if (Value.y > Max.y) Value.y = Max.y;
    
    return(Value);
}

inline void
SwapV2(v2 *Source, v2 *Dest)
{
    v2 *Temp;
    Temp = Dest;
    
    Dest = Source;
    Source = Temp;
}


