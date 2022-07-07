#include "headers.h"

//#define IMAGE_WIDTH 300
//#define IMAGE_HEIGHT 200
#define IMAGE_WIDTH 1200
#define IMAGE_HEIGHT 800

static void RandomScene(World&);

static inline void
WriteColor01(v3 color) {
    u8 r,g,b;

    color.x = Clamp(color.x, 0.0f, 1.0f);
    color.y = Clamp(color.y, 0.0f, 1.0f);
    color.z = Clamp(color.z, 0.0f, 1.0f);

    r = (u8)(color.x * 255);
    g = (u8)(color.y * 255);
    b = (u8)(color.z * 255);
    fprintf(stdout, "%i %i %i\n", r, g, b);
}

s32 main() {
    fprintf(stdout, "P3\n");

    World world;
    RandomScene(world);
    Triangle Triangles[2];
    world.triangle_count = 2;
    world.triangles = Triangles;

    Triangles[0].V0 = v3(-1,0,1);
    Triangles[0].V1 = v3(1,0, 1);
    Triangles[0].V2 = v3(-1,2, 1);

    Triangles[1].V0 = v3(1,0,1);
    Triangles[1].V1 = v3(1,2, 1);
    Triangles[1].V2 = v3(-1,2,1);

    v3 TriPos = v3(2, 0, 1);

    Triangles[0].V0 += TriPos;
    Triangles[0].V1 += TriPos;
    Triangles[0].V2 += TriPos;

    Triangles[1].V0 += TriPos;
    Triangles[1].V1 += TriPos;
    Triangles[1].V2 += TriPos;
    
    fprintf(stdout, "%i %i\n%i\n", IMAGE_WIDTH, IMAGE_HEIGHT, 255);

    //Camera
    f32 fov = 20;    
    v3 CamLookAt = v3(0,0,0);
    v3 CamPos    = v3(0,1,17);
    v3 CamZ      = Normalize(CamLookAt - CamPos);
    v3 CamX      = Normalize(Cross(CamZ, v3(0,1,0)));
    v3 CamY      = Normalize(Cross(CamX, CamZ));
    m4 CamToWorld = {};

    f32 aspect = (float)IMAGE_WIDTH / (float)IMAGE_HEIGHT;
    f32 theta = DegToRad(fov);
    f32 h = Tan(theta/2);
    f32 viewport_y = h * 2.0f;
    f32 viewport_x = viewport_y * aspect;

    // divide by 2 cuz range is (-1 1)
    CamX *= viewport_x/2;
    CamY *= viewport_y/2;
    
    CamToWorld.SetRow(0, CamX.x, CamX.y, CamX.z, 0);
    CamToWorld.SetRow(1, CamY.x, CamY.y, CamY.z, 0);
    CamToWorld.SetRow(2, CamZ.x, CamZ.y, CamZ.z, 0);
    CamToWorld.SetRow(3, CamPos.x, CamPos.y, CamPos.z, 1);
    //
    
    f32 FocalLength = Length(CamPos - v3(4, 1, 0));
    f32 Aperture = 0.1;
    
    u32 SamplePP = 50;
    u32 Depth    = 10;

    for(s32 y = 0; y < IMAGE_HEIGHT; ++y) {
        DebugLog("\rRendering %f", ((float)y/IMAGE_HEIGHT) * 100.0f);
        for(s32 x = 0; x < IMAGE_WIDTH; ++x) {
            v3 color = v3(0,0,0);
            for(u32 i = 0; i < SamplePP; ++i) {
                f32 randx = Rand01() * 2 - 1;
                f32 randy = Rand01() * 2 - 1;
                f32 tx = ((float)x + 0.5f + randx) / (float)(IMAGE_WIDTH - 1);
                f32 ty = ((float)y + 0.5f + randy) / (float)(IMAGE_HEIGHT - 1);
                v3 uv = v3(tx, ty, -1);

                uv.y = 1 - uv.y;
                uv.x = (1 - uv.x * 2);
                uv.y = 1 - uv.y * 2;

                v4 WorldUV = CamToWorld * v4(uv, 1.0f);
                uv.x = WorldUV.x;
                uv.y = WorldUV.y;
                uv.z = WorldUV.z;

                Ray r;
                r.d = Normalize(CamPos - uv);

                v3 FocalPoint = CamPos + r.d * FocalLength;
                v3 RandomPoint = RandomInUnitDisk() * Aperture;
                r.o = CamPos + RandomPoint.x * CamX + RandomPoint.y * CamY;
                r.d = Normalize(FocalPoint - r.o);
                
                color += RayColor(r, world, Depth);
            }

            color /= SamplePP;
            
            color.x = Sqrt(color.x);
            color.y = Sqrt(color.y);
            color.z = Sqrt(color.z);
            
            WriteColor01(color);
        }
    }
    
    return 0;
}

