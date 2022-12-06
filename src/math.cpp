#pragma once

#include <math.h>
#include <cmath>
#include <stdlib.h> // for rand()

#define PI  3.14159265359
#define TAU 6.28318530718

static inline float PowerF32(float, float);

union v2
{
    struct
    {
        float x,y;
    };

    struct
    {
        float u,v;
    };
    
    struct
    {
        float Width, Height;
    };

    float Elements[2];

    v2() = default;
    v2(float ix, float iy)
    {
        x = ix;
        y = iy;
    }
};

union v3
{
    struct
    {
        float x,y,z;
    };

    struct
    {
        float r,g,b;
    };

    struct
    {
        union
        {
            v2 xy;
            v2 rg;
        };
        float Ignored;
    };

    struct
    {
        float Ignored;
        union
        {
            v2 yz;
            v2 gb;
        };
    };
    
    float Elements[3];

    v3() = default;
    v3(float ix, float iy, float iz)
    {
        x = ix;
        y = iy;
        z = iz;
    }
};

union v4
{
    struct
    {
        float x,y,z,w;
    };

    struct
    {
        float r,g,b,a;
    };

    struct
    {
        union
        {
            v3 xyz;
            v3 rgb;
        };
        float Ignored;
    };
    
    struct
    {
        union
        {
            v2 xy;
            v2 rg;
        };
        float Ignored0;
        float Ignored1;
    };

    float Elements[4];

    v4() = default;
    v4(float ix, float iy, float iz, float iw)
    {
        x = ix;
        y = iy;
        z = iz;
        w = iw;
    }
    v4(v3 i, float iw)
    {
        x = i.x;
        y = i.y;
        z = i.z;
        w = iw;
    }
};

struct m4
{ 
    float Elements[4][4];
};

inline void
SetColumn(m4& M, int Column, float A1, float A2, float A3, float A4)
{
    M.Elements[Column][0] = A1;
    M.Elements[Column][1] = A2;
    M.Elements[Column][2] = A3;
    M.Elements[Column][3] = A4;
}

inline void
SetRow(m4& M, int Row, float A1, float A2, float A3, float A4)
{
    M.Elements[0][Row] = A1;
    M.Elements[1][Row] = A2;
    M.Elements[2][Row] = A3;
    M.Elements[3][Row] = A4;
}

inline m4
Transpose(const m4& A)
{
    m4 Result;
    for(int Row = 0;
        Row < 4;
        ++Row)
    {
        for(int Column = 0;
            Column < 4;
            ++Column)
        {
            Result.Elements[Column][Row] = A.Elements[Row][Column];
        }
    }

    return Result;
}

inline v4
operator*(const m4& M, const v4& A)
{
    v4 Result;
    for(int Column = 0;
        Column < 4;
        ++Column)
    {
        ((float*)&Result)[Column] =
            M.Elements[0][Column] * A.x +
            M.Elements[1][Column] * A.y +
            M.Elements[2][Column] * A.z +
            M.Elements[3][Column] * A.w;
    }
    return Result;
}

inline v2 operator+(v2 L, v2 R)    {return {L.x + R.x, L.y + R.y};}
inline v2 operator-(v2 L, v2 R)    {return {L.x - R.x, L.y - R.y};}
inline v2 operator+(v2 L, float R) {return {L.x + R, L.y + R};}
inline v2 operator-(v2 L, float R) {return {L.x - R, L.y - R};}
inline v2 operator*(float L, v2 R) {return {L * R.x, L * R.y};}
inline v2 operator*(v2 L, float R) {return {L.x * R, L.y * R};}
inline v2 operator/(float L, v2 R) {return {L / R.x, L / R.y};}
inline v2 operator/(v2 L, float R) {return {L.x / R, L.y / R};}
inline v2 operator*(v2 L, v2 R)    {return {L.x * R.x, L.y * R.y};}
inline v2& operator+=(v2& L, float R) {return (L = L + R);}
inline v2& operator+=(v2& L, v2 R)    {return (L = L + R);}
inline v2& operator-=(v2& L, float R) {return (L = L - R);}
inline v2& operator-=(v2& L, v2 R)    {return (L = L - R);}
inline v2& operator*=(v2& L, float R) {return (L = L * R);}
inline v2& operator/=(v2& L, float R) {return (L = L / R);}

