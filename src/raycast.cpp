#pragma once

#include <xmmintrin.h>
#include <smmintrin.h>

static inline float
M128Min(__m128 A)
{
    A = _mm_min_ps(A, _mm_shuffle_ps(A, A, 78));
    A = _mm_min_ps(A, _mm_shuffle_ps(A, A, 177));

    return *(float*)&A;
}

enum MaterialType
{
    LAMBERIAN,
    METAL,
    DIELECTRIC
};

struct Material
{
    MaterialType type;
    v3 color;
    f32 fuzz;
};

struct Ray
{
    v3 o;
    v3 d;
};

struct HitRecord
{
    v3 point;
    v3 normal;
    f32 t;
    bool front;
    Material mat;

    inline void SetFaceNormal(v3 ray_dir, v3 out_normal)
    {
        front  = Dot(ray_dir, out_normal) < 0;
        normal = front ? out_normal : out_normal * -1;
    }
};

struct Sphere
{
    v3 center;
    f32 radius;
    Material mat;
};

struct Triangle
{
    v3 V0;
    v3 V1;
    v3 V2;
    Material mat;
};

struct World
{
    Sphere* spheres;
    u32 count;
    Triangle* triangles;
    u32 triangle_count;
};

static f32
Reflectance(f32 cosine, f32 ref_idx)
{
    // Use Schlick's approximation for reflectance.
    auto r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*PowerF32((1 - cosine),5);
}

