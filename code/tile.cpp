
#include "tile.h"

// NOTE(Eric): Is In Rectangle? (Instead of being dependent on the tilemap?

inline tile
GetTileAtPosition(screen_map *Map, v2 Pos)
{
    // NOTE(Eric): I'm not sure if this is a great way to get a tile
    // It's certainly efficient, but I can't quite get the equation correct.
    // It's also very dependent on the indexes being off the size / offset
    /*
    int IndexX = Pos.x / (TILE_SIZE + OFFSET_X);
    int IndexY = Pos.y / (TILE_SIZE + OFFSET_Y);
    
    return (Map->Tiles[IndexY][IndexX]);
*/
    
    Assert(Pos.x <= MAP_WIDTH);
    Assert(Pos.y <= MAP_HEIGHT);
    
    //Pos.x = Clamp(0, Pos.x, MAP_WIDTH);
    //Pos.y = Clamp(0, Pos.y, MAP_HEIGHT);
    
    // NOTE(Eric): The -1 is so it does not _include_ the tile for exact matches
    
    tile Result = {};
    bool32 Found = 0;
    u16 IndexY = 0;
    
    while(!Found)
    {
        for (u16 IndexX = 0; IndexX <= TILE_COUNT_X; ++IndexX)
        {
            tile Tile = Map->Tiles[IndexY][IndexX];
            if ((Pos.x >= Tile.BottomLeft.x) &&
                ((Tile.BottomLeft.x + TILE_SIZE-1) >= Pos.x))
            {
                if ((Pos.y >= Tile.BottomLeft.y) &&
                    ((Tile.BottomLeft.y + TILE_SIZE-1) >= Pos.y))
                {
                    Result = Tile;
                    Found = true;
                    break;
                }
            }
        }
        ++IndexY;
    }
    
    return(Result);
}
