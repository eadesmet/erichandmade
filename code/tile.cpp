
#include "tile.h"


inline void
InitMap(game_state *GameState)
{
    u32 Width = GameState->RenderWidth;
    u32 Height = GameState->RenderHeight;
    
    screen_map *Map = &GameState->Map;
    
    Map->TileCountX = RoundReal32ToInt32((real32)Width / (real32)TILE_SIZE);
    Map->TileCountY = RoundReal32ToInt32((real32)Height / (real32)TILE_SIZE);
    
    for (u32 IndexY = 0; IndexY < Map->TileCountY; ++IndexY)
    {
        for (u32 IndexX = 0; IndexX < Map->TileCountX; ++IndexX)
        {
            Map->Tiles[u32(IndexY*Map->TileCountX + IndexX)].BottomLeft = V2((real32)((IndexX)*(TILE_SIZE)), (real32)((IndexY)*(TILE_SIZE)));
        }
    }
}

// NOTE(Eric): 'Is In Rectangle'? (Instead of being dependent on the tilemap)?
inline tile
GetTileAtPosition(screen_map *Map, v2 Pos)
{
    tile Result = {};
    bool32 Found = 0;
    u16 IndexY = 0;
    
    if (Map->TileCountX * TILE_SIZE > Pos.x ||
        Map->TileCountY * TILE_SIZE > Pos.y ||
        0 < Pos.x ||
        0 < Pos.y)
    {
        // TODO(ERIC): do something here for a tile that wasn't found
        tile NullTile = {};
        return(NullTile);
    }
    
    while(!Found)
    {
        for (u16 IndexX = 0; IndexX < Map->TileCountX; ++IndexX)
        {
            tile Tile = Map->Tiles[IndexY*Map->TileCountX + IndexX];
            if ((Pos.x >= Tile.BottomLeft.x) &&
                ((Tile.BottomLeft.x + TILE_SIZE) >= Pos.x))
            {
                if ((Pos.y >= Tile.BottomLeft.y) &&
                    ((Tile.BottomLeft.y + TILE_SIZE) >= Pos.y))
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

inline colliding_tiles_result
GetTilesInSquare(screen_map *Map, v2 Pos, u32 SquareSize)
{
    // Return all of the Tiles that are colliding with Square
    colliding_tiles_result Result = {};
    
    u32 MinIndexX = (u32)Pos.x / TILE_SIZE;
    u32 MinIndexY = (u32)Pos.y / TILE_SIZE;
    u32 MaxIndexX = RoundReal32ToUInt32((Pos.x + SquareSize)/TILE_SIZE);
    u32 MaxIndexY = RoundReal32ToUInt32((Pos.y + SquareSize)/TILE_SIZE);
    
    for (u32 IndexY = MinIndexY; IndexY < MaxIndexY; ++IndexY)
    {
        for (u32 IndexX = MinIndexX; IndexX < MaxIndexX; ++IndexX)
        {
            u32 TileIndex = (IndexY * Map->TileCountX) + IndexX;
            //tile FoundTile = Map->Tiles[TileIndex];
            
            Result.Tiles[Result.Count] = Map->Tiles[TileIndex];
            Result.Count++;
            //*Result.Tiles = FoundTile;
        }
    }
    
    return(Result);
}


inline v2
MetersToGamePoint(game_state *GameState, v2 PointInMeters)
{
    v2 Result = {};
    
    real32 MetersToPixels = 10;
    Result = PointInMeters * MetersToPixels;
    
    return(Result);
}

inline v2
GamePointToScreenPoint(game_state *GameState, v2 GamePoint)
{
    // GamePoint = Coordinate System in our Game with the center being the origin
    // ScreenPoint/Result = Coordinate in pixels on the screen.
    v2 Result = {};
    
    v2 ScreenCenter = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    Result = GamePoint + ScreenCenter;
    
    return(Result);
    
}