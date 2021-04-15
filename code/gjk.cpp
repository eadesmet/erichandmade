#include "gjk.h"

internal v2
FurthestPoint(entity *E, v2 Direction)
{
    // NOTE(Eric): A support point is the furthest point along the direction
    // ONLY WORKS FOR CONVEX SHAPES!
    // It is defined as the maximum dot product of all vertices;
    v2 Result = {};
    
    f32 Max = -FLOAT_MAX;//-99999.0f;
    for (u32 PointIndex = 0; PointIndex < ArrayCount(E->Points); ++PointIndex)
    {
        v2 *Point = E->Points + PointIndex;
        
        f32 DotProd = Inner(*Point, Direction);
        if (DotProd > Max)
        {
            Max = DotProd;
            Result = *Point;
        }
    }
    
    return(Result);
}

internal v2
Support(entity *E1, entity *E2, v2 Direction)
{
    v2 Result = FurthestPoint(E1, Direction) - FurthestPoint(E2, -Direction);
    
    return(Result);
}

internal b32
HandleLineSimplex(simplex *Simplex, v2 *Direction)
{
    // TODO(Eric): Handle edge case when the Origin lies on the line.
    
    v2 A = (Simplex->Nodes + Simplex->Count-1)->P;
    v2 B = (Simplex->Nodes + Simplex->Count-2)->P;
    
    v2 BA = B - A;
    v2 BO = Origin - B;
    
    // TODO(Eric): I don't know if this is right.
    // All resources I've seen use the cross product as in the comment,
    // but cross product only makes sense for v3.
    // So would it make sense to change all these to v3s with Z=0?
    v2 BAPerp = Perp(BA);//Cross(Cross(BA, BO), BA);
    
    // Set the new direction to be the perpendicular of the line
    Direction = &BAPerp;
    
    // Returns false because we only have a line, 
    // and the origin cannot be contained within the simplex yet.
    return(false); 
}

internal b32
HandleTriangleSimplex(simplex *Simplex, v2 *Direction)
{
    // NOTE(Eric): We now have 3 points in the simplex, find which reigon of space the origin lies
    
    // NOTE(Eric): A is the most recently added point
    v2 A = (Simplex->Nodes + Simplex->Count-1)->P;
    v2 B = (Simplex->Nodes + Simplex->Count-2)->P;
    v2 C = (Simplex->Nodes + Simplex->Count-3)->P;
    
    v2 AB = B - A;
    v2 AC = C - A;
    v2 AO = Origin - A;
    
    v2 ABPerp = Perp(AB);
    v2 ACPerp = Perp(AC);
    
    if (Inner(ABPerp, AO) > 0) // In Region AB
    {
        RemoveSimplexP(Simplex, Simplex->Count-2); // Remove C
        *Direction = ABPerp;
        return(false);
    }
    else if (Inner(ACPerp, AO) > 0) // In Region AC
    {
        RemoveSimplexP(Simplex, Simplex->Count-2); // Remove B
        *Direction = ACPerp;
        return(false);
    }
    
    // Our triangle contains the origin
    return(true);
}

internal b32
HandleSimplex(simplex *Simplex, v2 *Direction)
{
    if (Simplex->Count == 2)
        return HandleLineSimplex(Simplex, Direction);
    return HandleTriangleSimplex(Simplex, Direction);
}

internal b32
GJK(entity *E1, entity *E2)
{
    // NOTE(Eric): GJK checks the Minkowski difference between two shapes
    // if they contain the origin. If they do, then the two shapes are colliding.
    // GJK simplifies it by only checking a series of Simplexes (in our case triangles)
    // for the origin, instead of the entire minkowski difference.
    
    // TODO(Eric): Convert the entities shapes into Convex
    
    // TODO(Eric): Also realize that the Points in our entities are RELATIVE TO THE CENTERP
    // TODO(Eric): THIS MEANS EVERY ENTITY WILL ALWAYS BE COLLIDING IF WE DON'T CONVERT THEM FIRST
    
    // The Simplex is a triangle within the Minkowski difference
    simplex Simplex = NewSimplex();
    
    // The first direction (can be picked randomly, but this is standard)
    v2 D = Normalize(E2->CenterP - E1->CenterP);
    
    // The first point of the Simplex is in from our first direction
    AddSimplexP(&Simplex, Support(E1, E2, D));
    
    // Next direction is Towards the origin
    D = Origin - Simplex.Nodes[0].P;
    while(true)
    {
        // New support point, towards the origin
        AddSimplexP(&Simplex, Support(E1, E2, D));
        
        // If the new support point is not past the origin, then we are not colliding
        v2 NewestPoint = Simplex.Nodes[Simplex.Count-1].P;
        if (Inner(NewestPoint, D) < 0)
            return(false);
        
        if (HandleSimplex(&Simplex, &D))
            return(true);
    }
}