static bool
HitSphere(v3 ro, v3 rd, Sphere s, HitRecord& hit, f32 t_min, f32 t_max)
{
    v3 oc = ro - s.center;
    f32 a = SqrLength(rd);
    f32 b = Dot(oc, rd);
    f32 c = SqrLength(oc) - s.radius * s.radius;
    f32 discriminant = b*b - a*c;
    
    if(discriminant < 0) return false;

    f32 sqrtd = Sqrt(discriminant);
    f32 root = (-b - sqrtd) / a;
    if (root < t_min || t_max < root)
    {
        root = (-b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    hit.t = root;
    hit.point = ro + rd * hit.t;
    v3 out_normal = (hit.point - s.center) / s.radius;
    hit.SetFaceNormal(rd, out_normal);
    hit.mat = s.mat;

    return true;
}

bool HitTriangle(v3 RO, v3 RD, Triangle Tri, HitRecord& Output, f32 TMin, f32 TMax)
{
    v3 Normal = Normalize(Cross(Tri.V1 - Tri.V0, Tri.V2 - Tri.V0));
    v3 Origin = Tri.V0 + ((Tri.V1 - Tri.V0) * 0.5f) + (Tri.V2 - Tri.V0) * 0.5f;
    
    v3 Point = {};
    f32 T = 0;
    bool PlaneHit = false;
    
    { // Plane hit
        f32 Denom = Dot(Normal, RD);
        if(Abs(Denom) > 0.000001f)
        {
            T = Dot(Normal, (Origin - RO)) / Denom;

            if(T > TMin && T < TMax)
            {
                Point = RO + RD * T;
                PlaneHit = true; 
            }
        }
    }

    if(PlaneHit)
    {
        v3 Edge0 = Tri.V1 - Tri.V0;
        v3 Edge1 = Tri.V2 - Tri.V1;
        v3 Edge2 = Tri.V0 - Tri.V2;

        bool IsInsideEdge0 = Dot(Normal, Cross(Edge0, Point - Tri.V0)) > 0;
        bool IsInsideEdge1 = Dot(Normal, Cross(Edge1, Point - Tri.V1)) > 0;
        bool IsInsideEdge2 = Dot(Normal, Cross(Edge2, Point - Tri.V2)) > 0;

        if(IsInsideEdge0 && IsInsideEdge1 && IsInsideEdge2)
        {
            Material M;
            M.type = METAL;
            M.color = v3(1, 1, 1);
            M.fuzz = 0.01f;

            Output.mat = M;
            Output.t = T;
            Output.front = true;
            Output.point = Point;
            Output.normal = Normal;
            return true;
        }
    }

    return false;
}

#if !SPHERES_USE_SSE
static bool
HitWorld(Ray r, World world, HitRecord& hit, f32 t_min, f32 t_max)
{
    HitRecord local_hit;
    bool isHit = false;

    for(u32 i = 0; i < world.count; ++i)
    {
        Sphere s = world.spheres[i];
        if(HitSphere(r.o, r.d, s, local_hit, t_min, t_max))
        {
            isHit = true;
            t_max = local_hit.t;
            hit = local_hit;
        }
    }

    // for(u32 i = 0; i < world.triangle_count; ++i)
    // {
    //     Triangle t = world.triangles[i];
    //     if(HitTriangle(r.o, r.d, t, local_hit, t_min, t_max))
    //     {
    //         isHit = true;
    //         t_max = local_hit.t;
    //         hit = local_hit;
    //     }
    // }

    return isHit;
}
#else
static bool
HitWorld(Ray r, World world, HitRecord& hit, f32 t_min, f32 t_max)
{
    HitRecord local_hit;
    bool isHit = false;
    v3 ro = r.o;
    v3 rd = r.d;

    __m128 RayX = _mm_set_ps1(ro.x);
    __m128 RayY = _mm_set_ps1(ro.y);
    __m128 RayZ = _mm_set_ps1(ro.z);
    __m128 RayDX = _mm_set_ps1(rd.x);
    __m128 RayDY = _mm_set_ps1(rd.y);
    __m128 RayDZ = _mm_set_ps1(rd.z);
    __m128 A = _mm_set_ps1(SqrLength(rd));
    __m128 TMin = _mm_set_ps1(t_min);
    __m128 TMax = _mm_set_ps1(t_max);
    __m128 HitT = _mm_set_ps1(t_max);
    __m128i SphereIDs = {};

    // TODO: Add spheres with 0 radius instead of removing
    world.count = world.count - (world.count % 4);
    
    for(u32 I = 0; I < world.count; I += 4)
    {
        __m128 SpheresX = _mm_set_ps(world.spheres[I + 0].center.x,
                                     world.spheres[I + 1].center.x,
                                     world.spheres[I + 2].center.x,
                                     world.spheres[I + 3].center.x);
        __m128 SpheresY = _mm_set_ps(world.spheres[I + 0].center.y,
                                     world.spheres[I + 1].center.y,
                                     world.spheres[I + 2].center.y,
                                     world.spheres[I + 3].center.y);
        __m128 SpheresZ = _mm_set_ps(world.spheres[I + 0].center.z,
                                     world.spheres[I + 1].center.z,
                                     world.spheres[I + 2].center.z,
                                     world.spheres[I + 3].center.z);

        __m128 SpheresRad = _mm_set_ps(world.spheres[I + 0].radius,
                                       world.spheres[I + 1].radius,
                                       world.spheres[I + 2].radius,
                                       world.spheres[I + 3].radius);

        __m128i SpheresID = _mm_set_epi32(I + 0, I + 1, I + 2, I + 3);

        __m128 COX = _mm_sub_ps(SpheresX, RayX);
        __m128 COY = _mm_sub_ps(SpheresY, RayY);
        __m128 COZ = _mm_sub_ps(SpheresZ, RayZ);
        
        __m128 BMulX = _mm_mul_ps(COX, RayDX);
        __m128 BMulY = _mm_mul_ps(COY, RayDY);
        __m128 BMulZ = _mm_mul_ps(COZ, RayDZ);
        __m128 NB = _mm_add_ps(BMulX, _mm_add_ps(BMulY, BMulZ));

        __m128 SqrLenCO = _mm_add_ps(_mm_mul_ps(COX, COX),
                                     _mm_add_ps(_mm_mul_ps(COY, COY),
                                                _mm_mul_ps(COZ, COZ)));

        __m128 C = _mm_sub_ps(SqrLenCO, _mm_mul_ps(SpheresRad, SpheresRad));

        __m128 Discriminant = _mm_sub_ps(_mm_mul_ps(NB,NB),
                                         _mm_mul_ps(A,C));

        __m128 DiscMask = _mm_cmpgt_ps(Discriminant, _mm_set_ps1(0.0f));

        if(_mm_movemask_ps(DiscMask))
        {
            __m128 SqrtDiscriminant = _mm_sqrt_ps(Discriminant);
            __m128 T1 = _mm_div_ps(_mm_sub_ps(NB, SqrtDiscriminant), A);
            __m128 T2 = _mm_div_ps(_mm_add_ps(NB, SqrtDiscriminant), A);
            __m128 T1Mask = _mm_cmpgt_ps(T1, TMin);
            __m128 T = _mm_blendv_ps(T2, T1, T1Mask); // Select T1 if T1Mask is set otherwise T2

            // DiscMask && T > TMin && T < HitT
            __m128 Mask = _mm_and_ps(DiscMask, _mm_and_ps(_mm_cmpgt_ps(T, TMin), (_mm_cmplt_ps(T, HitT))));
            
            HitT = _mm_blendv_ps(HitT, T, Mask);
            SphereIDs = _mm_blendv_epi8(SphereIDs, SpheresID, _mm_castps_si128(Mask));
        }
    }

    f32 Min = M128Min(HitT);
    if(Min < *(f32*)&TMax)
    {
        int MinMask = _mm_movemask_ps(_mm_cmpeq_ps(HitT, _mm_set1_ps(Min)));
        
        static const int LaneId[16] =
        {
            0, 0, 1, 0,
            2, 0, 1, 0,
            3, 0, 1, 0,
            2, 0, 1, 0,
        };

        i32 ID = LaneId[MinMask];
        i32 SphereID = ((i32*)&SphereIDs)[ID];
           
        local_hit.t = Min;
        local_hit.point = ro + rd * local_hit.t;
        v3 out_normal = (local_hit.point - world.spheres[SphereID].center) / world.spheres[SphereID].radius;
        local_hit.SetFaceNormal(rd, out_normal);
        local_hit.mat = world.spheres[SphereID].mat;

        isHit = true;
        hit = local_hit;
    }
    
#if 0
    for(u32 i = 0; i < world.triangle_count; ++i)
    {
        Triangle t = world.triangles[i];
        if(HitTriangle(r.o, r.d, t, local_hit, t_min, t_max))
        {
            isHit = true;
            t_max = local_hit.t;
            hit = local_hit;
        }
    }
#endif
    return isHit;
}
#endif
static v3
RayColor(Ray r, World world, u32 depth)
{
    HitRecord hit;

    if(depth <= 0) return v3(0,0,0);
    
    if(HitWorld(r, world, hit, 0.0005f, 100))
    {
        Ray ray;
        ray.o = hit.point;

        if(hit.mat.type == LAMBERIAN)
        { 
            v3 target = hit.point + RandomInHemiSphere(hit.normal);
            ray.d = target - hit.point;
            return hit.mat.color * RayColor(ray, world, depth - 1);
        }
        else if (hit.mat.type == METAL)
        {
            ray.d = Normalize(Reflect(r.d, hit.normal));
            ray.d = ray.d + hit.mat.fuzz * RandomInUnitSphere();

            if(Dot(ray.d, hit.normal) > 0.0f)
            {
                return hit.mat.color * RayColor(ray, world, depth - 1);
            }
            else return v3(0,0,0);
        }
        else if(hit.mat.type == DIELECTRIC)
        {
            f32 ir = 1.5f;
            f32 refraction_ratio = hit.front ? (1.0f/ir) : ir;
            v3 unit_dir = Normalize(r.d);
            f32 cos_theta = Min(Dot(unit_dir * -1, hit.normal), 1.0f);
            f32 sin_theta = Sqrt(1.0f - cos_theta*cos_theta);

            bool cannot_reflect = (refraction_ratio * sin_theta) > 1.0f;

            if(cannot_reflect || Reflectance(cos_theta, refraction_ratio) > Rand01())
            {
                ray.d = Reflect(unit_dir, hit.normal);
            }
            else
            {
                ray.d = Refract(unit_dir, hit.normal, refraction_ratio);
            }

            return RayColor(ray, world, depth - 1);
        }
    }

    float t1 = 0.5f*(r.d.y + 1.0f);
    return v3((1.0f-t1)*v3(1.0f, 1.0f, 1.0f) + t1*v3(0.5f, 0.7f, 1.0f));
}
