#ifndef TILE_H
#define TILE_H

#define TILE_SIZE 20

struct tile
{
    v2 BottomLeft;
    
    // NOTE(Eric): Not Used yet? Do we need it?
    tile *NextTile;
};

struct screen_map
{
    u32 TileCountX;
    u32 TileCountY;
    
    // NOTE(Eric): Index is: (Y-Index * TileCountX + X-Index)
    // TODO(Eric): This is tough to work with because the entries are not sequential
    // and there are a lot of empty spots. Need a different solution here.
    tile Tiles[4096];
};

struct colliding_tiles_result
{
    u32 Count;
    
    tile Tiles[4096];
};

#endif //TILE_H
