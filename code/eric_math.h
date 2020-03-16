#ifndef ERIC_MATH_H
#define ERIC_MATH_H

struct v2
{
    s32 x;
    s32 y;
};

inline v2
V2(s32 X, s32 Y)
{
    v2 Result;
    
    Result.x = X;
    Result.y = Y;
    
    return(Result);
}

inline v2
operator+(v2 P1, v2 P2)
{
    v2 Result = {};
    
    Result.x = P1.x + P2.x;
    Result.y = P1.y + P2.y;
    
    return(Result);
}

inline real32
Square(real32 A)
{
    real32 Result = A*A;
    
    return(Result);
}

#endif //ERIC_MATH_H
