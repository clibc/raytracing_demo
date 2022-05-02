#include "headers.h"

#define IMAGE_WIDTH 400
#define IMAGE_HEIGHT 250

static inline void
WriteColor01(v3 color) {
    u8 r,g,b;

    //color.x = Clamp(color.x, 0, 1);
    //color.y = Clamp(color.y, 0, 1);
    //color.z = Clamp(color.z, 0, 1);

    r = (u8)(color.x * 255);
    g = (u8)(color.y * 255);
    b = (u8)(color.z * 255);
    fprintf(stdout, "%i %i %i\n", r, g, b);
}

s32 main() {
    fprintf(stdout, "P3\n");

    v3 ro = v3(0,0,0);
    Sphere spheres[2];
    spheres[0].center = v3(0, 0, -1.2f);
    spheres[0].radius = 0.5f;
    spheres[1].center = v3(0, -100.5f, -10);
    spheres[1].radius = 100.0f;

    World world;
    world.spheres = spheres;
    world.count   = sizeof(spheres) / sizeof(Sphere);

    u32 samplePP = 1000;
    u32 depth    = 100;

    fprintf(stdout, "%i %i\n%i\n", IMAGE_WIDTH, IMAGE_HEIGHT, 255);
    
    f32 aspect = (float)IMAGE_WIDTH / (float)IMAGE_HEIGHT;
    for(s32 y = 0; y < IMAGE_HEIGHT; ++y) {
        DebugLog("\rRendering %f", ((float)y/IMAGE_HEIGHT) * 100.0f);
        for(s32 x = 0; x < IMAGE_WIDTH; ++x) {
            v3 color = v3(0,0,0);
            for(u32 i = 0; i < samplePP; ++i) {
                f32 randx = Rand01() * 2 - 1;
                f32 randy = Rand01() * 2 - 1;
                f32 tx = ((float)x + 0.5f + randx) / (float)(IMAGE_WIDTH - 1);
                f32 ty = ((float)y + 0.5f + randy) / (float)(IMAGE_HEIGHT - 1);
                v3 uv = v3(tx, ty, 0);
                uv.y = 1 - uv.y;
                uv.x = (1 - uv.x * 2) * aspect;
                uv.y = 1 - uv.y * 2;
                uv.z = 1.0f;
                
                Ray r;
                r.o = ro;
                r.d = Normalize(ro - uv);
                color += RayColor(r, world, depth);
            }

            color /= samplePP;
            
            color.x = Sqrt(color.x);
            color.y = Sqrt(color.y);
            color.z = Sqrt(color.z);
            
            WriteColor01(color);
        }
    }
    
    return 0;
}
