#include "gjk.h"

internal v2
FurthestPoint(entity *E, v2 Direction)
{
    // NOTE(Eric): A support point is the furthest point along the direction
    // ONLY WORKS FOR CONVEX SHAPES!
    // It is defined as the maximum dot product of all vertices;
    v2 Result = {};
    
    f32 Max = -99999.0f;
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

internal entity
ConvertConcaveToConvex(entity *E)
{
    for (u32 Index = 0; Index < E->PointCount; Index++)
    {
        v2 *P = E->Points + Index;
        
        // TODO(Eric): This.
    }
}