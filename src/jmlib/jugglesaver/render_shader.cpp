/* Juggler3D, Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
 *
 * JMLib - Portable JuggleMaster Library
 * Version 3.0
 * Copyright (c) Per Johan Groland 2025-2026
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.  No
 * representations are made about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * Changes for jmlib and OpenGL ES compatibility
 * Per Johan Groland, (C) 2008
 */

#include "jugglesaver.h"
#include "render_common.h"
#include <functional>

// Rendering paths:
// - When USE_SHADER_RENDERING is defined (WebAssembly/WebGL 2.0 or desktop shaders),
//   use the shader/VBO implementation.
// - Otherwise (desktop OpenGL fixed pipeline), use the classic fixed‑function implementation.

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *
 * Rendering
 *
 *****************************************************************************/

// Camera state variables - shared between legacy and ES paths
static float deltaX = 0.0f;
static float deltaY = 0.0f;
static float extraZoom = 0.0f;
static float resetZoom = 0.0f;
static float cameraHeightAdjustment = 0.0f;

static int adjustCameraHeight = 1;
static float gEyePos[3] = {0.0f, 0.0f, 10.0f}; // Camera position for shader

// ES path utility: cosine rule (degrees), mirrored from legacy
static double CosineRuleES(double a, double b, double c) {
    double cosang = (a * a + b * b - c * c) / (2 * a * b);
    if (cosang < -1.0 || cosang > 1.0)
        return 0.0;
    return 180.0 * acos(cosang) / PI;
}

#ifdef __cplusplus
}
#endif

#include "shader_manager.h"
#include "shaders.h"
#include "matrix_stack.h"
#include <vector>
#include <stdio.h>
#include <math.h>

static ShaderProgram* gPhong = nullptr;
static GLuint gSphereVBO = 0;
static GLuint gSphereIBO = 0;
static GLsizei gSphereIndexCount = 0;

static GLuint gRingVBO = 0;   // legacy-style washer ring
static GLuint gRingIBO = 0;
static GLsizei gRingIndexCount = 0;
static GLuint gCylVBO = 0;    // unit cylinder along +Z, radius=1, height=1
static GLuint gCylIBO = 0;
static GLsizei gCylIndexCount = 0;
static GLuint gFrustumVBO = 0; // unit frustum: base radius=1, top radius=0.5, height=1
static GLuint gFrustumIBO = 0;
static GLsizei gFrustumIndexCount = 0;
static GLuint gDiskVBO = 0;    // unit disk in XY plane (radius=1 at z=0)
static GLuint gDiskIBO = 0;
static GLsizei gDiskIndexCount = 0;
static GLuint gClubVBO = 0;
static GLuint gClubIBOBodyEven = 0; // stripes with object color
static GLuint gClubIBOBodyOdd  = 0; // stripes with silver/handle color
static GLuint gClubIBOHandle   = 0; // tapered handle + end caps (silver)

static GLuint gFishVBO = 0;        // "Radical Fish" club lathe
static GLuint gFishIBOHandle = 0;  // knob + long thin handle (white)
static GLuint gFishIBOBody   = 0;  // cone-bulge-cone body + base cap (object color)
static GLsizei gFishHandleIndexCount = 0;
static GLsizei gFishBodyIndexCount = 0;
static GLsizei gClubIndexCountBodyEven = 0;
static GLsizei gClubIndexCountBodyOdd  = 0;
static GLsizei gClubIndexCountHandle   = 0;
static Matrix4 gProj;
static Matrix4 gView;
static float gFovDeg = 70.0f;
static bool gNoSpecular = false;
static bool gSolidColor = false;
static float gAspect = 4.0f/3.0f;

// Lighting configuration structure
struct LightingConfig {
    float lightPos[3];
    float viewPos[3];
    float lightColor[3];
    float ambient;
    float specularStrength;
    float shininess;
};

static void bindVertexAttributesAndDraw(GLuint vbo, GLuint ibo, GLsizei indexCount,
                                        GLint posLoc, GLint nrmLoc) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    if (posLoc >= 0) {
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)0);
        glEnableVertexAttribArray(posLoc);
    }
    if (nrmLoc >= 0) {
        glVertexAttribPointer(nrmLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(sizeof(float)*3));
        glEnableVertexAttribArray(nrmLoc);
    }
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

