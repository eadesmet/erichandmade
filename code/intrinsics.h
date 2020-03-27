#ifndef INTRINSICS_H
#define INTRINSICS_H


inline real32
SquareRoot(real32 Real32)
{
    real32 Result = sqrtf(Real32);
    return(Result);
}

inline real32
AbsoluteValue(real32 Real32)
{
    real32 Result = (real32)fabs(Real32);
    return(Result);
}

inline real32
RoundReal32(real32 Real32)
{
    real32 Result = roundf(Real32);
    return(Result);
}

inline s32
RoundReal32ToInt32(real32 Real32)
{
    s32 Result = (s32)roundf(Real32);
    return(Result);
}

inline u32
RoundReal32ToUInt32(real32 Real32)
{
    u32 Result = (u32)roundf(Real32);
    return(Result);
}

inline s32
FloorReal32ToInt32(real32 Real32)
{
    s32 Result = (s32)floorf(Real32);
    return(Result);
}

inline s32
CeilReal32ToInt32(real32 Real32)
{
    s32 Result = (s32)ceilf(Real32);
    return(Result);
}

inline s32
TruncateReal32ToInt32(real32 Real32)
{
    s32 Result = (s32)Real32;
    return(Result);
}

inline real32
Sin(real32 Angle)
{
    real32 Result = sinf(Angle);
    return(Result);
}

inline real32
Cos(real32 Angle)
{
    real32 Result = cosf(Angle);
    return(Result);
}

inline real32
ATan2(real32 Y, real32 X)
{
    real32 Result = atan2f(Y, X);
    return(Result);
}


#endif //INTRINSICS_H
