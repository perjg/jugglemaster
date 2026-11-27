// Renders three JuggleMaster 3D balls (the engine's own mesh, shader and lighting) to a transparent PNG.
#include "jugglesaver/render_shader.cpp"

#include <OpenGL/OpenGL.h>
#include <OpenGL/glext.h>
#include <ApplicationServices/ApplicationServices.h>
#include <string>
#include <vector>
#include <cstdlib>

struct Ball { float x, y, z, rot; float c[3]; float a[3]; };

static void writePNG(const char* path, int w, int h, const std::vector<unsigned char>& rgba) {
    CGColorSpaceRef cs = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
    CGDataProviderRef dp = CGDataProviderCreateWithData(nullptr, rgba.data(), rgba.size(), nullptr);
    CGImageRef img = CGImageCreate(w, h, 8, 32, w * 4, cs, kCGImageAlphaPremultipliedLast, dp, nullptr, false,
                                   kCGRenderingIntentDefault);
    CFURLRef url = CFURLCreateFromFileSystemRepresentation(nullptr, (const UInt8*)path, strlen(path), false);
    CGImageDestinationRef dst = CGImageDestinationCreateWithURL(url, CFSTR("public.png"), 1, nullptr);
    CGImageDestinationAddImage(dst, img, nullptr);
    CGImageDestinationFinalize(dst);
    CFRelease(dst); CFRelease(url); CGImageRelease(img); CGDataProviderRelease(dp); CGColorSpaceRelease(cs);
}

// usage: iconrender out.png width height fovy [x y z rot r g b ar ag ab]...
int main(int argc, char** argv) {
    const char* out = argv[1];
    const int W = atoi(argv[2]), H = atoi(argv[3]);
    const float fovy = atof(argv[4]);
    const int ss = getenv("JM_SUPERSAMPLE") ? atoi(getenv("JM_SUPERSAMPLE")) : 4;
    const int BW = W * ss, BH = H * ss;
    std::vector<Ball> balls;
    for (int i = 5; i + 9 < argc; i += 10) {
        Ball b;
        b.x = atof(argv[i]); b.y = atof(argv[i + 1]); b.z = atof(argv[i + 2]); b.rot = atof(argv[i + 3]);
        for (int k = 0; k < 3; k++) { b.c[k] = atof(argv[i + 4 + k]); b.a[k] = atof(argv[i + 7 + k]); }
        balls.push_back(b);
    }

    CGLPixelFormatAttribute attrs[] = {kCGLPFAOpenGLProfile, (CGLPixelFormatAttribute)kCGLOGLPVersion_Legacy,
                                       kCGLPFAAccelerated, (CGLPixelFormatAttribute)0};
    CGLPixelFormatObj pf; GLint npf;
    CGLChoosePixelFormat(attrs, &pf, &npf);
    CGLContextObj ctx; CGLCreateContext(pf, nullptr, &ctx); CGLSetCurrentContext(ctx);

    GLuint fbo, color, depth;
    glGenFramebuffersEXT(1, &fbo); glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    glGenRenderbuffersEXT(1, &color); glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, color);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, BW, BH);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, color);
    glGenRenderbuffersEXT(1, &depth); glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, BW, BH);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth);

    RENDER_STATE state;
    InitGLSettings(&state, 0);
    glViewport(0, 0, BW, BH);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gProj = perspective(fovy, (float)W / H, 0.1f, 20.0f);
    gView = lookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    gPhong->use();
    GLint posLoc = gPhong->getAttribLocation("aPosition");
    GLint nrmLoc = gPhong->getAttribLocation("aNormal");
    const LightingConfig jmLighting = {{0.0f, 10.0f, 10.0f}, {0.0f, 0.0f, 10.0f}, {1.0f, 1.0f, 1.0f}, 0.5f, 0.6f, 60.0f};

    for (const Ball& b : balls) {
        drawBallES(b.x, b.y, b.z, b.rot, b.c[0], b.c[1], b.c[2], b.a[0], b.a[1], b.a[2],
                   [&](float tx, float ty, float tz, float r1, float rx1, float ry1, float rz1,
                       float r2, float rx2, float ry2, float rz2, float sx, float sy, float sz,
                       float cr, float cg, float cb) {
                       drawObjectWithTransformES(gSphereVBO, gSphereIBO, gSphereIndexCount, tx, ty, tz,
                                                 r1, rx1, ry1, rz1, r2, rx2, ry2, rz2, sx, sy, sz,
                                                 cr, cg, cb, jmLighting, posLoc, nrmLoc, false);
                   });
    }
    glFinish();

    std::vector<unsigned char> px(BW * BH * 4);
    glReadPixels(0, 0, BW, BH, GL_RGBA, GL_UNSIGNED_BYTE, px.data());
    std::vector<unsigned char> res(W * H * 4);
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++) {
            float acc[4] = {0, 0, 0, 0};
            for (int j = 0; j < ss; j++)
                for (int i = 0; i < ss; i++) {
                    // GL rows are bottom-up
                    const unsigned char* p = &px[((BH - 1 - (y * ss + j)) * BW + x * ss + i) * 4];
                    float a = p[3] / 255.0f;
                    for (int k = 0; k < 3; k++) acc[k] += p[k] * a;
                    acc[3] += p[3];
                }
            unsigned char* d = &res[(y * W + x) * 4];
            for (int k = 0; k < 4; k++) d[k] = (unsigned char)(acc[k] / (ss * ss) + 0.5f);
        }
    writePNG(out, W, H, res);
    return 0;
}
