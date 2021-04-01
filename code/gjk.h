/* date = April 1st 2021 10:32 am */
#ifndef GJK_H
#define GJK_H

// NOTE(Eric): This setup is kinda weird
// I can just make some assumptions and remove some of this junk, right?

global_variable v2 Origin = {0,0};

// NOTE(Eric): A 'Simplex' is the simplest shape that is guaranteed to enclose a point.
// For 2d, the shape ends up being a triangle.
struct simplex_node
{
    v2 P;
    simplex_node *Next;
};

struct simplex
{
    simplex_node *First;
    u32 Count;
};

inline void
AddSimplexP(simplex *Simplex, v2 P)
{
    simplex_node *Node = Simplex->First + Simplex->Count++;
    Node->P = P;
    Node->Next = 0;
}

inline void
RemoveSimplexP(simplex *Simplex, u32 Index)
{
    simplex_node *ToRemove = Simplex->First + Index;
    
    if (Index == 0)
    {
        // If removing the first item, shift the list over by 1
        Simplex->First = Simplex->First + 1;
    }
    else if (Simplex->Count == Index)
    {
        // If removing the last item, just remove it
        simplex_node *Node = Simplex->First + Index - 1;
        Node->Next = 0;
    }
    else
    {
        // If removing from the middle, point the previous item to the next item
        simplex_node *Prev = ToRemove-1;
        Prev->Next = ToRemove+1;
    }
    
    Simplex->Count--;
}


#endif //GJK_H
