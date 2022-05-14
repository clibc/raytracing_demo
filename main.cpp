#include "headers.h"

#define IMAGE_WIDTH 1024
#define IMAGE_HEIGHT 576

static inline void
WriteColor01(v3 color) {
    u8 r,g,b;

    color.x = Clamp(color.x, 0, 1);
    color.y = Clamp(color.y, 0, 1);
    color.z = Clamp(color.z, 0, 1);

    r = (u8)(color.x * 255);
    g = (u8)(color.y * 255);
    b = (u8)(color.z * 255);
    fprintf(stdout, "%i %i %i\n", r, g, b);
}

s32 main() {
    fprintf(stdout, "P3\n");

    Sphere spheres[6];
    spheres[0].center = v3(-1.0, 0.0, -1.0);
    spheres[0].radius = 0.5f;
    spheres[0].mat.type  = DIELECTRIC;
    spheres[0].mat.color = v3(0.8, 0.8, 0.8);

    spheres[1].center = v3(-1.0, 0.0, -1.0);
    spheres[1].radius = -0.45f;
    spheres[1].mat.type  = DIELECTRIC;
    spheres[1].mat.color = v3(0.8, 0.8, 0.8);
    
    spheres[2].center = v3(0, -100.5, -1.0);
    spheres[2].radius = 100.0f;
    spheres[2].mat.type  = LAMBERIAN;
    spheres[2].mat.color = v3(0.8, 0.8, 0.0);
    
    spheres[3].center = v3(0.0, 0.0, -1.0);
    spheres[3].radius = 0.5f;
    spheres[3].mat.type  = LAMBERIAN;
    spheres[3].mat.color = v3(0.1, 0.2, 0.5);

    spheres[5].center = v3(1.0, 0.0, -1.0);
    spheres[5].radius = 0.5f;
    spheres[5].mat.type  = METAL;
    spheres[5].mat.color = v3(0.8, 0.6, 0.2);
    spheres[5].mat.fuzz  = 0.01;
    

    World world;
    world.spheres = spheres;
    world.count   = sizeof(spheres) / sizeof(Sphere);
    u32 samplePP = 15;
    u32 depth    = 50;
    
    fprintf(stdout, "%i %i\n%i\n", IMAGE_WIDTH, IMAGE_HEIGHT, 255);

    //Camera
    v3 CamLookAt = v3(0,0,-1);
    v3 CamPos    = v3(-2,2,1);
    v3 CamZ      = Normalize(CamLookAt - CamPos);
    v3 CamX      = Normalize(Cross(CamZ, v3(0,1,0)));
    v3 CamY      = Normalize(Cross(CamX, CamZ));
    CamX.Print();
    CamY.Print();
    CamZ.Print();
    m4 CamToWorld = {};
    CamToWorld.SetRow(0, CamX.x, CamX.y, CamX.z, 0);
    CamToWorld.SetRow(1, CamY.x, CamY.y, CamY.z, 0);
    CamToWorld.SetRow(2, CamZ.x, CamZ.y, CamZ.z, 0);
    CamToWorld.SetRow(3, CamPos.x, CamPos.y, CamPos.z, 1);
    CamToWorld.Print();
    //

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
                v3 uv = v3(tx, ty, -1);

                uv.y = 1 - uv.y;
                uv.x = (1 - uv.x * 2) * aspect;
                uv.y = 1 - uv.y * 2;

                v4 WorldUV = CamToWorld * v4(uv, 1.0f);
                uv.x = WorldUV.x;
                uv.y = WorldUV.y;
                uv.z = WorldUV.z;

                Ray r;
                r.o = CamPos;
                r.d = Normalize(r.o - uv);
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
