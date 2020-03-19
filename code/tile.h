#ifndef TILE_H
#define TILE_H

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
};


#endif //TILE_H
