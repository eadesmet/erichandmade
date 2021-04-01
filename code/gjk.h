/* date = April 1st 2021 10:32 am */
#ifndef GJK_H
#define GJK_H

global_variable v2 Origin = {0,0};

// NOTE(Eric): A 'Simplex' is the simplest shape that is guaranteed to enclose a point.
// For 2d, the shape ends up being a triangle.

struct simplex_node
{
    v2 P;
};

struct simplex
{
    simplex_node Nodes[3];
    u32 Count;
};

inline simplex
NewSimplex()
{
    simplex Result = {};
    
    return(Result);
}

inline void
AddSimplexP(simplex *Simplex, v2 P)
{
    simplex_node NewNode = {};
    NewNode.P = P;
    
    Simplex->Nodes[Simplex->Count++] = NewNode;
}

inline void
RemoveSimplexP(simplex *Simplex, u32 Index)
{
    for(u32 i = Index; i < Simplex->Count; i++)
        Simplex->Nodes[i] = Simplex->Nodes[i+1];
    
    Simplex->Count--;
}


#endif //GJK_H