static void unbindVertexAttributes(GLint posLoc, GLint nrmLoc) {
    if (posLoc >= 0) glDisableVertexAttribArray(posLoc);
    if (nrmLoc >= 0) glDisableVertexAttribArray(nrmLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Handles luminance scaling and alternate color rendering
static void drawBallES(float x, float y, float z, float rotation,
                       float primaryR, float primaryG, float primaryB,
                       float alternateR, float alternateG, float alternateB,
                       std::function<void(float, float, float,
                                         float, float, float, float,
                                         float, float, float, float,
                                         float, float, float,
                                         float, float, float)> drawFunc) {
    const float targetLum = 0.75f;

    float r1 = primaryR;
    float g1 = primaryG;
    float b1 = primaryB;
    float lum1 = 0.2126f*r1 + 0.7152f*g1 + 0.0722f*b1;
    float scale1 = lum1 > 1e-4f ? (targetLum / lum1) : 1.0f;
    r1 = min(1.0f, r1*scale1); g1 = min(1.0f, g1*scale1); b1 = min(1.0f, b1*scale1);

    drawFunc(x, y, z,
             rotation, 0.6963f, 0.6963f, 0.1742f,
             0.0f, 0, 0, 0,
             0.34f, 0.34f, 0.34f,
             r1, g1, b1);

    // Second hemisphere: rotate 90° around Y-axis, use alternate color
    float r2 = alternateR;
    float g2 = alternateG;
    float b2 = alternateB;
    float lum2 = 0.2126f*r2 + 0.7152f*g2 + 0.0722f*b2;
    float scale2 = lum2 > 1e-4f ? (targetLum / lum2) : 1.0f;
    r2 = min(1.0f, r2*scale2); g2 = min(1.0f, g2*scale2); b2 = min(1.0f, b2*scale2);

    // Apply rotation first, then 90° Y rotation (cumulative, matching legacy)
    drawFunc(x, y, z,
             rotation, 0.6963f, 0.6963f, 0.1742f,
             90.0f, 0, 1, 0,
             0.34f, 0.34f, 0.34f,
             r2, g2, b2);
}

// Draws three parts: body even stripes, body odd stripes, and handle
static void drawClubES(float x, float y, float z, float rotation, float elevation,
                       float primaryR, float primaryG, float primaryB,
                       float handleR, float handleG, float handleB,
                       std::function<void(float, float, float,
                                         float, float, float, float,
                                         float, float, float, float,
                                         float, float, float,
                                         float, float, float,
                                         GLuint, GLuint, GLsizei)> drawFunc) {
    drawFunc(x, y, z,
             rotation, 0, 1, 0,
             elevation, -1, 0, 0,
             1.0f, 1.0f, 1.0f,
             primaryR, primaryG, primaryB,
             gClubVBO, gClubIBOBodyEven, gClubIndexCountBodyEven);

    drawFunc(x, y, z,
             rotation, 0, 1, 0,
             elevation, -1, 0, 0,
             1.0f, 1.0f, 1.0f,
             handleR, handleG, handleB,
             gClubVBO, gClubIBOBodyOdd, gClubIndexCountBodyOdd);

    drawFunc(x, y, z,
             rotation, 0, 1, 0,
             elevation, -1, 0, 0,
             1.0f, 1.0f, 1.0f,
             handleR, handleG, handleB,
             gClubVBO, gClubIBOHandle, gClubIndexCountHandle);
}

static Matrix4 buildModelMatrix(float tx, float ty, float tz,
                                 float r1, float rx1, float ry1, float rz1,
                                 float r2, float rx2, float ry2, float rz2,
                                 float sx, float sy, float sz) {
    MatrixStack model;
    model.loadIdentity();
    model.translate(tx, ty, tz);
    if (sx != 1.0f || sy != 1.0f || sz != 1.0f) model.scale(sx, sy, sz);
    if (r1 != 0.0f) model.rotate(r1, rx1, ry1, rz1);
    if (r2 != 0.0f) model.rotate(r2, rx2, ry2, rz2);
    return model.top();
}

static void setupShaderUniformsES(const Matrix4& modelMatrix,
                                   const Matrix4& viewMatrix,
                                   const Matrix4& projMatrix,
                                   float colorR, float colorG, float colorB,
                                   const LightingConfig& lighting) {
    Matrix4 mv = viewMatrix * modelMatrix;
    gPhong->setMatrix4("uMVPMatrix", (projMatrix * mv).m);
    gPhong->setMatrix4("uModelViewMatrix", mv.m);
    gPhong->setMatrix4("uNormalMatrix", mv.inverse().transpose().m);

    gPhong->setVec4("uColor", colorR, colorG, colorB, 1.0f);
    gPhong->setVec3("uLightPos", lighting.lightPos[0], lighting.lightPos[1], lighting.lightPos[2]);
    gPhong->setVec3("uViewPos", lighting.viewPos[0], lighting.viewPos[1], lighting.viewPos[2]);
    gPhong->setVec3("uLightColor", lighting.lightColor[0], lighting.lightColor[1], lighting.lightColor[2]);
    gPhong->setFloat("uAmbient", lighting.ambient);
    gPhong->setFloat("uSpecularStrength", lighting.specularStrength);
    gPhong->setFloat("uShininess", lighting.shininess);
}

static void drawObjectWithTransformES(GLuint vbo, GLuint ibo, GLsizei indexCount,
                                       float tx, float ty, float tz,
                                       float r1, float rx1, float ry1, float rz1,
                                       float r2, float rx2, float ry2, float rz2,
                                       float sx, float sy, float sz,
                                       float colorR, float colorG, float colorB,
                                       const LightingConfig& lighting,
                                       GLint posLoc, GLint nrmLoc,
                                       bool unbindAfterDraw) {
    Matrix4 modelMatrix = buildModelMatrix(tx, ty, tz, r1, rx1, ry1, rz1, r2, rx2, ry2, rz2, sx, sy, sz);
    setupShaderUniformsES(modelMatrix, gView, gProj, colorR, colorG, colorB, lighting);
    bindVertexAttributesAndDraw(vbo, ibo, indexCount, posLoc, nrmLoc);
    if (unbindAfterDraw) {
        unbindVertexAttributes(posLoc, nrmLoc);
    }
}

// "Radical Fish" club: small knob on a long thin handle (~45% of length),
// then a slender cone swelling into a low ball-like bulge and tapering to a
// flat base cap. Lathe over the profile below; local Z matches the club's
// baked range so club motion carries over. Two index groups: handle (white)
// and body (per-object color).
static void buildFishMesh(int radialSeg) {
    if (gFishVBO && (gFishIBOHandle || gFishIBOBody)) return;
    const int n = (radialSeg < 18 ? 18 : radialSeg);

    // (radius, z) profile, knob at low z; -1.0 baked like the club mesh.
    // Proportions taken from reference photos: handle ends ~47% down, bulge
    // apex ~76%, widest radius ~0.30.
    // Measured from the reference photos (pixel silhouette of both):
    // handle+knob 48-51% of total length; body symmetric about its center —
    // cap, linear cone to 0.70x max width at 40%, ball bulge peaking at the
    // body's midpoint (49% in both photos), mirrored cone, cap. Max diameter
    // 0.16x total length.
    const float prof[][2] = {
        {0.001f, -0.45f}, {0.09f, -0.38f}, {0.062f, -0.29f},   // knob
        {0.058f, -0.27f}, {0.058f, 0.74f},                     // handle
        {0.07f, 0.78f},                                        // body cap
        {0.105f, 1.05f}, {0.145f, 1.31f},                      // cone (linear)
        {0.24f, 1.37f}, {0.267f, 1.44f}, {0.24f, 1.51f},        // ball, centered (+30% per feedback)
        {0.145f, 1.57f}, {0.105f, 1.82f}, {0.07f, 2.06f},      // mirror cone
        {0.065f, 2.09f},                                       // base cap rim
    };
    const int rings = (int)(sizeof(prof) / sizeof(prof[0]));
    const int handleRings = 5; // knob + handle rings -> handle group

    std::vector<float> data;
    for (int j = 0; j < rings; j++) {
        // Per-ring slope normal averaged from adjacent segments
        float dzPrev = j > 0 ? prof[j][1] - prof[j-1][1] : 0.0f;
        float drPrev = j > 0 ? prof[j][0] - prof[j-1][0] : 0.0f;
        float dzNext = j < rings-1 ? prof[j+1][1] - prof[j][1] : 0.0f;
        float drNext = j < rings-1 ? prof[j+1][0] - prof[j][0] : 0.0f;
        float dz = dzPrev + dzNext;
        float dr = drPrev + drNext;
        // Surface normal is perpendicular to the profile tangent (dr, dz)
        float nr = dz, nz = -dr;
        float inv = 1.0f / sqrtf(nr*nr + nz*nz + 1e-9f);
        nr *= inv; nz *= inv;

        for (int i = 0; i <= n; i++) {
            float a = (float)i / n * 2.0f * M_PI;
            float ca = cosf(a), sa = sinf(a);
            data.push_back(prof[j][0] * ca);
            data.push_back(prof[j][0] * sa);
            data.push_back(prof[j][1] - 1.0f);
            data.push_back(nr * ca);
            data.push_back(nr * sa);
            data.push_back(nz);
        }
    }

    // Base cap: center vertex + fan over the last ring
    unsigned capCenter = (unsigned)(data.size() / 6);
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(prof[rings-1][1] - 1.0f);
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f);

    std::vector<unsigned int> handleIdx;
    std::vector<unsigned int> bodyIdx;
    for (int j = 0; j < rings - 1; j++) {
        std::vector<unsigned int>& idx = (j < handleRings - 1) ? handleIdx : bodyIdx;
        for (int i = 0; i < n; i++) {
            unsigned a0 = (unsigned)(j * (n + 1) + i);
            unsigned a1 = (unsigned)(j * (n + 1) + i + 1);
            unsigned b0 = (unsigned)((j + 1) * (n + 1) + i);
            unsigned b1 = (unsigned)((j + 1) * (n + 1) + i + 1);
            idx.push_back(a0); idx.push_back(b0); idx.push_back(a1);
            idx.push_back(a1); idx.push_back(b0); idx.push_back(b1);
        }
    }
    for (int i = 0; i < n; i++) {
        bodyIdx.push_back((unsigned)((rings - 1) * (n + 1) + i));
        bodyIdx.push_back(capCenter);
        bodyIdx.push_back((unsigned)((rings - 1) * (n + 1) + i + 1));
    }

    glGenBuffers(1, &gFishVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gFishVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &gFishIBOHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gFishIBOHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, handleIdx.size() * sizeof(unsigned int), handleIdx.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &gFishIBOBody);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gFishIBOBody);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bodyIdx.size() * sizeof(unsigned int), bodyIdx.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    gFishHandleIndexCount = (GLsizei)handleIdx.size();
    gFishBodyIndexCount = (GLsizei)bodyIdx.size();
}

static void drawFishES(float x, float y, float z, float rotation, float elevation,
                       float bodyR, float bodyG, float bodyB,
                       const LightingConfig& lighting,
                       GLint posLoc, GLint nrmLoc, bool unbindAfterDraw) {
    buildFishMesh(18);

    MatrixStack m; m.loadIdentity();
    m.translate(x, y, z);
    m.rotate(rotation, 0.0f, 1.0f, 0.0f);
    m.rotate(elevation, -1.0f, 0.0f, 0.0f);

    setupShaderUniformsES(m.top(), gView, gProj,
                          FishHandleCol[0], FishHandleCol[1], FishHandleCol[2], lighting);
    bindVertexAttributesAndDraw(gFishVBO, gFishIBOHandle, gFishHandleIndexCount, posLoc, nrmLoc);

    setupShaderUniformsES(m.top(), gView, gProj, bodyR, bodyG, bodyB, lighting);
    bindVertexAttributesAndDraw(gFishVBO, gFishIBOBody, gFishBodyIndexCount, posLoc, nrmLoc);

    if (unbindAfterDraw) {
        unbindVertexAttributes(posLoc, nrmLoc);
    }
}

// Ring orientation matches legacy: pattern rotation about Y, elevation about -X,
// then the mesh-alignment rotations (+90° Y, 90° -X)
static void drawRingES(float x, float y, float z, float rotation, float elevation,
                       float colorR, float colorG, float colorB,
                       const LightingConfig& lighting,
                       GLint posLoc, GLint nrmLoc,
                       bool unbindAfterDraw) {
    MatrixStack model; model.loadIdentity();
    model.translate(x, y, z);
    model.rotate(rotation, 0.0f, 1.0f, 0.0f);
    model.rotate(elevation, -1.0f, 0.0f, 0.0f);
    model.rotate(90.0f, 0.0f, 1.0f, 0.0f);
    model.rotate(90.0f, -1.0f, 0.0f, 0.0f);

    setupShaderUniformsES(model.top(), gView, gProj, colorR, colorG, colorB, lighting);
    bindVertexAttributesAndDraw(gRingVBO, gRingIBO, gRingIndexCount, posLoc, nrmLoc);
    if (unbindAfterDraw) {
        unbindVertexAttributes(posLoc, nrmLoc);
    }
}

