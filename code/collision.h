#ifndef COLLISION_H
#define COLLISION_H

struct line_collision_result
{
    bool32 IsColliding;
    v2 NewEndP;
    v2 CollisionP;
};


#endif //COLLISION_H