static void
RandomScene(World& world) {
    u32 MaxSphereCount = 1000;
    world.spheres = (Sphere*)malloc(sizeof(Sphere) * MaxSphereCount);
    Sphere* spheres = world.spheres;
    
    // ground
    spheres[0].center = v3(0, -1000, 0);
    spheres[0].radius = 1000.0f;
    spheres[0].mat.type  = LAMBERIAN;
    spheres[0].mat.color = v3(0.8, 0.8, 0.0);

    // middle
    spheres[2].center = v3(0, 1, 0);
    spheres[2].radius = 1;
    spheres[2].mat.type  = DIELECTRIC;
    spheres[2].mat.color = v3(0.8, 0.8, 0.8);

    spheres[3].center = v3(0, 1, 0);
    spheres[3].radius = -0.95;
    spheres[3].mat.type  = DIELECTRIC;
    spheres[3].mat.color = v3(0.8, 0.8, 0.8);

    // left
    spheres[1].center = v3(-4, 1, 0);
    spheres[1].radius = 1;
    spheres[1].mat.type  = LAMBERIAN;
    spheres[1].mat.color = v3(0.1, 0.2, 0.5);
    
    // right
    spheres[4].center = v3(4, 1, 0);
    spheres[4].radius = 1;
    spheres[4].mat.type  = METAL;
    spheres[4].mat.color = v3(0.8, 0.6, 0.2);
    spheres[4].mat.fuzz  = 0.01;

    world.count = 5;

    u32 Index = 5;
#if 0
    for(s32 a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; b++) {
            if(Index >= MaxSphereCount) break;
            f32 x = (float)a;
            f32 z = (float)b;
            f32 RandomMat = Rand01();
            v3  Pos       = v3(x + Rand01() * 0.9, 0.2, z + Rand01() * 0.9);
            v3  RandomCol = v3(Rand01(), Rand01(), Rand01());
            f32 Radius    = 0.2;
            
            if((Pos - Length(v3(4,0,0)) > 0.9) {
                if(RandomMat < 0.8) { // diffuse
                    spheres[Index].center = Pos;
                    spheres[Index].radius = Radius;
                    spheres[Index].mat.type  = LAMBERIAN;
                    spheres[Index].mat.color = RandomCol;
                }
                else if(RandomMat < 0.95) { // metal
                    spheres[Index].center = Pos;
                    spheres[Index].radius = Radius;
                    spheres[Index].mat.type  = METAL;
                    spheres[Index].mat.color = RandomCol;
                    spheres[Index].mat.fuzz  = Rand01() * 0.5;
                }
                else { // glass
                    spheres[Index].center = Pos;
                    spheres[Index].radius = Radius;
                    spheres[Index].mat.type  = DIELECTRIC;
                    spheres[Index].mat.color = v3(0.8, 0.8, 0.8);
                }
                ++Index;
            }
        }
    }
#endif    
    world.count = Index;
}
