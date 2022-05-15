#pragma once

enum MaterialType {
    LAMBERIAN,
    METAL,
    DIELECTRIC
};

struct Material {
    MaterialType type;
    v3 color;
    f32 fuzz;
};

struct Ray {
    v3 o;
    v3 d;
};

struct HitRecord {
    v3 point;
    v3 normal;
    f32 t;
    bool front;
    Material mat;

    inline void SetFaceNormal(v3 ray_dir, v3 out_normal) {
        front  = Dot(ray_dir, out_normal) < 0;
        normal = front ? out_normal : out_normal * -1; 
    }
};

struct Sphere {
    v3 center;
    f32 radius;
    Material mat;
};

struct World {
    Sphere* spheres;
    u32 count;
};

static f32
Reflectance(f32 cosine, f32 ref_idx) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*PowerF32((1 - cosine),5);
}

static bool
HitSphere(v3 ro, v3 rd, Sphere s, HitRecord& hit, f32 t_min, f32 t_max) {
    v3 oc = ro - s.center;
    f32 a = rd.SqrLength();
    f32 b = Dot(oc, rd);
    f32 c = oc.SqrLength() - s.radius * s.radius;
    f32 discriminant = b*b - a*c;
    
    if(discriminant < 0) return false;

    f32 sqrtd = Sqrt(discriminant);
    f32 root = (-b - sqrtd) / a;
    if (root < t_min || t_max < root) {
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

static bool
HitWorld(Ray r, World world, HitRecord& hit, f32 t_min, f32 t_max) {
    HitRecord local_hit;
    bool isHit = false;
    f32 closest = t_max;

    for(u32 i = 0; i < world.count; ++i) {
        Sphere s = world.spheres[i];
        if(HitSphere(r.o, r.d, s, local_hit, t_min, closest)) {
            isHit = true;
            closest = local_hit.t;
            hit = local_hit;
        }
    }

    return isHit;
}

static v3
RayColor(Ray r, World world, u32 depth) {
    HitRecord hit;

    if(depth <= 0) return v3(0,0,0);
    
    if(HitWorld(r, world, hit, 0.0005f, 100)) {
        Ray ray;
        ray.o = hit.point;

        if(hit.mat.type == LAMBERIAN) { 
            v3 target = hit.point + RandomInHemiSphere(hit.normal);
            ray.d = target - hit.point;
            return hit.mat.color * RayColor(ray, world, depth - 1);
        }
        else if (hit.mat.type == METAL) {
            ray.d = Normalize(Reflect(r.d, hit.normal));
            ray.d = ray.d + hit.mat.fuzz * RandomInUnitSphere();

            if(Dot(ray.d, hit.normal) > 0.0f)
                return hit.mat.color * RayColor(ray, world, depth - 1);
            else return v3(0,0,0);
        }
        else if(hit.mat.type == DIELECTRIC) {
            f32 ir = 1.5f;
            f32 refraction_ratio = hit.front ? (1.0f/ir) : ir;
            v3 unit_dir = Normalize(r.d);
            f32 cos_theta = Min(Dot(unit_dir * -1, hit.normal), 1.0f);
            f32 sin_theta = Sqrt(1.0f - cos_theta*cos_theta);

            bool cannot_reflect = (refraction_ratio * sin_theta) > 1.0f;

            if(cannot_reflect || Reflectance(cos_theta, refraction_ratio) > Rand01()) {
                ray.d = Reflect(unit_dir, hit.normal);
            }
            else {
                ray.d = Refract(unit_dir, hit.normal, refraction_ratio);
            }

            //r.d = Normalize(r.d);
            return RayColor(ray, world, depth - 1);
        }
    }

    float t1 = 0.5f*(r.d.y + 1.0f);
    return v3((1.0f-t1)*v3(1.0f, 1.0f, 1.0f) + t1*v3(0.5f, 0.7f, 1.0f));
}
