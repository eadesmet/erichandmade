#ifndef ERIC_MATH_H
#define ERIC_MATH_H

union v2
{
    struct
    {
        real32 x;
        real32 y;
    };
    real32 E[2];
};

inline v2
V2i(u32 X, u32 Y)
{
    v2 Result = {(real32)X, (real32)Y};

    return(Result);
}

inline v2
V2i(s32 X, s32 Y)
{
    v2 Result = {(real32)X, (real32)Y};

    return(Result);
}

inline v2
V2(real32 X, real32 Y)
{
    v2 Result;

    Result.x = X;
    Result.y = Y;

    return(Result);
}

inline v2
operator*(real32 A, v2 B)
{
    v2 Result;

    Result.x = A*B.x;
    Result.y = A*B.y;

    return(Result);
}

inline v2
operator*(v2 B, real32 A)
{
    v2 Result = A*B;

    return(Result);
}

inline v2 &
operator*=(v2 &B, real32 A)
{
    B = A * B;

    return(B);
}

inline v2 &
operator/(v2 &B, real32 A)
{
    B.x = B.x / A;
    B.y = B.y / A;

    return(B);
}

inline v2
operator-(v2 A)
{
    v2 Result;

    Result.x = -A.x;
    Result.y = -A.y;

    return(Result);
}

inline v2
operator+(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return(Result);
}

inline v2
operator+(v2 A, real32 B)
{
    v2 Result;

    Result.x = A.x + B;
    Result.y = A.y + B;

    return(Result);
}

inline v2 &
operator+=(v2 &A, v2 B)
{
    A = A + B;

    return(A);
}

inline v2
operator-(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return(Result);
}

inline v2 &
operator-=(v2 &A, v2 B)
{
    A = A - B;

    return(A);
}

inline v2
operator-(v2 A, u32 B)
{
    v2 Result;

    Result.x = A.x - B;
    Result.y = A.y - B;

    return(Result);
}

inline v2
operator-(v2 A, real32 B)
{
    v2 Result;

    Result.x = A.x - B;
    Result.y = A.y - B;

    return(Result);
}

inline bool32
operator==(v2 P1, v2 P2)
{
    bool32 Result = (P1.x == P2.x) && (P1.y == P2.y);

    return(Result);
}

inline bool32
operator!=(v2 P1, v2 P2)
{
    bool32 Result = (P1.x != P2.x) && (P1.y != P2.y);

    return(Result);
}

inline bool32
operator>(v2 P1, v2 P2)
{
    bool32 Result = (P1.x > P2.x) && (P1.y > P2.y);

    return(Result);
}

inline bool32
operator<(v2 P1, v2 P2)
{
    bool32 Result = (P1.x < P2.x) && (P1.y < P2.y);

    return(Result);
}

inline real32
Square(real32 A)
{
    real32 Result = A*A;

    return(Result);
}

inline real32
Inner(v2 A, v2 B)
{
    real32 Result = A.x*B.x + A.y*B.y;

    return(Result);
}

inline real32
LengthSq(v2 A)
{
    real32 Result = Inner(A, A);

    return(Result);
}

inline real32
Length(v2 A)
{
    real32 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline v2
Normalize(v2 A)
{
    // NOTE(Eric): A Normalized vector is a vector that has a length of 1 (but same direction)
    v2 Result = A / Length(A);

    return(Result);
}

inline real32
RadiansToDegrees(real32 R)
{
    real32 Result = R * (180 / Pi32);

    return(Result);
}

inline real32
Slope(v2 P1, v2 P2)
{
    // TODO(EriC): Check for divide by 0!
    real32 Result = (P2.y - P1.y) / (P2.x - P1.x);

    return(Result);
}

//~ V3

// NOTE(Eric): V4 For colors instead? Do we want an Alpha value?
union v3
{
    struct
    {
        real32 r;
        real32 g;
        real32 b;
    };
    struct
    {
        real32 x;
        real32 y;
        real32 z;
    };
    real32 E[3];
};

inline v3
V3(real32 X, real32 Y, real32 Z)
{
    v3 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return(Result);
}

// TODO(Eric): v3 functions

inline bool32
operator==(v3 P1, v3 P2)
{
    bool32 Result = (P1.x == P2.x) && (P1.y == P2.y) && (P1.z == P2.z);

    return(Result);
}

#endif //ERIC_MATH_H
