#pragma once

struct Ray {
    v3 o;
    v3 d;
};

struct HitRecord {
    v3 point;
    v3 normal;
    f32 t;
    bool front;

    inline void SetFaceNormal(v3 ray_dir, v3 out_normal) {
        front  = Dot(ray_dir, out_normal) < 0;
        normal = front ? out_normal : out_normal * -1; 
    }
};

struct Sphere {
    v3 center;
    f32 radius;
};

struct World {
    Sphere* spheres;
    u32 count;
};

bool HitSphere(v3 ro, v3 rd, Sphere s, HitRecord& hit, f32 t_min, f32 t_max) {
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
    return true;
}

bool HitWorld(Ray r, World world, HitRecord& hit, f32 t_min, f32 t_max) {
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

v3 RayColor(Ray r, World world, u32 depth) {
    HitRecord hit;

    if(depth <= 0) return v3(0,0,0);
    
    if(HitWorld(r, world, hit, 0, 100)) {
        Ray ray;
        v3 target = hit.point + hit.normal + RandomOnUnitSphere();
        ray.o = hit.point;
        ray.d = target - hit.point;

        //return (hit.normal + 1) * 0.5f;
        
        return 0.5f * RayColor(ray, world, depth - 1);
    }

    float t1 = 0.5f*(r.d.y + 1.0f);
    return v3((1.0f-t1)*v3(1.0f, 1.0f, 1.0f) + t1*v3(0.5f, 0.7f, 1.0f));
}
