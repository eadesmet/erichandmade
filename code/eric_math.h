#ifndef ERIC_MATH_H
#define ERIC_MATH_H

inline int
Clamp(int Min, int Value, int Max)
{
    if (Value < Min) return Min;
    if (Value > Max) return Max;
    return Value;
}

inline real32
Clamp(real32 Min, real32 Value, real32 Max)
{
    if (Value < Min) return Min;
    if (Value > Max) return Max;
    return Value;
}

// NOTE(Eric): Linear Interpolation or lerp
inline real32
Mix(real32 A, real32 B, real32 Amount)
{
    real32 Result = (1-Amount) * A + Amount * B;

    return(Result);
}
inline real32
Lerp(real32 A, real32 B, real32 Amount)
{
    real32 Result = (1-Amount) * A + Amount * B;

    return(Result);
}


// ------------ v2
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
Slope(v2 P1, v2 P2)
{
    // TODO(EriC): Check for divide by 0!
    real32 Result = (P2.y - P1.y) / (P2.x - P1.x);

    return(Result);
}

inline v2
Mix(v2 A, v2 B, real32 Amount)
{
    v2 Result = {};
    Result.x = (1-Amount) * A.x + Amount * B.x;
    Result.y = (1-Amount) * A.y + Amount * B.y;

    return(Result);
}
inline v2
Lerp(v2 A, v2 B, real32 Amount)
{
    v2 Result = {};
    Result.x = (1-Amount) * A.x + Amount * B.x;
    Result.y = (1-Amount) * A.y + Amount * B.y;

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

inline u32
GetColor(v3 Values)
{
    u32 Result = ((RoundReal32ToUInt32(Values.r * 255.0f) << 16) |
                  (RoundReal32ToUInt32(Values.g * 255.0f) << 8) |
                  (RoundReal32ToUInt32(Values.b * 255.0f) << 0));
    return(Result);
}

// --------

inline real32
RadiansToDegrees(real32 R)
{
    real32 Result = R * (180 / Pi32);

    return(Result);
}

inline real32
Min3(real32 a, real32 b, real32 c)
{
    real32 Result = Minimum(a,b);
    Result = Minimum(Result, c);
    return(Result);
}

inline real32
Max3(real32 a, real32 b, real32 c)
{
    real32 Result = Maximum(a,b);
    Result = Maximum(Result, c);
    return(Result);
}


#endif //ERIC_MATH_H
