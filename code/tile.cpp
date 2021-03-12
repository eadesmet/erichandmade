
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
            // NOTE(Eric): I don't understand this code. What is u32() ..? u32 is my type, but used as a function???
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
GamePointToScreenPoint(v2 GamePoint, bool32 UseMeters = true)
{
    // GamePoint = Coordinate System in our Game with the center being the origin
    // ScreenPoint (Result) = Coordinate in pixels on the screen.
    v2 Result = {};

    // UseMeters thing is wonky: rendering the player breaks when doing meters here.
    // And changing the player size to match didn't work?
    if (UseMeters)
        Result = (GamePoint * METERS_TO_PIXELS) + ScreenCenter;
    else
        Result = (GamePoint) + ScreenCenter;

    return(Result);

}
inline v2
GPToSP(v2 GamePoint, bool32 UseMeters = true)
{
    return (GamePointToScreenPoint(GamePoint, UseMeters));
}

// TODO(Eric): Probably don't want something like this? Everything should be converted to GamePoints
inline v2
ScreenPointToGamePoint(v2 ScreenPoint, bool32 UseMeters = true)
{
    v2 Result = {};

    if (UseMeters)
    {
        v2 Value = ScreenPoint - ScreenCenter;
        Result = Value / METERS_TO_PIXELS;

        // NOTE(Eric): 'argument' conversion from 'v2' to 'v2 &'   ?????????????
        //Result = (ScreenPoint - ScreenCenter) / METERS_TO_PIXELS;
    }
    else
        Result = ScreenPoint - ScreenCenter;

    return(Result);
}
inline v2
SPToGP(v2 ScreenPoint, bool32 UseMeters = true)
{
    return (ScreenPointToGamePoint(ScreenPoint, UseMeters));
}

inline real32
PixelsToMeters(real32 Pixel)
{
    real32 Result = 0;

    Result = Pixel / METERS_TO_PIXELS;

    return(Result);
}