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

inline v2
Perp(v2 A)
{
    v2 Result = {-A.y, A.x};
    return(Result);
}

inline real32
Inner(v2 A, v2 B)
{
    // NOTE(Eric): This is the Dot Product!
    // Length(A)*Length(B)*cos(Theta) == A.x*B.x + A.y*B.y
    real32 Result = A.x*B.x + A.y*B.y;
    
    return(Result);
}

inline real32
Dot(v2 A, v2 B)
{
    return (Inner(A, B));
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
TripleProduct(v2 a, v2 b, v2 c)
{
    v2 Result;
    
    float ac = a.x * c.x + a.y * c.y; // perform a.dot(c)
    float bc = b.x * c.x + b.y * c.y; // perform b.dot(c)
    
    // perform b * a.dot(c) - a * b.dot(c)
    Result.x = b.x * ac - a.x * bc;
    Result.y = b.y * ac - a.y * bc;
    
    return Result;
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

inline v2
Clamp(v2 Min, v2 Value, v2 Max)
{
    if (Value < Min) return Min;
    if (Value > Max) return Max;
    return Value;
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
inline v3
operator-(v3 A, v3 B)
{
    v3 Result;
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    
    return(Result);
}

inline real32
Inner(v3 A, v3 B)
{
    // NOTE(Eric): This is the Dot Product!
    // Length(A)*Length(B)*cos(Theta) == A.x*B.x + A.y*B.y
    real32 Result = A.x*B.x + A.y*B.y + A.z*B.z;
    
    return(Result);
}

inline real32
Dot(v3 A, v3 B)
{
    return (Inner(A, B));
}

inline v3
Cross(v3 A, v3 B)
{
    v3 Result;
    
    Result.x = A.y*B.z - A.z*B.y;
    Result.y = A.z*B.x - A.x*B.z;
    Result.z = A.x*B.y - A.y*B.x;
    
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