//~---------------------------------


internal int
WhichDirection(v2 p, v2 q, v2 r)
{
    // Checks the direction of the next point
    // Given line p to q, which side does r lie?
    // -1 : q lies to the left (qr turns Clockwise)
    //  1 : q lies to the right(qr turns Counter-Clockwise)
    //  0 : q lies on the line from p to r
    int Result;
    
    r32 Value = (p.x - q.x) * (q.y - r.y) - (p.y - q.y) * (q.x - r.x);
    if (Value < 0)
        Result = -1;
    else if (Value > 0)
        Result = 1;
    else
        Result = 0;
    
    return Result;
}

internal b32
CheckConcave(entity *E)
{
    b32 Result = false;
    
    // Assumes at least 3 points (starting index 3)
    v2 *p = E->Points;
    v2 *q = E->Points + 1;
    for (u32 Index = 2;
         Index < E->PointCount; 
         Index++, p = E->Points + (Index-2), q = E->Points + (Index-1))
    {
        // Checking the turn of r, with line segment p to q
        v2 *r = E->Points + Index;
        
        // Given our assumptions, it must always turn clockwise
        if (WhichDirection(*p, *q, *r) != -1)
            return true;
        
    }
    
    return Result;
}

internal entity
ConvertConcaveToConvex(entity *E)
{
    // TODO(Eric): Implement.
    
    /*
To check if a polygon is convex:
"
For general n-gons, not just quads, a straightforward solution is to, for each polygon edge,
test to see if all other vertices lie (strictly) on the same side of that edge.
If the test is true for all edges, the polygon is convex, and otherwise it is concave.
A separate check for coincident vertices is required to make the rest robust.
However, although easy to implement, this test is expensive for large polygons,
with an O(n^2) complexity in the number of vertices.

A strictly convex polygon has interior angles that are all less than 180 degrees.
However, this test only works if the polygon is known not to be self-intersecting.

"

Could also check that each vertex takes a right turn.
This won't work for shapes like a pentagram, however we can assume nothing is like that
(At this point in time, anyway)
*/
    
    entity Result = {};
    
    
    return Result;
}


internal void
Barycentric(v2 A, v2 B, v2 C, v2 P, r32 &U, r32 &V, r32 &W)
{
    // NOTE(Eric): Taken from "Real-Time Collision Detection" by Christer Ericson
    // Computing barycentric coordinates (u, v, w) for
    // point P with respect to triangle (a, b, c)
    v2 Vec0 = B - A;
    v2 Vec1 = C - A;
    v2 Vec2 = P - A;
    
    // NOTE(Eric): If several points are tested against the same triangle,
    // d00, d01, d11, and denom only have to be computed once, as they are fixed for a given triangle
    real32 d00 = Dot(Vec0, Vec0);
    real32 d01 = Dot(Vec0, Vec1);
    real32 d11 = Dot(Vec1, Vec1);
    real32 d20 = Dot(Vec2, Vec0);
    real32 d21 = Dot(Vec2, Vec1);
    
    real32 denom = d00 * d11 - d01 * d01;
    
    V = (d11 * d20 - d01 * d21) / denom;
    W = (d00 * d21 - d01 * d20) / denom;
    U = 1.0f - V - W;
    
    // Huh.. So this is actually telling us if point P is within the triangle A,B,C
    // if 0 <= V,W,Y <= 1
}

internal b32
CheckPointInTriangle(v2 A, v2 B, v2 C, v2 P)
{
    r32 U, V, W;
    Barycentric(A, B, C, P, U, V, W);
    return (V >= 0.0f &&
            W >= 0.0f &&
            V+W <= 1.0f);
}

#if 0
internal b32
IsConvexQuad(v3 A, v3 B, v3 C, v3 D)
{
    // NOTE(Eric): This doesn't work?
    // NOTE(Eric): Also taken from "Real-Time Collision Detection"
    
    // Quad is nonconvex if Dot(Cross(bd, ba), Cross(bd, bc)) >= 0
    v3 bda = Cross(D - B, A - B);
    v3 bdc = Cross(D - B, C - B);
    if (Dot(bda, bdc) >= 0.0f) return 0;
    
    v3 acd = Cross(C - A, D - A);
    v3 acb = Cross(C - A, B - A);
    return Dot(acd, acb) < 0.0f;
}
#endif

