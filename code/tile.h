#ifndef TILE_H
#define TILE_H

#define TILE_SIZE 40

#define METERS_TO_PIXELS 10.0f

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
    tile Tiles[4096];
    
    
    /*
     A resolution of 1920 x 1080, tile size of 20, 4096 is not enough space

    */
    
    //c 1920*1080
    
};

struct colliding_tiles_result
{
    u32 Count;
    
    tile Tiles[4096];
};

#endif //TILE_H