inline v3 operator+(v3 L, v3 R)    {return {L.x + R.x, L.y + R.y, L.z + R.z};}
inline v3 operator-(v3 L, v3 R)    {return {L.x - R.x, L.y - R.y, L.z - R.z};}
inline v3 operator+(v3 L, float R) {return {L.x + R, L.y + R, L.z + R};}
inline v3 operator-(v3 L, float R) {return {L.x - R, L.y - R, L.z - R};}
inline v3 operator*(float L, v3 R) {return {L * R.x, L * R.y, L * R.z};}
inline v3 operator*(v3 L, float R) {return {L.x * R, L.y * R, L.z * R};}
inline v3 operator/(float L, v3 R) {return {L / R.x, L / R.y, L / R.z};}
inline v3 operator/(v3 L, float R) {return {L.x / R, L.y / R, L.z / R};}
inline v3 operator*(v3 L, v3 R)    {return {L.x * R.x, L.y * R.y, L.z * R.z};}
inline v3& operator+=(v3& L, float R) {return (L = L + R);}
inline v3& operator+=(v3& L, v3 R)    {return (L = L + R);}
inline v3& operator-=(v3& L, float R) {return (L = L - R);}
inline v3& operator-=(v3& L, v3 R)    {return (L = L - R);}
inline v3& operator*=(v3& L, float R) {return (L = L * R);}
inline v3& operator/=(v3& L, float R) {return (L = L / R);}

inline v4 operator+(v4 L, v4 R)    {return {L.x + R.x, L.y + R.y, L.z + R.z, L.w + R.w};}
inline v4 operator-(v4 L, v4 R)    {return {L.x - R.x, L.y - R.y, L.z - R.z, L.w - R.w};}
inline v4 operator+(v4 L, float R) {return {L.x + R, L.y + R, L.z + R, L.w + R};}
inline v4 operator-(v4 L, float R) {return {L.x - R, L.y - R, L.z - R, L.w - R};}
inline v4 operator*(float L, v4 R) {return {L * R.x, L * R.y, L * R.z, L * R.w};}
inline v4 operator*(v4 L, float R) {return {L.x * R, L.y * R, L.z * R, L.w * R};}
inline v4 operator/(float L, v4 R) {return {L / R.x, L / R.y, L / R.z, L / R.w};}
inline v4 operator/(v4 L, float R) {return {L.x / R, L.y / R, L.z / R, L.w / R};}
inline v4 operator*(v4 L, v4 R)    {return {L.x * R.x, L.y * R.y, L.z * R.z, L.w * R.w};}
inline v4& operator+=(v4& L, float R) {return (L = L + R);}
inline v4& operator+=(v4& L, v4 R)    {return (L = L + R);}
inline v4& operator-=(v4& L, float R) {return (L = L - R);}
inline v4& operator-=(v4& L, v4 R)    {return (L = L - R);}
inline v4& operator*=(v4& L, float R) {return (L = L * R);}
inline v4& operator/=(v4& L, float R) {return (L = L / R);}

inline void
Print(v2& A)
{
#ifdef DebugLog
    DebugLog("Vec2(%f, %f)\n", A.x, A.y);
#endif
}

inline void
Print(v3& A)
{
#ifdef DebugLog
    DebugLog("Vec3(%f, %f, %f)\n", A.x, A.y, A.z);
#endif
}

inline void
Print(v4& A)
{
#ifdef DebugLog
    DebugLog("Vec4(%f, %f, %f, %f)\n", A.x, A.y, A.z, A.w);
#endif
}

inline void
Print(const m4& A)
{
#ifdef DebugLog
    DebugLog("m4 RowMajor:\n");
    DebugLog("[%f, %f, %f, %f]\n", A.Elements[0][0],A.Elements[1][0],A.Elements[2][0],A.Elements[3][0]);
    DebugLog("[%f, %f, %f, %f]\n", A.Elements[0][1],A.Elements[1][1],A.Elements[2][1],A.Elements[3][1]);
    DebugLog("[%f, %f, %f, %f]\n", A.Elements[0][2],A.Elements[1][2],A.Elements[2][2],A.Elements[3][2]);
    DebugLog("[%f, %f, %f, %f]\n", A.Elements[0][3],A.Elements[1][3],A.Elements[2][3],A.Elements[3][3]);
#endif
}

inline float
Dot(v2 a, v2 b)
{
    return a.x * b.x + a.y * b.y;
}

inline float
Dot(v3 a, v3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float
Dot(v4 a, v4 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w + b.w;
}

inline float
Length(v2 a)
{
    return sqrtf(Dot(a,a));
}

inline float
Length(v3 a)
{
    return sqrtf(Dot(a,a));
}

inline float
Length(v4 a)
{
    return sqrtf(Dot(a,a));
}

inline float
SqrLength(v2 a)
{
    return Dot(a,a);
}

inline float
SqrLength(v3 a)
{
    return Dot(a,a);
}

inline float
SqrLength(v4 a)
{
    return Dot(a,a);
}

inline v3
Cross(v3 a, v3 b)
{
    v3 r;
    r.x = a.y * b.z - a.z * b.y;
    r.y = a.z * b.x - a.x * b.z;
    r.z = a.x * b.y - a.y * b.x;
    return r;
}

inline v2
Normalize(v2 a)
{
    return a / Length(a);
}

inline v3
Normalize(v3 a)
{
    return a / Length(a);
}

inline v4
Normalize(v4 a)
{
    return a / Length(a);
}

inline float
Sqrt(float a)
{
    return sqrtf(a);
}

inline float
Rand01()
{
    return (float)rand() / (float)RAND_MAX; 
}

inline v3
RandomInUnitSphere()
{
    for(;;)
    {
        v3 p = v3(Rand01() * 2 - 1, Rand01() * 2 - 1, Rand01() * 2 - 1);
        if(SqrLength(p) >= 1) continue;
        return p;
    }
}

inline v3
RandomInHemiSphere(v3 normal)
{
    v3 rand = RandomInUnitSphere();
    return (Dot(normal, rand) > 0) ? rand : rand * -1;
}

inline v3
RandomInUnitDisk()
{
    for(;;)
    {
        v3 p = v3(Rand01()*2-1, Rand01()*2-1, 0);
        if (SqrLength(p) >= 1) continue;
        return p;
    }
}

inline float
Abs(float a)
{
    return fabs(a);
}

inline float
Min(float a, float b)
{
    return a < b ? a : b;
}

inline float
Max(float a, float b)
{
    return a > b ? a : b;
}

inline int
Max(int a, int b)
{
    return a > b ? a : b;
}

inline int
Min(int a, int b)
{
    return a < b ? a : b;
}

inline float
PowerF32(float x, float p)
{
    return pow(x, p);
}

inline v3
Reflect(v3 d, v3 n)
{
    return d - 2*Dot(d, n)*n;
}

inline v3
Refract(v3 uv, v3 n, float etai_over_etat)
{
    float cos_theta = Min(Dot(uv * -1, n), 1.0);
    v3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    v3 r_out_parallel = -Sqrt(Abs(1.0f - SqrLength(r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}

inline float
DegToRad(float deg)
{
    return deg * (float)(PI/180.0f);
}

inline float
Sin(float a)
{
    return sin(a);
}

inline float
Cos(float a)
{
    return cos(a);
}

inline float
Tan(float a)
{
    return tan(a);
}

inline float
SmoothStep(float edge0, float edge1, float x)
{
   if (x < edge0)
      return 0;
   if (x >= edge1)
      return 1;
   x = (x - edge0) / (edge1 - edge0);
   return x * x * (3 - 2 * x);
}

inline int
TruncateF32ToS32(float Value)
{
    return (int)Value;
}

inline int
RoundF32ToS32(float Value)
{
    return (int)(Value + 0.5f);
}

inline float
Clamp(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline int
Clamp(int x, int min, int max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
