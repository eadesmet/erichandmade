#include "gjk.h"

internal v2
FurthestPoint(entity *E, v2 Direction)
{
    // NOTE(Eric): A support point is the furthest point along the direction
    // ONLY WORKS FOR CONVEX SHAPES!
    // It is defined as the maximum dot product of all vertices;
    v2 Result = {};
    
    f32 Max = Inner(E->Points[0], Direction);
    for (u32 PointIndex = 1; PointIndex < E->PointCount; ++PointIndex)
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
AveragePoint(entity E)
{
    v2 Result = {};
    
    for (u32 PointIndex = 0; PointIndex < E.PointCount; ++PointIndex)
    {
        v2 *Point = E.Points + PointIndex;
        Result += *Point;
    }
    
    Result.x /= E.PointCount;
    Result.y /= E.PointCount;
    
    return Result;
}

internal v2
Support(entity *E1, entity *E2, v2 Direction)
{
    v2 E1Furthest = FurthestPoint(E1, Direction);
    v2 E2NegFurthest = FurthestPoint(E2, -Direction);
    
    v2 Result = E1Furthest - E2NegFurthest;
    
    Assert((Result.x != 0 && Result.y != 0));
    
    return(Result);
}

#if 0
internal b32
HandleLineSimplex(v2 *Simplex, v2 *Direction)
{
    v2 A = (Simplex->Nodes + Simplex->Count-1)->P;
    v2 B = (Simplex->Nodes + Simplex->Count-2)->P;
    
    v2 AB = B - A;
    v2 AO = -A;
    *Direction = TripleProduct(AB, AO, AB);
    
    if (LengthSq(*Direction) == 0)
        *Direction = Perp(AB);
    
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
    
    //Assert(A != B);
    //Assert(B != C);
    //Assert(C != A);
    
    v2 AB = B - A;
    v2 AC = C - A;
    v2 AO = Origin - A;
    
    v2 ACPerp = TripleProduct(AB, AC, AC);
    
    if (Inner(ACPerp, AO) >= 0) // In Region AB
    {
        *Direction = ACPerp;
    }
    else // In Region AC
    {
        v2 ABPerp = TripleProduct(AC, AB, AB);
        if (Inner(ABPerp, AO) < 0)
            return(true);
        
        Simplex->Nodes[0] = Simplex->Nodes[1]; // Swap first element (point C)
        *Direction = ABPerp;
    }
    
    Simplex->Nodes[1] = Simplex->Nodes[2]; // Swap element in the middle (point B)
    Simplex->Count--;
    
    return(false);
}

internal b32
HandleSimplex(v2 Simplex, u32 Index, v2 *Direction)
{
    if (Index < 2)
        return HandleLineSimplex(Simplex, Direction);
    return HandleTriangleSimplex(Simplex, Direction);
}
#endif

internal void
RelativeToActual(entity *E)
{
    for(u32 Index = 0;
        Index <= E->PointCount;
        Index++)
    {
        E->Points[Index] += E->CenterP;
    }
}
internal void
ActualToRelative(entity *E)
{
    for(u32 Index = 0;
        Index <= E->PointCount;
        Index++)
    {
        E->Points[Index] -= E->CenterP;
    }
}

internal b32
GJK(entity *E1, entity *E2)
{
    // NOTE(Eric): GJK checks the Minkowski difference between two shapes
    // if they contain the origin. If they do, then the two shapes are colliding.
    // GJK simplifies it by only checking a series of Simplexes (in our case triangles)
    // for the origin, instead of the entire minkowski difference.
    
    // NOTE(Eric): This is so that adding back the centerP at the end isn't different than what it came in with.
    entity one = *E1;
    entity two = *E2;
    RelativeToActual(&one);
    RelativeToActual(&two);
    
    // NOTE(Eric): Requires the shapes be Convex!
    
    u32 IterCount = 0;
    
    // TODO(Eric): Also realize that the Points in our entities are RELATIVE TO THE CENTERP
    // TODO(Eric): THIS MEANS EVERY ENTITY WILL ALWAYS BE COLLIDING IF WE DON'T CONVERT THEM FIRST
    
    v2 P1 = AveragePoint(one);
    v2 P2 = AveragePoint(two);
    
    // TODO(Eric): IDK, this isn't working. 
    // Kind of there, but now we aren't catching all collisions? Maybe that's due to my loop + response
    // This is really just annoying. I am very tempted to just use circle collisions and call it good.
    // In debugging, I just ripped it straight from this:
    // https://github.com/kroitor/gjk.c/blob/master/gjk.c
    
    // The Simplex is a triangle within the Minkowski difference
    //simplex Simplex = NewSimplex();
    u32 Index = 0;
    v2 Simplex[3];
    v2 a, b, c, ab, ac, ao, abperp, acperp;
    
    // The first direction (can be picked randomly, but this is standard)
    //v2 D = (E2->CenterP - E1->CenterP);
    v2 D = (P1 - P2);
    
    // If the original direction is zero, set it to an arbitrary axis
    if (D.x == 0 && D.y == 0)
        D.x = 1.0f;
    
    // The first point of the Simplex is in from our first direction
    a = Simplex[0] = Support(&one, &two, D);
    
    if (Inner(a, D) < 0)
        return false; // No collision
    
    // Next direction is Towards the origin
    D = -a;
    while(true)
    {
        Assert(IterCount++ < 1000);
        // New support point, towards the origin
        //AddSimplexP(&Simplex, Support(E1, E2, D));
        a = Simplex[++Index] = Support(&one, &two, D);
        
        // If the new support point is not past the origin, then we are not colliding
        if (Inner(a, D) <= 0)
        {
            return (false);
        }
        
        ao = -a;
        
        //if (HandleSimplex(&Simplex, Index, &D))
        if (Index < 2)
        {
            b = Simplex[0];
            ab = b - a;
            D = TripleProduct(ab, ao, ab);
            if (LengthSq(ab) == 0)
                D = Perp(ab);
            continue;
        }
        
        b = Simplex[1];
        c = Simplex[0];
        ab = b - a; // from point A to B
        ac = c - a; // from point A to C
        
        acperp = TripleProduct(ab, ac, ac);
        
        if (Inner(acperp, ao) >= 0) 
        {
            D = acperp; // new direction is normal to AC towards Origin
        } 
        else 
        {
            abperp = TripleProduct(ac, ab, ab);
            
            if (Inner(abperp, ao) < 0)
            {
                return (true); // collision
            }
            
            Simplex[0] = Simplex[1]; // swap first element (point C)
            
            D = abperp; // new direction is normal to AB towards Origin
        }
        
        Simplex[1] = Simplex[2]; // swap element in the middle (point B)
        --Index;
    }
    
    return(false);
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

internal s32
CheckConcave(entity *E)
{
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
            return Index;//return true;
    }
    
    // NOTE(Eric): Check the last point to the starting point (kind of a hack, I guess?)
    v2 *SecondToLastPoint = E->Points + E->PointCount - 2;
    v2 *LastPoint = E->Points + E->PointCount - 1;
    v2 *FirstPoint = E->Points;
    if (WhichDirection(*SecondToLastPoint, *LastPoint, *FirstPoint) != -1)
        return E->PointCount - 1;
    
    return -1;
}

internal void
ConvertConcaveToConvex(entity *E)
{
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
    
    // NOTE(Eric): (6/23/21) Are we back???
    // Just thinking about this a bit: If detecting the first 'left' turn on a concave check,
    // on the left turn, I'd just need to bump out the point (just enough so it isn't a left turn)
    s32 LeftTurnIndex = CheckConcave(E);
    while(LeftTurnIndex != -1)
    {
        // I would need to figure out which direction is "left"
        v2 *P, *NextP;
        if (LeftTurnIndex == (s32)(E->PointCount - 1))
        {
            NextP = E->Points;
        }
        else
        {
            NextP = E->Points + (LeftTurnIndex + 1);
        }
        P = E->Points + LeftTurnIndex;
        
        // So the line from LastP to NextP is valid,
        
        // Wait, honestly.. Just _remove_ that point! Shift P = NextP. That's it.
        for(u32 Index = LeftTurnIndex;
            Index < E->PointCount - 1;
            Index++)
        {
            E->Points[Index] = E->Points[Index+1];
        }
        E->Points[E->PointCount--] = {};
        //break;
        
        LeftTurnIndex = CheckConcave(E);
    }
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