// Build hemisphere mesh to match legacy DrawSphere behavior
// Legacy draws half a sphere (2 panels), then rotates 90° and draws again with alt color
// Result: 4-panel beanbag look
static void buildSphereMesh(int lat, int lon) {
    if (gSphereVBO && gSphereIBO) return;

    std::vector<float> data; // interleaved position (3) + normal (3)
    std::vector<unsigned int> indices;

    // We need to generate two separate patches to match legacy DrawSphere:
    // Patch 1: X>0, Z>0 (Phi: 0 to PI/2)
    // Patch 2: X<0, Z<0 (Phi: PI to 3PI/2)

    // Number of segments per patch (quarter of total longitude)
    int patchLon = lon / 4;
    if (patchLon < 2) patchLon = 2;

    auto addPatch = [&](float startPhi, float endPhi) {
        unsigned int baseIdx = (unsigned int)(data.size() / 6);

        for (int y = 0; y <= lat; ++y) {
            float v = (float)y / (float)lat;
            float theta = v * M_PI;  // 0 to PI (top to bottom)
            float sinT = sinf(theta);
            float cosT = cosf(theta);

            for (int x = 0; x <= patchLon; ++x) {
                float u = (float)x / (float)patchLon;
                float phi = startPhi + u * (endPhi - startPhi);

                float sinP = sinf(phi);
                float cosP = cosf(phi);

                float nx = cosP * sinT;
                float ny = cosT;
                float nz = sinP * sinT;

                // position (unit sphere), then normal
                data.push_back(nx); data.push_back(ny); data.push_back(nz);
                data.push_back(nx); data.push_back(ny); data.push_back(nz);
            }
        }

        for (int y = 0; y < lat; ++y) {
            for (int x = 0; x < patchLon; ++x) {
                int i0 = baseIdx + y * (patchLon + 1) + x;
                int i1 = i0 + 1;
                int i2 = i0 + (patchLon + 1);
                int i3 = i2 + 1;
                indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
                indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
            }
        }
    };

    // Patch 1: 0 to PI/2
    addPatch(0.0f, (float)M_PI_2);

    // Patch 2: PI to 3PI/2
    addPatch((float)M_PI, (float)(1.5 * M_PI));

    gSphereIndexCount = (GLsizei)indices.size();

    glGenBuffers(1, &gSphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gSphereVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &gSphereIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gSphereIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



static void buildRingMesh(float outerR, float innerR, float thickness, int radialSeg) {
    if (gRingVBO && gRingIBO) return;
    const int n = radialSeg < 16 ? 16 : radialSeg;
    const float halfH = thickness * 0.5f;

    std::vector<float> data; // interleaved pos(3) + normal(3)
    std::vector<unsigned int> indices;
    data.reserve((size_t)((n+1)*8*6));
    indices.reserve((size_t)(n*12*3));

    auto pushVertex = [&](float x, float y, float z, float nx, float ny, float nz) -> unsigned int {
        unsigned int idx = (unsigned int)(data.size()/6);
        data.push_back(x); data.push_back(y); data.push_back(z);
        data.push_back(nx); data.push_back(ny); data.push_back(nz);
        return idx;
    };

    std::vector<unsigned int> outerLow(n+1), outerHigh(n+1);
    std::vector<unsigned int> innerLow(n+1), innerHigh(n+1);
    std::vector<unsigned int> topOuter(n+1), topInner(n+1);
    std::vector<unsigned int> botOuter(n+1), botInner(n+1);

    for (int i=0;i<=n;i++) {
        float a = (float)i / (float)n * 2.0f * M_PI;
        float ca = cosf(a), sa = sinf(a);
        // outer side
        outerLow[i]  = pushVertex(outerR*ca, -halfH, outerR*sa, ca, 0.0f, sa);
        outerHigh[i] = pushVertex(outerR*ca,  halfH, outerR*sa, ca, 0.0f, sa);
        // inner side (inward normal)
        innerLow[i]  = pushVertex(innerR*ca, -halfH, innerR*sa, -ca, 0.0f, -sa);
        innerHigh[i] = pushVertex(innerR*ca,  halfH, innerR*sa, -ca, 0.0f, -sa);
        // top face (ny=+1)
        topOuter[i]  = pushVertex(outerR*ca,  halfH, outerR*sa, 0.0f, 1.0f, 0.0f);
        topInner[i]  = pushVertex(innerR*ca,  halfH, innerR*sa, 0.0f, 1.0f, 0.0f);
        // bottom face (ny=-1)
        botOuter[i]  = pushVertex(outerR*ca, -halfH, outerR*sa, 0.0f, -1.0f, 0.0f);
        botInner[i]  = pushVertex(innerR*ca, -halfH, innerR*sa, 0.0f, -1.0f, 0.0f);
    }

    auto quad = [&](unsigned int a, unsigned int b, unsigned int c, unsigned int d) {
        indices.push_back(a); indices.push_back(b); indices.push_back(c);
        indices.push_back(c); indices.push_back(b); indices.push_back(d);
    };

    // Outer and inner sides
    for (int i=0;i<n;i++) {
        quad(outerLow[i], outerLow[i+1], outerHigh[i], outerHigh[i+1]);
        quad(innerLow[i], innerHigh[i], innerLow[i+1], innerHigh[i+1]);
    }

    // Top and bottom faces (rings)
    for (int i=0;i<n;i++) {
        quad(topOuter[i], topOuter[i+1], topInner[i], topInner[i+1]);
        quad(botInner[i], botInner[i+1], botOuter[i], botOuter[i+1]);
    }

    gRingIndexCount = (GLsizei)indices.size();
    glGenBuffers(1, &gRingVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gRingVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &gRingIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gRingIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void buildCylinderMesh(int slices) {
    if (gCylVBO && gCylIBO) return;
    int n = slices < 12 ? 12 : slices;
    std::vector<float> data; // pos(3)+nrm(3)
    std::vector<unsigned int> idx;
    // side vertices (bottom z=0, top z=1)
    for (int i=0;i<=n;i++) {
        float a = (float)i / (float)n * 2.0f * M_PI;
        float ca = cosf(a), sa = sinf(a);
        // bottom
        data.push_back(ca); data.push_back(sa); data.push_back(0.0f);
        data.push_back(ca); data.push_back(sa); data.push_back(0.0f);
        // top
        data.push_back(ca); data.push_back(sa); data.push_back(1.0f);
        data.push_back(ca); data.push_back(sa); data.push_back(0.0f);
    }
    for (int i=0;i<n;i++) {
        unsigned int i0 = i*2;
        unsigned int i1 = i0+1;
        unsigned int i2 = i0+2;
        unsigned int i3 = i0+3;
        idx.push_back(i0); idx.push_back(i2); idx.push_back(i1);
        idx.push_back(i1); idx.push_back(i2); idx.push_back(i3);
    }
    gCylIndexCount = (GLsizei)idx.size();
    glGenBuffers(1, &gCylVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gCylVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &gCylIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gCylIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void buildFrustumMesh(int slices) {
    if (gFrustumVBO && gFrustumIBO) return;
    int n = slices < 12 ? 12 : slices;
    const float topR = 0.5f;
    std::vector<float> data; // pos(3)+nrm(3)
    std::vector<unsigned int> idx;
    for (int i=0;i<=n;i++) {
        float a = (float)i / (float)n * 2.0f * M_PI;
        float ca = cosf(a), sa = sinf(a);
        // bottom ring z=0 radius=1
        data.push_back(ca); data.push_back(sa); data.push_back(0.0f);
        // normal for frustum side: approximate using vector (ca,sa, slope)
        float slope = (1.0f - topR); // delta radius across height 1
        float nx = ca, ny = sa, nz = slope;
        float inv = 1.0f / sqrtf(nx*nx + ny*ny + nz*nz);
        nx*=inv; ny*=inv; nz*=inv;
        data.push_back(nx); data.push_back(ny); data.push_back(nz);
        // top ring z=1 radius=topR
        data.push_back(topR*ca); data.push_back(topR*sa); data.push_back(1.0f);
        data.push_back(nx); data.push_back(ny); data.push_back(nz);
    }
    for (int i=0;i<n;i++) {
        unsigned int i0 = i*2;
        unsigned int i1 = i0+1;
        unsigned int i2 = i0+2;
        unsigned int i3 = i0+3;
        idx.push_back(i0); idx.push_back(i2); idx.push_back(i1);
        idx.push_back(i1); idx.push_back(i2); idx.push_back(i3);
    }
    gFrustumIndexCount = (GLsizei)idx.size();
    glGenBuffers(1, &gFrustumVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gFrustumVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &gFrustumIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gFrustumIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void buildDiskMesh(int slices) {
    if (gDiskVBO && gDiskIBO) return;
    int n = slices < 16 ? 16 : slices;
    std::vector<float> data; // pos(3)+nrm(3)
    std::vector<unsigned int> idx;
    // center vertex
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(0.0f);
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f);
    for (int i=0;i<=n;i++) {
        float a = (float)i / (float)n * 2.0f * M_PI;
        float ca = cosf(a), sa = sinf(a);
        data.push_back(ca); data.push_back(sa); data.push_back(0.0f);
        data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f);
    }
    for (int i=1;i<=n;i++) {
        idx.push_back(0); idx.push_back(i); idx.push_back(i+1);
    }
    gDiskIndexCount = (GLsizei)idx.size();
    glGenBuffers(1, &gDiskVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gDiskVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &gDiskIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gDiskIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void buildClubMesh(int radialSeg) {
    if (gClubVBO && (gClubIBOBodyEven || gClubIBOBodyOdd || gClubIBOHandle)) return;
    const int n = (radialSeg < 18 ? 18 : radialSeg); // match legacy default 18
    // Legacy profile (radii and z positions)
    const float r[4] = {0.06f, 0.10f, 0.34f, 0.34f / 2.0f};
    const float z[4] = {-0.4f, 0.6f, 1.35f, 2.1f};
    float na[4];
    na[0] = (float) atan((r[1] - r[0]) / (z[1] - z[0]));
    na[1] = (float) atan((r[2] - r[1]) / (z[2] - z[1]));
    na[2] = (float) atan((r[3] - r[1]) / (z[3] - z[1]));
    na[3] = (float) atan((r[3] - r[2]) / (z[3] - z[2]));

    // Build vertex rings for j = 1..3 (club body), plus handle rings j = 0..1
    std::vector<float> data; // interleaved pos(3) + normal(3)
    auto pushRing = [&](int j, float angle) {
        float nx = cosf(na[j]) * cosf(angle);
        float ny = cosf(na[j]) * sinf(angle);
        float nz = sinf(na[j]);
        float px = r[j] * cosf(angle);
        float py = r[j] * sinf(angle);
        float pz = z[j] - 1.0f; // Bake local Z-offset (-1.0) into mesh
        data.push_back(px); data.push_back(py); data.push_back(pz);
        data.push_back(nx); data.push_back(ny); data.push_back(nz);
    };

    // Precompute vertex indices per (j, i)
    auto indexAt = [&](int j, int i) {
        return j*(n+1) + i; // j in [0..3], i in [0..n]
    };

    // Reserve: 4 rings * (n+1) vertices
    data.reserve((size_t) (4 * (n+1) * 6));
    for (int j = 0; j <= 3; ++j) {
        for (int i = 0; i <= n; ++i) {
            float a = (float)i / (float)n * 2.0f * M_PI;
            pushRing(j, a);
        }
    }

    // Build indices: body stripes between rings (1,2,3)
    std::vector<unsigned int> evenIdx; // object color
    std::vector<unsigned int> oddIdx;  // silver
    evenIdx.reserve((size_t)(n*6*2));
    oddIdx.reserve((size_t)(n*6*2));
    for (int i = 0; i < n; ++i) {
        unsigned int i0 = indexAt(1, i);
        unsigned int i1 = indexAt(2, i);
        unsigned int i2 = indexAt(1, i+1);
        unsigned int i3 = indexAt(2, i+1);
        std::vector<unsigned int>& dst = ((i % 2) == 0) ? evenIdx : oddIdx;
        dst.push_back(i0); dst.push_back(i1); dst.push_back(i2);
        dst.push_back(i2); dst.push_back(i1); dst.push_back(i3);

        i0 = indexAt(2, i);
        i1 = indexAt(3, i);
        i2 = indexAt(2, i+1);
        i3 = indexAt(3, i+1);
        dst.push_back(i0); dst.push_back(i1); dst.push_back(i2);
        dst.push_back(i2); dst.push_back(i1); dst.push_back(i3);
    }

    // Handle (between rings 0 and 1) + end caps (disks at z[3] and z[0]) use silver
    std::vector<unsigned int> handleIdx;
    handleIdx.reserve((size_t)(n*6 + n*6));
    for (int i = 0; i < n; ++i) {
        unsigned int i0 = indexAt(0, i);
        unsigned int i1 = indexAt(1, i);
        unsigned int i2 = indexAt(0, i+1);
        unsigned int i3 = indexAt(1, i+1);
        handleIdx.push_back(i0); handleIdx.push_back(i1); handleIdx.push_back(i2);
        handleIdx.push_back(i2); handleIdx.push_back(i1); handleIdx.push_back(i3);
    }
    // End caps: add center vertices and triangles
    auto addDisk = [&](int ringJ) {
        unsigned int base = (unsigned int)(data.size()/6);
        // center normal points along +Z if ringJ==3, else along -Z for ringJ==0
        float nz = (ringJ==3 ? 1.0f : -1.0f);
        data.push_back(0.0f); data.push_back(0.0f); data.push_back(z[ringJ] - 1.0f); // Bake local Z-offset
        data.push_back(0.0f); data.push_back(0.0f); data.push_back(nz);
        for (int i=0;i<n;i++) {
            unsigned int i0 = base;
            unsigned int i1 = indexAt(ringJ, i);
            unsigned int i2 = indexAt(ringJ, i+1);
            if (ringJ==3) { // top disk
                handleIdx.push_back(i0); handleIdx.push_back(i1); handleIdx.push_back(i2);
            } else { // bottom disk (flip winding)
                handleIdx.push_back(i0); handleIdx.push_back(i2); handleIdx.push_back(i1);
            }
        }
    };
    addDisk(3);
    addDisk(0);

    // Upload buffers
    glGenBuffers(1, &gClubVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gClubVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &gClubIBOBodyEven);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gClubIBOBodyEven);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, evenIdx.size()*sizeof(unsigned int), evenIdx.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    gClubIndexCountBodyEven = (GLsizei)evenIdx.size();

    glGenBuffers(1, &gClubIBOBodyOdd);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gClubIBOBodyOdd);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, oddIdx.size()*sizeof(unsigned int), oddIdx.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    gClubIndexCountBodyOdd = (GLsizei)oddIdx.size();

    glGenBuffers(1, &gClubIBOHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gClubIBOHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, handleIdx.size()*sizeof(unsigned int), handleIdx.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    gClubIndexCountHandle = (GLsizei)handleIdx.size();
}

#ifdef __cplusplus
extern "C" {
#endif

void InitGLSettings(RENDER_STATE* pState, int WireFrame) {
    memset(pState, 0, sizeof(RENDER_STATE));
    if (!gPhong) {
        gPhong = new ShaderProgram();
        if (!gPhong->loadFromStrings(phongVertexShader, phongFragmentShader)) {
            fprintf(stderr, "ES3: Failed to load Phong shader\n");
        }
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // View matrix: simple camera back along -Z
    gView.identity();
    MatrixStack viewStack; viewStack.loadIdentity();
    viewStack.translate(0.0f, 0.0f, -12.0f);
    gView = viewStack.top();

    buildSphereMesh(24, 36);
    buildRingMesh(1.0f, 0.7f, 0.1f, 22); // legacy-style washer ring
    buildClubMesh(24);
    buildCylinderMesh(18);
    buildFrustumMesh(18);
    buildDiskMesh(24);
}

void SetCamera(RENDER_STATE* pState) {
    if (!pState || !pState->pPattern) return;
    const PATTERN_INFO* pPattern = pState->pPattern;

    // Compute H like legacy
    float H = 0.0f;
    for (int i = 0; i < pPattern->ThrowLen; i++)
        H = max(H, pPattern->pThrowInfo[i].FromPos.y);
    H += pPattern->Height;

    float ElevRad = pState->CameraElev * PI / 180.0f;
    float w = 3.0f; // legacy constant

    float ta = (pState->CameraElev  - (gFovDeg - 10.0f) / 2.0f) * PI / 180.0f;
    float ba = (pState->CameraElev  + (gFovDeg - 10.0f) / 2.0f) * PI / 180.0f;

    float tz = ta >= 0.0f ? -w : w;
    float bz = ba >= 0.0f ?  w : -w;
    float ty = H;
    float by = -1.0f;

    float d = cosf(ba) * sinf(ta) - cosf(ta) * sinf(ba);
    float a = (cosf(ba) * (by - ty) - sinf(ba) * (bz - tz)) / d;

    float ey = ty + a * sinf(ta);
    float ez = tz + a * cosf(ta);

    float cz = 0.0f;
    float cy = ey - ez * tanf(ElevRad);

    // extra zoom adjustments (ported from legacy)
    if (extraZoom > 0) {
        ez -= (ez * extraZoom);
        float multiplier = 1.0f;
        if (adjustCameraHeight) {
            if (extraZoom >= 0.29f)
                cameraHeightAdjustment = 1.5f * multiplier;
            else
                cameraHeightAdjustment = 1.0f * multiplier;
        }
        cy -= cameraHeightAdjustment;
    }

    // Compute perspective
    float dist = sqrtf(ez * ez + (cy - ey) * (cy - ey));
    gProj = perspective(gFovDeg, pState->AspectRatio > 0.0f ? pState->AspectRatio : 1.3333f, 0.1f, dist + 20.0f);
    // Apply panning (deltaX, deltaY) to lookAt
    gEyePos[0] = 0.0f; // Eye X is always 0 in lookAt call below (before view matrix transforms)
    gEyePos[1] = ey;
    gEyePos[2] = ez;

    gView = lookAt(0.0f, ey, ez, deltaX, deltaY + cy, cz, 0.0f, 1.0f, 0.0f);
}

void SetCameraExtraZoom(float f) {
    extraZoom = f;
    resetZoom = f;
    adjustCameraHeight = 1;
    deltaX = 0.0f;
    deltaY = 0.0f;
}

float GetCameraExtraZoom(void) {
    return extraZoom;
}

void MoveCamera(RENDER_STATE* pState, float deltaX_, float deltaY_) {
    deltaX += deltaX_;
    deltaY += deltaY_;
    SetCamera(pState);
}

void ResetCamera(RENDER_STATE* pState) {
    deltaX = 0.0f;
    deltaY = 0.0f;
    extraZoom = 0.0f;
    cameraHeightAdjustment = 0.0f;
    adjustCameraHeight = 1;
    SetCamera(pState);
}

void ResizeGL(RENDER_STATE* pState, int w, int h) {
    if (h <= 0) h = 1;
    glViewport(0, 0, w, h);
    float aspect = (float)w / (float)h;
    if (pState) pState->AspectRatio = aspect;
    gAspect = aspect;
    SetCamera(pState);
}

void Zoom(RENDER_STATE* pState, float zoom) {
    extraZoom += zoom;
    adjustCameraHeight = 0;
    if (extraZoom >= 0.9f) extraZoom = 0.9f;
    if (extraZoom <= 0.0f) extraZoom = 0.01f;
    SetCamera(pState);
}

void ResetZoom(RENDER_STATE* pState) {
    SetCameraExtraZoom(resetZoom);
    SetCamera(pState);
}

void DrawGLScene(RENDER_STATE* pState) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!gPhong || !gPhong->isValid()) return;

    gPhong->use();
    SetCamera(pState);

    GLint posLoc = gPhong->getAttribLocation("aPosition");
    GLint nrmLoc = gPhong->getAttribLocation("aNormal");

    // JuggleSaver lighting configuration
    const LightingConfig jsLighting = {
        {LightPos[0], LightPos[1], LightPos[2]},
        {0.0f, 0.0f, 0.0f},
        {LightDiff[0], LightDiff[1], LightDiff[2]},
        0.2f + LightAmb[0],
        gNoSpecular ? 0.0f : SpecCol[0],
        60.0f
    };

    auto drawBound2 = [&](GLuint vbo, GLuint ibo, GLsizei indexCount,
                          float tx, float ty, float tz,
                          float r1, float rx1, float ry1, float rz1,
                          float r2, float rx2, float ry2, float rz2,
                          float sx, float sy, float sz,
                          float cr, float cg, float cb) {
        drawObjectWithTransformES(vbo, ibo, indexCount,
                                   tx, ty, tz,
                                   r1, rx1, ry1, rz1,
                                   r2, rx2, ry2, rz2,
                                   sx, sy, sz,
                                   cr, cg, cb,
                                   jsLighting,
                                   posLoc, nrmLoc,
                                   true);
    };

    // Apply orbit camera: azimuth (SpinAngle) then elevation
    {
        MatrixStack vm; vm.loadIdentity(); vm.multiply(gView);
        vm.translate(5.0f * sinf(pState->TranslateAngle), 0.0f, 0.0f);
        vm.rotate(pState->SpinAngle, 0.0f, 1.0f, 0.0f);
        vm.rotate(pState->orbitElevation, 1.0f, 0.0f, 0.0f);
        vm.translate(0.0f, 0.0f, -1.0f);
        gView = vm.top();
    }

    // Pattern-driven rendering
    PATTERN_INFO* pPattern = pState->pPattern;
    if (!pPattern) return;
    int nCols = ColsCount;
    float Time = pState->Time;

    for (int i = 0; i < pPattern->Objects; i++) {
        POS ObjPos;
        float cr = Cols[i % nCols][0];
        float cg = Cols[i % nCols][1];
        float cb = Cols[i % nCols][2];

        switch (pPattern->pObjectInfo[i].ObjectType) {
            case OBJECT_CLUB:
                GetObjectPosition(pPattern, i, Time, 1.0f, &ObjPos);
                drawClubES(ObjPos.x, ObjPos.y, ObjPos.z, ObjPos.Rot, ObjPos.Elev,
                           cr, cg, cb,
                           HandleCol[0], HandleCol[1], HandleCol[2],
                           [&](float tx, float ty, float tz,
                               float r1, float rx1, float ry1, float rz1,
                               float r2, float rx2, float ry2, float rz2,
                               float sx, float sy, float sz,
                               float cr, float cg, float cb,
                               GLuint vbo, GLuint ibo, GLsizei indexCount) {
                               drawBound2(vbo, ibo, indexCount,
                                          tx, ty, tz, r1, rx1, ry1, rz1, r2, rx2, ry2, rz2,
                                          sx, sy, sz, cr, cg, cb);
                           });
                break;
            case OBJECT_FISH:
                GetObjectPosition(pPattern, i, Time, 1.0f, &ObjPos);
                drawFishES(ObjPos.x, ObjPos.y, ObjPos.z, ObjPos.Rot, ObjPos.Elev,
                           cr, cg, cb, jsLighting, posLoc, nrmLoc, true);
                break;
            case OBJECT_RING:
                GetObjectPosition(pPattern, i, Time, 1.0f, &ObjPos);
                drawRingES(ObjPos.x, ObjPos.y, ObjPos.z, ObjPos.Rot, ObjPos.Elev,
                           cr, cg, cb, jsLighting, posLoc, nrmLoc, true);
                break;
            default: // ball
                GetObjectPosition(pPattern, i, Time, 0.0f, &ObjPos);
                // Get alternate color
                float acr = AltCols[i % (AltColsCount)][0];
                float acg = AltCols[i % (AltColsCount)][1];
                float acb = AltCols[i % (AltColsCount)][2];

                // Apply solid color mode if needed
                float finalCr = (gSolidColor ? 0.7f : cr);
                float finalCg = (gSolidColor ? 0.7f : cg);
                float finalCb = (gSolidColor ? 0.7f : cb);
                float finalAcr = (gSolidColor ? 0.7f : acr);
                float finalAcg = (gSolidColor ? 0.7f : acg);
                float finalAcb = (gSolidColor ? 0.7f : acb);

                drawBallES(ObjPos.x, ObjPos.y, ObjPos.z, ObjPos.Rot,
                           finalCr, finalCg, finalCb,
                           finalAcr, finalAcg, finalAcb,
                           [&](float tx, float ty, float tz,
                               float r1, float rx1, float ry1, float rz1,
                               float r2, float rx2, float ry2, float rz2,
                               float sx, float sy, float sz,
                               float cr, float cg, float cb) {
                drawBound2(gSphereVBO, gSphereIBO, gSphereIndexCount,
                                          tx, ty, tz, r1, rx1, ry1, rz1, r2, rx2, ry2, rz2,
                                          sx, sy, sz, cr, cg, cb);
                           });
                break;
        }
    }

    // Draw JuggleSaver juggler (legacy-accurate geometry and kinematics)
    const float sX = ShoulderPos[0];
    const float sY = ShoulderPos[1];
    const float sZ = -ShoulderPos[2];

    // Body coordinate system matches legacy (no rotation needed)
    MatrixStack bodyBase; bodyBase.loadIdentity();

    auto drawTransformed = [&](MatrixStack& base, float sx, float sy, float sz,
                               GLuint vbo, GLuint ibo, GLsizei idx,
                               float cr, float cg, float cb) {
        MatrixStack model = base; // copy
        if (sx != 1.0f || sy != 1.0f || sz != 1.0f) model.scale(sx, sy, sz);
        Matrix4 mv = gView * model.top();
        gPhong->setMatrix4("uModelViewMatrix", mv.m);
        gPhong->setMatrix4("uMVPMatrix", (gProj * mv).m);
        gPhong->setMatrix4("uNormalMatrix", mv.inverse().transpose().m);
        gPhong->setVec4("uColor", cr, cg, cb, 1.0f);
        GLint posLoc = gPhong->getAttribLocation("aPosition");
        GLint nrmLoc = gPhong->getAttribLocation("aNormal");
        bindVertexAttributesAndDraw(vbo, ibo, idx, posLoc, nrmLoc);
        unbindVertexAttributes(posLoc, nrmLoc);
    };

    auto deg = [&](float radians) { return radians * 180.0f / PI; };

    auto drawArmES = [&](int Left) {
        // Legacy DrawArm convention: Left=1 is the right arm, Left=0 the left
        POS hand; GetHandPosition(pState->pPattern, Left, Time, &hand);
        float x = hand.x + (Left ? -sX : sX);
        float y = hand.y - sY;
        float len = sqrtf(x * x + y * y + ShoulderPos[2] * ShoulderPos[2]);
        float len2 = sqrtf(x * x + ShoulderPos[2] * ShoulderPos[2]);
        float ang  = (float) CosineRuleES(UArmLen, len, LArmLen);
        float ang2 = (float) CosineRuleES(UArmLen, LArmLen, len);
        if (ang == 0.0f && ang2 == 0.0f) ang2 = 180.0f;

        MatrixStack base = bodyBase;
        base.translate(Left ? sX : -sX, sY, sZ);
        // match legacy rotation sequence exactly
        base.rotate(deg(asinf(x / len2)), 0.0f, 1.0f, 0.0f);
        base.rotate(-deg(asinf(y / len)), 1.0f, 0.0f, 0.0f);
        base.rotate(Left ? 20.0f : -20.0f, 0.0f, 0.0f, 1.0f);
        base.rotate(ang, 1.0f, 0.0f, 0.0f);

        // Shoulder joint: two spheres (base + 90° rotated) like legacy DrawUpperArm
        drawTransformed(base, 0.3f, 0.3f, 0.3f, gSphereVBO, gSphereIBO, gSphereIndexCount,
                        BodyCol[0], BodyCol[1], BodyCol[2]);
        MatrixStack shoulder2 = base; shoulder2.rotate(90.0f, 0.0f, 1.0f, 0.0f);
        drawTransformed(shoulder2, 0.3f, 0.3f, 0.3f, gSphereVBO, gSphereIBO, gSphereIndexCount,
                        BodyCol[0], BodyCol[1], BodyCol[2]);

        // Upper arm: cylinder along +Z (unit), scaled to UArmLen with radius 0.3
        drawTransformed(base, 0.3f, 0.3f, UArmLen, gCylVBO, gCylIBO, gCylIndexCount,
                        BodyCol[0], BodyCol[1], BodyCol[2]);

        // Elbow joint: two spheres (base + 90° rotated) like legacy DrawUpperArm
        MatrixStack elbow = base; elbow.translate(0.0f, 0.0f, UArmLen);
        drawTransformed(elbow, 0.3f, 0.3f, 0.3f, gSphereVBO, gSphereIBO, gSphereIndexCount,
                        BodyCol[0], BodyCol[1], BodyCol[2]);
        MatrixStack elbow2 = elbow; elbow2.rotate(90.0f, 0.0f, 1.0f, 0.0f);
        drawTransformed(elbow2, 0.3f, 0.3f, 0.3f, gSphereVBO, gSphereIBO, gSphereIndexCount,
                        BodyCol[0], BodyCol[1], BodyCol[2]);

        // Forearm: additional rotate then frustum of length LArmLen (0.3 -> 0.15)
        MatrixStack fore = elbow; fore.rotate(ang2 - 180.0f, 1.0f, 0.0f, 0.0f);
        drawTransformed(fore, 0.3f, 0.3f, LArmLen, gFrustumVBO, gFrustumIBO, gFrustumIndexCount,
                        BodyCol[0], BodyCol[1], BodyCol[2]);
        // Wrist disk at end of forearm (like legacy DrawForearm)
        MatrixStack wrist = fore; wrist.translate(0.0f, 0.0f, LArmLen);
        drawTransformed(wrist, 0.15f, 0.15f, 1.0f, gDiskVBO, gDiskIBO, gDiskIndexCount,
                        BodyCol[0], BodyCol[1], BodyCol[2]);
    };

    // Torso/head: match legacy DrawHead/DrawTorso
    MatrixStack head = bodyBase; head.translate(0.0f, sY + 1.0f, sZ);
    head.rotate(-30.0f, 1.0f, 0.0f, 0.0f);
    // head cylinder
    drawTransformed(head, 0.5f, 0.5f, 0.3f, gCylVBO, gCylIBO, gCylIndexCount,
                    BodyCol[0], BodyCol[1], BodyCol[2]);
    // bottom disk (flip)
    MatrixStack headDisk = head; headDisk.rotate(180.0f, 1.0f, 0.0f, 0.0f);
    drawTransformed(headDisk, 0.5f, 0.5f, 1.0f, gDiskVBO, gDiskIBO, gDiskIndexCount,
                    BodyCol[0], BodyCol[1], BodyCol[2]);
    // top disk
    MatrixStack headTop = head; headTop.translate(0.0f, 0.0f, 0.3f);
    drawTransformed(headTop, 0.5f, 0.5f, 1.0f, gDiskVBO, gDiskIBO, gDiskIndexCount,
                    BodyCol[0], BodyCol[1], BodyCol[2]);
    MatrixStack torso = bodyBase; torso.translate(0.0f, sY - (ShoulderPos[1] + 1.0f), sZ);
    torso.rotate(-90.0f, 1.0f, 0.0f, 0.0f);
    drawTransformed(torso, 0.3f, 0.3f, ShoulderPos[1] + 1.0f, gCylVBO, gCylIBO, gCylIndexCount,
                    BodyCol[0], BodyCol[1], BodyCol[2]);

    // Shoulder bar (horizontal across shoulders): fixed rotation to +X, length = 2*sX
    MatrixStack shoulderMs = bodyBase; shoulderMs.translate(-sX, sY, sZ); shoulderMs.rotate(90.0f, 0.0f, 1.0f, 0.0f);
    drawTransformed(shoulderMs,
                    0.3f, 0.3f, sX*2.0f,
                    gCylVBO, gCylIBO, gCylIndexCount,
                    BodyCol[0], BodyCol[1], BodyCol[2]);
    // End caps for shoulder bar
    MatrixStack shoulderCap1 = shoulderMs; shoulderCap1.rotate(180.0f, 1.0f, 0.0f, 0.0f);
    drawTransformed(shoulderCap1, 0.3f, 0.3f, 1.0f, gDiskVBO, gDiskIBO, gDiskIndexCount,
                    BodyCol[0], BodyCol[1], BodyCol[2]);
    MatrixStack shoulderCap2 = shoulderMs; shoulderCap2.translate(0.0f, 0.0f, sX*2.0f);
    drawTransformed(shoulderCap2, 0.3f, 0.3f, 1.0f, gDiskVBO, gDiskIBO, gDiskIndexCount,
                    BodyCol[0], BodyCol[1], BodyCol[2]);

    // Arms - match legacy calling convention: DrawArm(pState, Time, 1) then DrawArm(pState, Time, 0)
    drawArmES(1); // Right arm (Left=1)
    drawArmES(0); // Left arm (Left=0)
}

void SetPattern(RENDER_STATE* pState, char* pattern) {
    if (pState->pPattern != NULL)
        ReleasePatternInfo(pState->pPattern);

    pState->pPattern = (PATTERN_INFO*) malloc(sizeof(PATTERN_INFO));

    int ExtSiteLen;
    EXT_SITE_INFO* pExtInfo = ParsePattern(pattern, &ExtSiteLen);
    InitPatternInfo(pState->pPattern, NULL, pExtInfo, ExtSiteLen);

    free(pExtInfo);

    pState->CameraElev = 0.0f;
    pState->TranslateAngle = 0.0f;
    pState->SpinAngle = 0.0f;
    pState->Time = 50.0f;
}

// Helper functions for JuggleMaster juggler rendering in ES path

// Draw a full sphere using hemisphere mesh (matches legacy DrawFullSphere)
// Legacy draws hemisphere twice with 90° Y rotation between them
static void drawFullSphereES(float radius, float x, float y, float z,
                             const float* color,
                             GLint posLoc, GLint nrmLoc) {
    if (!gPhong) return;

    // First hemisphere (use world coordinates directly)
    MatrixStack model1; model1.loadIdentity();
    model1.translate(x, y, z);
    model1.scale(radius, radius, radius);
    Matrix4 mv = gView * model1.top();
    gPhong->setMatrix4("uModelViewMatrix", mv.m);
    gPhong->setMatrix4("uMVPMatrix", (gProj * mv).m);
    gPhong->setMatrix4("uNormalMatrix", mv.inverse().transpose().m);

    gPhong->setVec4("uColor", color[0], color[1], color[2], color[3]);

    bindVertexAttributesAndDraw(gSphereVBO, gSphereIBO, gSphereIndexCount, posLoc, nrmLoc);

    // Second hemisphere rotated 90° around Y (matches legacy DrawFullSphere)
    MatrixStack model2; model2.loadIdentity();
    model2.translate(x, y, z);
    model2.rotate(90.0f, 0.0f, 1.0f, 0.0f);
    model2.scale(radius, radius, radius);
    mv = gView * model2.top();
    gPhong->setMatrix4("uModelViewMatrix", mv.m);
    gPhong->setMatrix4("uMVPMatrix", (gProj * mv).m);
    gPhong->setMatrix4("uNormalMatrix", mv.inverse().transpose().m);

    gPhong->setVec4("uColor", color[0], color[1], color[2], color[3]);

    bindVertexAttributesAndDraw(gSphereVBO, gSphereIBO, gSphereIndexCount, posLoc, nrmLoc);
}

// Draw a cylinder between two points (matches legacy DrawCylinderAt)
// Uses frustum mesh if radii differ, cylinder mesh if same
static void drawCylinderAtES(POS from, float from_rad, POS to, float to_rad,
                              const float* color,
                              GLint posLoc, GLint nrmLoc) {
    if (!gPhong) return;

    // Orientation vector
    float vx = to.x - from.x;
    float vy = to.y - from.y;
    float vz = to.z - from.z;
    float length = sqrtf(vx*vx + vy*vy + vz*vz);

    if (length < 1e-6f) return; // degenerate case

    // Rotation to align +Z axis with the from->to vector
    // Using same logic as legacy DrawCylinderAt
    float r2d = 180.0f / PI;
    float rx = -vy * vz;
    float ry = vx * vz;
    float ax = 0.0f;
    const float zero = 1.0e-3f;

    if (fabs(vz) < zero) {
        ax = r2d * acosf(vx / length);  // rotation angle in x-y plane
        if (vx <= 0.0f) ax = -ax;
    } else {
        ax = r2d * acosf(vz / length);
        if (vz <= 0.0f) ax = -ax;
    }

    MatrixStack model; model.loadIdentity();
    // Use world coordinates directly (no centering offset)
    model.translate(from.x, from.y, from.z);

    if (fabs(vz) < zero) {
        model.rotate(90.0f, 0.0f, 1.0f, 0.0f);   // align with x axis
        model.rotate(ax, -1.0f, 0.0f, 0.0f);     // rotate to target in x-y plane
    } else {
        model.rotate(ax, rx, ry, 0.0f);
    }

    // Scale: use cylinder mesh with average radius
    // Unit cylinder is radius=1, height=1 along +Z

    if (fabs(from_rad - to_rad) < 1e-4f) {
        // Uniform cylinder
        model.scale(from_rad, from_rad, length);

        Matrix4 mv = gView * model.top();
        gPhong->setMatrix4("uModelViewMatrix", mv.m);
        gPhong->setMatrix4("uMVPMatrix", (gProj * mv).m);
        gPhong->setMatrix4("uNormalMatrix", mv.inverse().transpose().m);
        gPhong->setVec4("uColor", color[0], color[1], color[2], color[3]);

        // Always use cylinder mesh (uniform radius along length)
        bindVertexAttributesAndDraw(gCylVBO, gCylIBO, gCylIndexCount, posLoc, nrmLoc);
    } else {
        // Tapered cylinder (frustum)
        model.scale(1.0f, 1.0f, length); // Scale Z only, radii handled by mesh

        Matrix4 mv = gView * model.top();
        gPhong->setMatrix4("uModelViewMatrix", mv.m);
        gPhong->setMatrix4("uMVPMatrix", (gProj * mv).m);
        gPhong->setMatrix4("uNormalMatrix", mv.inverse().transpose().m);
        gPhong->setVec4("uColor", color[0], color[1], color[2], color[3]);

        // Only a handful of distinct radius pairs occur (arm segment profile),
        // so build each tapered mesh once and cache its VBO. Client-side
        // arrays are invalid in core WebGL 2 / ES 3.0.
        struct TaperedMesh { float fromRad; float toRad; GLuint vbo; GLsizei count; };
        static std::vector<TaperedMesh> taperedCache;

        const int slices = 12;
        GLuint vbo = 0;
        GLsizei count = 0;
        for (size_t i = 0; i < taperedCache.size(); i++) {
            if (fabsf(taperedCache[i].fromRad - from_rad) < 1e-5f &&
                fabsf(taperedCache[i].toRad - to_rad) < 1e-5f) {
                vbo = taperedCache[i].vbo;
                count = taperedCache[i].count;
                break;
            }
        }

        if (vbo == 0) {
            std::vector<float> data;
            float dr = from_rad - to_rad;
            // Side normal: (cos, sin, dr) normalized — perpendicular to the slanted side
            for (int i=0; i<=slices; i++) {
                float a = (float)i / slices * 2.0f * M_PI;
                float ca = cosf(a), sa = sinf(a);

                float nx = ca;
                float ny = sa;
                float nz = dr;
                float inv = 1.0f / sqrtf(nx*nx + ny*ny + nz*nz);
                nx*=inv; ny*=inv; nz*=inv;

                // Bottom (Z=0)
                data.push_back(from_rad * ca); data.push_back(from_rad * sa); data.push_back(0.0f);
                data.push_back(nx); data.push_back(ny); data.push_back(nz);

                // Top (Z=1)
                data.push_back(to_rad * ca); data.push_back(to_rad * sa); data.push_back(1.0f);
                data.push_back(nx); data.push_back(ny); data.push_back(nz);
            }

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
            count = (slices + 1) * 2;

            TaperedMesh entry = { from_rad, to_rad, vbo, count };
            taperedCache.push_back(entry);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        if (posLoc >= 0) {
            glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)0);
            glEnableVertexAttribArray(posLoc);
        }
        if (nrmLoc >= 0) {
            glVertexAttribPointer(nrmLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(sizeof(float)*3));
            glEnableVertexAttribArray(nrmLoc);
        }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, count);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

// Draw JuggleMaster juggler (matches legacy JMDrawJMJuggler)
static void drawJMJugglerES(JUGGLEMASTER_RENDER_STATE* pState,
                            GLint posLoc, GLint nrmLoc,
                            const LightingConfig& lighting) {
    if (!gPhong) return;

    gPhong->setVec3("uLightPos", lighting.lightPos[0], lighting.lightPos[1], lighting.lightPos[2]);
    gPhong->setVec3("uViewPos", lighting.viewPos[0], lighting.viewPos[1], lighting.viewPos[2]);
    gPhong->setVec3("uLightColor", lighting.lightColor[0], lighting.lightColor[1], lighting.lightColor[2]);
    gPhong->setFloat("uAmbient", lighting.ambient);
    gPhong->setFloat("uSpecularStrength", lighting.specularStrength);
    gPhong->setFloat("uShininess", lighting.shininess);

    // Right arm (3 segments)
    for (int i = 0; i < 3; i++) {
        POS from, to;
        from.x = pState->juggler.rx[i];
        from.y = pState->juggler.ry[i];
        from.z = pState->juggler.z[i];

        to.x = pState->juggler.rx[i+1];
        to.y = pState->juggler.ry[i+1];
        to.z = pState->juggler.z[i+1];

        if (i == 4) to.y += 0.15f;

        // Draw joint sphere (skip first one)
        if (i > 0) {
            drawFullSphereES(pState->juggler.rad[i], from.x, from.y, from.z,
                            BodyCol, posLoc, nrmLoc);
        }

        // Draw arm segment cylinder
        drawCylinderAtES(from, pState->juggler.rad[i], to, pState->juggler.rad[i+1],
                        BodyCol, posLoc, nrmLoc);
    }

    // Left arm (3 segments)
    for (int i = 0; i < 3; i++) {
        POS from, to;
        from.x = pState->juggler.lx[i];
        from.y = pState->juggler.ly[i];
        from.z = pState->juggler.z[i];

        to.x = pState->juggler.lx[i+1];
        to.y = pState->juggler.ly[i+1];
        to.z = pState->juggler.z[i+1];

        if (i == 4) to.y += 0.15f;

        // Draw joint sphere (skip first one)
        if (i > 0) {
            drawFullSphereES(pState->juggler.rad[i], from.x, from.y, from.z,
                            BodyCol, posLoc, nrmLoc);
        }

        // Draw arm segment cylinder
        drawCylinderAtES(from, pState->juggler.rad[i], to, pState->juggler.rad[i+1],
                        BodyCol, posLoc, nrmLoc);
    }

    // Complete the body (shoulder bar and torso)
    POS from, to, mid;
    from.x = pState->juggler.lx[3];
    from.y = pState->juggler.ly[3];
    from.z = pState->juggler.z[3];

    to.x = pState->juggler.rx[3];
    to.y = pState->juggler.ry[3];
    to.z = pState->juggler.z[3];

    mid.x = (from.x + to.x) / 2.0f;
    mid.y = pState->juggler.ly[4];
    mid.z = pState->juggler.z[4];

    // Left shoulder joint
    drawFullSphereES(pState->juggler.rad[3], from.x, from.y, from.z,
                    BodyCol, posLoc, nrmLoc);

    // Left shoulder to center
    drawCylinderAtES(from, 0.3f, mid, 0.3f, BodyCol, posLoc, nrmLoc);

    // Center shoulder joint
    drawFullSphereES(pState->juggler.rad[3], mid.x, mid.y, mid.z,
                    BodyCol, posLoc, nrmLoc);

    // Center to right shoulder
    drawCylinderAtES(mid, 0.3f, to, 0.3f, BodyCol, posLoc, nrmLoc);

    // Right shoulder joint
    drawFullSphereES(pState->juggler.rad[3], to.x, to.y, to.z,
                    BodyCol, posLoc, nrmLoc);

    // Vertical torso cylinder (from mid down)
    float height = ShoulderPos[1] + 1.0f;
    MatrixStack torso; torso.loadIdentity();
    torso.translate(mid.x, mid.y - height, mid.z);
    torso.rotate(-90.0f, 1.0f, 0.0f, 0.0f);
    torso.scale(0.3f, 0.3f, height);

    Matrix4 mv = gView * torso.top();
    gPhong->setMatrix4("uModelViewMatrix", mv.m);
    gPhong->setMatrix4("uMVPMatrix", (gProj * mv).m);
    gPhong->setMatrix4("uNormalMatrix", mv.inverse().transpose().m);
    gPhong->setVec4("uColor", BodyCol[0], BodyCol[1], BodyCol[2], BodyCol[3]);

    bindVertexAttributesAndDraw(gCylVBO, gCylIBO, gCylIndexCount, posLoc, nrmLoc);

    // Bottom disk of torso
    MatrixStack torsoDisk; torsoDisk.loadIdentity();
    torsoDisk.translate(mid.x, mid.y - height, mid.z);
    torsoDisk.rotate(-90.0f, 1.0f, 0.0f, 0.0f);
    torsoDisk.rotate(180.0f, 1.0f, 0.0f, 0.0f);
    torsoDisk.scale(0.3f, 0.3f, 1.0f);

    Matrix4 mvDisk = gView * torsoDisk.top();
    gPhong->setMatrix4("uModelViewMatrix", mvDisk.m);
    gPhong->setMatrix4("uMVPMatrix", (gProj * mvDisk).m);
    gPhong->setMatrix4("uNormalMatrix", mvDisk.inverse().transpose().m);
    gPhong->setVec4("uColor", BodyCol[0], BodyCol[1], BodyCol[2], BodyCol[3]);

    bindVertexAttributesAndDraw(gDiskVBO, gDiskIBO, gDiskIndexCount, posLoc, nrmLoc);

    // Head (matches legacy: cylinder with two disks)
    MatrixStack head; head.loadIdentity();
    head.translate(pState->juggler.hx, pState->juggler.hy, pState->juggler.hz);
    head.rotate(-30.0f, 1.0f, 0.0f, 0.0f);
    head.scale(pState->juggler.hr, pState->juggler.hr, 0.3f);

    Matrix4 mvHead = gView * head.top();
    gPhong->setMatrix4("uModelViewMatrix", mvHead.m);
    gPhong->setMatrix4("uMVPMatrix", (gProj * mvHead).m);
    gPhong->setMatrix4("uNormalMatrix", mvHead.inverse().transpose().m);
    gPhong->setVec4("uColor", BodyCol[0], BodyCol[1], BodyCol[2], BodyCol[3]);

    bindVertexAttributesAndDraw(gCylVBO, gCylIBO, gCylIndexCount, posLoc, nrmLoc);

    // Head bottom disk
    MatrixStack headBottom; headBottom.loadIdentity();
    headBottom.translate(pState->juggler.hx, pState->juggler.hy, pState->juggler.hz);
    headBottom.rotate(-30.0f, 1.0f, 0.0f, 0.0f);
    headBottom.rotate(180.0f, 1.0f, 0.0f, 0.0f);
    headBottom.rotate(180.0f, 0.0f, 0.0f, 1.0f);
    headBottom.scale(pState->juggler.hr, pState->juggler.hr, 1.0f);

    Matrix4 mvHeadBot = gView * headBottom.top();
    gPhong->setMatrix4("uMVPMatrix", (gProj * mvHeadBot).m);
    gPhong->setMatrix4("uModelViewMatrix", mvHeadBot.m);
    gPhong->setMatrix4("uNormalMatrix", mvHeadBot.inverse().transpose().m);

    bindVertexAttributesAndDraw(gDiskVBO, gDiskIBO, gDiskIndexCount, posLoc, nrmLoc);

    // Head top disk
    MatrixStack headTop; headTop.loadIdentity();
    headTop.translate(pState->juggler.hx, pState->juggler.hy, pState->juggler.hz);
    headTop.rotate(-30.0f, 1.0f, 0.0f, 0.0f);
    headTop.translate(0.0f, 0.0f, 0.3f);
    headTop.scale(pState->juggler.hr, pState->juggler.hr, 1.0f);

    Matrix4 mvHeadTop = gView * headTop.top();
    gPhong->setMatrix4("uMVPMatrix", (gProj * mvHeadTop).m);
    gPhong->setMatrix4("uModelViewMatrix", mvHeadTop.m);
    gPhong->setMatrix4("uNormalMatrix", mvHeadTop.inverse().transpose().m);

    bindVertexAttributesAndDraw(gDiskVBO, gDiskIBO, gDiskIndexCount, posLoc, nrmLoc);
}

void JMDrawGLScene(JUGGLEMASTER_RENDER_STATE* pState, RENDER_STATE* jsState) {
    // ES 3.0 path: render JuggleMaster-converted state using shader/VBO path
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!gPhong) return; // not initialized yet

    int nCols = (int)(ColsCount);

    gPhong->use();

    GLint posLoc = gPhong->getAttribLocation("aPosition");
    GLint nrmLoc = gPhong->getAttribLocation("aNormal");

    // Use SetCamera with JuggleSaver state to get correct projection/view
    // Legacy does the same: uses jsState (which has pattern info) to set camera for JM rendering
    // The jsState pattern is set to match JM pattern's highest throw for correct zoom level
    SetCamera(jsState);

    // Apply orbit camera: azimuth (SpinAngle) then elevation
    {
        MatrixStack vm; vm.loadIdentity(); vm.multiply(gView);
        vm.translate(5.0f * sinf(pState->TranslateAngle), 0.0f, 0.0f);
        vm.rotate(pState->SpinAngle, 0.0f, 1.0f, 0.0f);
        vm.rotate(jsState->orbitElevation, 1.0f, 0.0f, 0.0f);
        vm.translate(0.0f, 0.0f, -1.0f);
        gView = vm.top();
    }

    // JuggleMaster lighting configuration
    const LightingConfig jmLighting = {
        {0.0f, 10.0f, 10.0f},
        {0.0f, 0.0f, 10.0f},
        {1.0f, 1.0f, 1.0f},
        0.5f,
        gNoSpecular ? 0.0f : 0.6f,
        60.0f
    };

    // Common drawBound function for objects that don't need post-rotation translation
    auto drawBound = [&](GLuint vbo, GLuint ibo, GLsizei indexCount,
                         float tx, float ty, float tz,
                         float r1, float rx1, float ry1, float rz1,
                         float r2, float rx2, float ry2, float rz2,
                         float sx, float sy, float sz,
                         float cr, float cg, float cb) {
        drawObjectWithTransformES(vbo, ibo, indexCount,
                                   tx, ty, tz,
                                   r1, rx1, ry1, rz1,
                                   r2, rx2, ry2, rz2,
                                   sx, sy, sz,
                                   cr, cg, cb,
                                   jmLighting,
                                   posLoc, nrmLoc,
                                   false);
    };

    for (int i = 0; i < pState->objectCount; ++i) {
        float cr = Cols[i % nCols][0];
        float cg = Cols[i % nCols][1];
        float cb = Cols[i % nCols][2];

        const float x = pState->objects[i].x;
        const float y = pState->objects[i].y;
        const float z = pState->objects[i].z;
        const float rot = pState->objects[i].Rot;
        const float elev = pState->objects[i].Elev;

        switch (pState->objectTypes[i]) {
            case OBJECT_CLUB: {
                drawClubES(x, y, z, rot, elev,
                           cr, cg, cb,
                           HandleCol[0], HandleCol[1], HandleCol[2],
                           [&](float tx, float ty, float tz,
                               float r1, float rx1, float ry1, float rz1,
                               float r2, float rx2, float ry2, float rz2,
                               float sx, float sy, float sz,
                               float cr, float cg, float cb,
                               GLuint vbo, GLuint ibo, GLsizei indexCount) {
                               drawBound(vbo, ibo, indexCount,
                                         tx, ty, tz, r1, rx1, ry1, rz1, r2, rx2, ry2, rz2,
                                         sx, sy, sz, cr, cg, cb);
                           });
                break;
            }
            case OBJECT_FISH:
                drawFishES(x, y, z, rot, elev,
                           cr, cg, cb, jmLighting, posLoc, nrmLoc, false);
                break;
            case OBJECT_RING:
                drawRingES(x, y, z, rot, elev,
                           cr, cg, cb, jmLighting, posLoc, nrmLoc, false);
                break;
            default:
            {
                // Get alternate color
                float acr = AltCols[i % (AltColsCount)][0];
                float acg = AltCols[i % (AltColsCount)][1];
                float acb = AltCols[i % (AltColsCount)][2];

                // Apply solid color mode if needed
                float finalCr = (gSolidColor ? 0.7f : cr);
                float finalCg = (gSolidColor ? 0.7f : cg);
                float finalCb = (gSolidColor ? 0.7f : cb);
                float finalAcr = (gSolidColor ? 0.7f : acr);
                float finalAcg = (gSolidColor ? 0.7f : acg);
                float finalAcb = (gSolidColor ? 0.7f : acb);

                drawBallES(x, y, z, rot,
                           finalCr, finalCg, finalCb,
                           finalAcr, finalAcg, finalAcb,
                           [&](float tx, float ty, float tz,
                               float r1, float rx1, float ry1, float rz1,
                               float r2, float rx2, float ry2, float rz2,
                               float sx, float sy, float sz,
                               float cr, float cg, float cb) {
                drawBound(gSphereVBO, gSphereIBO, gSphereIndexCount,
                                         tx, ty, tz, r1, rx1, ry1, rz1, r2, rx2, ry2, rz2,
                                         sx, sy, sz, cr, cg, cb);
                           });
                break;
            }
        }
    }

    // Draw the juggler (torso, arms, head) using pState->juggler data
    // This matches the legacy behavior exactly
    drawJMJugglerES(pState, posLoc, nrmLoc, jmLighting);
}

// Debug hook (C linkage) to toggle specular and solid color
#ifdef __cplusplus
extern "C" {
#endif
void js_render_set_debug(int noSpec, int solid) {
    gNoSpecular = (noSpec != 0);
    gSolidColor = (solid != 0);
}
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif
