/* Juggler3D, Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.  No
 * representations are made about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

#include "jugglesaver.h"

#ifdef __cplusplus
extern "C" {
#endif
 
/*****************************************************************************
 *
 *  Juggle Saver Patterns
 *
 *****************************************************************************
 *
 * This is a selection of some of the more interesting patterns from taken
 * from the Juggle Saver sites.txt file.  I've only used patterns that I
 * originally created.
 */
const char* PatternText[] =
{
    "9b@(-2.5,0,-70,40)>(2.5,0,70)*2 1b@(1,0,10)>(-1,0,-10)",
    
    "3B@(1,-0.4)>(2,4.2)/(-2,1)3B@(-1.8,4.4)>(-2.1,0)",
    
    "7c@(-2,0,-20)>(1.2,0,-5)7c@(2,0,20)>(-1.2,0,5)",
    
    "3b@(-0.5,0)>(1.5,0) 3b@(0.5,0)>(-1.5,0) 3r@(-2.5,3,-90,80)>(2,1,90,30)"
    "3b@(0.5,0)>(-1.5,0) 3b@(-0.5,0)>(1.5,0) 3r@(2.5,3,90,80)>(-2,1,-90,30)",
    
    "5c@(2,1.9,10)>(-1,1,10)5c@(2,1.8,10)>(-0.5,1.6,10)/(5,-1)"
    "5c@(1.6,0.2,10)>(0,-1,10)/(9,-2)5c@(-2,1.9,-10)>(1,1,-10)"
    "5c@(-2,1.8,-10)>(0.5,1.6,-10)/(-5,-1)5@(-1.6,0.2,-10)>(0,-1,-10)/(-9,-2)",
    
    "3c@(-1.5,0,0)>(-1.5,1,0)3c@(1.5,-0.2,0)>(1.5,-0.1,0)3c@(0,-0.5,0)>(0,1,0)"
    "3@(-1.5,2,0)>(-1.5,-1,0)3@(1.5,0,0)>(1.5,1,0)3@(0,0,0)>(0,-0.5,0)",
    
    "9c@(-2.5,0,-70,40)>(2.5,0,70)*2 1c@(1,0,10)>(-1,0,-10)*0",
    
    "3c@(2,0.5,60,0)>(1.5,4,60,80)/(-6,-12)"
    "3c@(-2,0.5,-60,0)>(-1.5,4,-60,80)/(6,-12)",
    
    "3c@(-0.2,0)>(1,0)3c@(0.2,0)>(-1,0)3c@(-2.5,2,-85,30)>(2.5,2,85,40)*2 "
    "3@(0.2,0)>(-1,0) 3@(-0.2,0)>(1,0) 3@(2.5,2,85,30)>(-2.5,2,-85,40)*2",
    
    "3c@(-0.5,-0.5,20,-30)>(2.6,4.3,60,60)/(0,1)*1 "
    "3c@(1.6,5.6,60,80)>(-2.6,0,-80)*0",
    
    "5c@(-0.3,0,10)>(1.2,0,10) 5c@(0.3,0,-10)>(-1.2,0,-10)"
    "5c@(-0.3,0,10)>(1.2,0,10) 5c@(0.3,0,-10)>(-1.2,0,-10)"
    "5c@(-3,3.5,-65,80)>(3,2.5,65) 5c@(0.3,0,-10)>(-1.2,0,-10)"
    "5@(-0.3,0,10)>(1.2,0,10) 5@(0.3,0,-10)>(-1.2,0,-10)"
    "5@(-0.3,0,10)>(1.2,0,10)5@(3,3.5,65,80)>(-3,2.5,-65)"
};

/*****************************************************************************
 *
 * Rendering
 *
 *****************************************************************************/

static const float FOV = 70.0f;
static const float BodyCol[] = {0.6f, 0.6f, 0.45f, 1.0f};
static const float HandleCol[] = {0.45f, 0.45f, 0.45f, 1.0f};
static const float LightPos[] = {0.0f, 200.0f, 400.0f, 1.0f};
static const float LightDiff[] = {1.0f, 1.0f, 1.0f, 0.0f};
static const float LightAmb[] = {0.02f, 0.02f, 0.02f, 0.0f};
static const float ShoulderPos[3] = {0.95f, 2.1f, 1.7f};
static const float DiffCol[] = {1.0f, 0.0f, 0.0f, 1.0f};
static const float SpecCol[] = {1.0f, 1.0f, 1.0f, 1.0f};

static const float BallRad = 0.34f;
static const float UArmLen = 1.9f;
static const float LArmLen = 2.3f;

#define DL_BALL 0
#define DL_CLUB 1
#define DL_RING 2
#define DL_TORSO 3
#define DL_FOREARM 4
#define DL_UPPERARM 5

static const float AltCols[][4] =
{
    {0.0f, 0.7f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.9f, 1.0f},
    {0.0f, 0.9f, 0.9f, 1.0f},
    {0.45f, 0.0f, 0.9f, 1.0f},
    {0.9f, 0.45f, 0.0f, 1.0f},
    {0.0f, 0.45f, 0.9f, 1.0f},
    {0.9f, 0.0f, 0.9f, 1.0f},
    {0.9f, 0.9f, 0.0f, 1.0f},
    {0.9f, 0.0f, 0.45f, 1.0f},
    {0.45f, 0.15f, 0.6f, 1.0f}, 
    {0.9f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.9f, 0.45f, 1.0f},
};

static const float Cols[][4] =
{
    {0.9f, 0.0f, 0.0f, 1.0f},  /*  0 */
    {0.0f, 0.7f, 0.0f, 1.0f},  /*  1 */
    {0.0f, 0.0f, 0.9f, 1.0f},  /*  2 */
    {0.0f, 0.9f, 0.9f, 1.0f},  /*  3 */
    {0.9f, 0.0f, 0.9f, 1.0f},  /*  4 */
    {0.9f, 0.9f, 0.0f, 1.0f},  /*  5 */
    {0.9f, 0.45f, 0.0f, 1.0f}, /*  6 */
    {0.9f, 0.0f, 0.45f, 1.0f}, /*  7 */
    {0.45f, 0.9f, 0.0f, 1.0f}, /*  8 */
    {0.0f, 0.9f, 0.45f, 1.0f}, /*  9 */
    {0.45f, 0.0f, 0.9f, 1.0f}, /* 10 */
    {0.0f, 0.45f, 0.9f, 1.0f}, /* 11 */
};

int InitGLDisplayLists(void);


void InitGLSettings(RENDER_STATE* pState, int WireFrame)
{
    srand((unsigned int)time(NULL));

    memset(pState, 0, sizeof(RENDER_STATE));
        
    if (WireFrame)
        glPolygonMode(GL_FRONT, GL_LINE);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiff);
    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmb);
    
    glEnable(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    pState->DLStart = InitGLDisplayLists();
}

static float extraZoom = 0.0f;

void SetCameraExtraZoom(float f)
{
  extraZoom = f;
}

void SetCamera(RENDER_STATE* pState)
{
    /* Try to work out a sensible place to put the camera so that more or less
     * the whole juggling pattern fits into the screen. We assume that the
	 * pattern is height limited (i.e. if we get the height right then the width
	 * will be OK).  This is a pretty good assumption given that the screen
	 * tends to wider than high, and that a juggling pattern is normally much
	 * higher than wide.
     *
     * If I could draw a diagram here then it would be much easier to
     * understand but my ASCII-art skills just aren't up to it.  
     *
     * Basically we estimate a bounding volume for the juggler and objects 
     * throughout the pattern.  We don't fully account for the fact that the
     * juggler moves across the stage in an epicyclic-like motion and instead
     * use the near and far planes in x-y (with z = +/- w).  We also
     * assume that the scene is centred at x=0, this reduces our task to finding
     * a bounding rectangle.  Finally we need to make an estimate of the
     * height - for this we work out the max height of a standard throw or max
     * weight from the pattern; we then do a bit of adjustment to account for
     * a throw occurring at non-zero y values.
     *
     * Next we work out the best way to fit this rectangle into the perspective
     * transform.  Based on the angle of elevation (+ve angle looks down) and
     * the FOV we can work out whether it's the near or far corners that are
     * the extreme points.  And then trace back from them to find the eye
     * point.
     *
     */
     
    float ElevRad = pState->CameraElev * PI / 180.0f;
    float w = 3.0f;
    float cy, cz;
    float ey, ez;
    float d;
    float H = 0.0f;
    int i;
    float a;
    
    float tz, ty, ta;
    float bz, by, ba;
    const PATTERN_INFO* pPattern = pState->pPattern;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
        
    for (i = 0; i < pPattern->ThrowLen; i++)
        H = max(H, pPattern->pThrowInfo[i].FromPos.y);
        
    H += pPattern->Height;
    
    ElevRad = pState->CameraElev * PI / 180.0f;
    
    /* ta is the angle from a point on the top of the bounding area to the eye
     * similarly ba is the angle from a point on the bottom. */
    ta = (pState->CameraElev  - (FOV - 10.0f) / 2.0f) * PI / 180.0f;
    ba = (pState->CameraElev  + (FOV - 10.0f) / 2.0f) * PI / 180.0f;

    /* tz and bz hold the z location of the top and bottom extreme points.
     * For the top, if the angle to the eye location is positive then the
     * extreme point is with far z corner (the camera looks in -ve z).
     * The logic is reserved for the bottom. */
    tz = ta >= 0.0f ? -w : w;
    bz = ba >= 0.0f ? w : -w;
    
    ty = H;
    by = -1.0f;
    
    /* Solve of the eye location by using a bit of geometry.
     * We know the eye lies on intersection of two lines.  One comes from the
     * top and other from the bottom. Giving two equations:
     *   ez = tz + a * cos(ta) = bz + b * cos(ba)
     *   ey = ty + a * sin(ta) = by + b * sin(ba)
     * We don't bother to solve for b and use Crammer's rule to get
     *         | bz-tz  -cos(ba) |
     *         | by-ty  -sin(ba) |     
     *   a =  ----------------------
     *        | cos(ta)   -cos(ba) |
     *        | sin(ta)   -sin(ba) |
     */
    d = cosf(ba) * sinf(ta) - cosf(ta) * sinf(ba);
    a = (cosf(ba) * (by - ty) - sinf(ba) * (bz - tz)) / d;
    
    ey = ty + a * sinf(ta);
    ez = tz + a * cosf(ta);
    
    /* now work back from the eye point to get the lookat location */
    cz = 0.0;
    cy = ey - ez * tanf(ElevRad);
    
    /* use the distance from the eye to the scene centre to get a measure
     * of what the far clipping should be.  We then add on a bit more to be 
     * comfortable */
    d = sqrtf(ez * ez + (cy - ey) * (cy - ey));
    
    gluPerspective(FOV, pState->AspectRatio, 0.1f, d + 20.0f);
    
    // adjust the camera for JuggleMaster
    if (extraZoom > 0) {
      ez -= (ez * extraZoom);
      if (extraZoom >= 0.29f)
        cy -= 1.5f;
      else
        cy -= 1.0f;
    }
    
    gluLookAt(0.0, ey, ez, 0.0, cy, cz, 0.0, 1.0, 0.0);

    glMatrixMode(GL_MODELVIEW);
}


void ResizeGL(RENDER_STATE* pState, int w, int h)
{
    glViewport(0, 0, w, h);
    pState->AspectRatio = (float) w / h;
    SetCamera(pState);
}


/* Determine the angle at the vertex of a triangle given the length of the
 * three sides. */

double CosineRule(double a, double b, double c)
{
    double cosang = (a * a + b * b - c * c) / (2 * a * b);
    /* If lengths don't form a proper triangle return something sensible.
     * This typically happens with patterns where the juggler reaches too 
     * far to get hold of an object. */
    if (cosang < -1.0 || cosang > 1.0)
        return 0;
    else
        return 180.0 * acos(cosang) / PI;
}


/* Spheres for the balls are generated by subdividing each triangle face into
 * four smaller triangles.  We start with an octahedron (8 sides) and repeat the
 * process a number of times.  The result is a mesh that can be split into four
 * panels (like beanbags) and is smoother than the normal stacks and slices
 * approach. */

void InterpolateVertex(
    const float* v1, const float* v2, float t, float* result)
{
    result[0] = v1[0] * (1.0f - t) + v2[0] * t;
    result[1] = v1[1] * (1.0f - t) + v2[1] * t;
    result[2] = v1[2] * (1.0f - t) + v2[2] * t;
}


void SetGLVertex(const float* v, float rad)
{
    float Len = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    if (Len >= 1.0e-10f)
    {
        glNormal3f(v[0] / Len, v[1] / Len, v[2] / Len);
        glVertex3f(rad * v[0] / Len, rad * v[1] / Len, rad * v[2] / Len);
    }
    else
        glVertex3fv(v);
}


void SphereSegment(
    const float* v1, const float* v2, const float* v3, float r, int Levels)
{
    int i, j;

    for (i = 0; i < Levels; i++)
    {
        float A[3], B[3], C[3], D[3];
        
        InterpolateVertex(v3, v1, (float) i / Levels, D);
        InterpolateVertex(v3, v1, (float)(i + 1) / Levels, A);
        InterpolateVertex(v3, v2, (float)(i + 1) / Levels, B);
        InterpolateVertex(v3, v2, (float) i / Levels, C);

        glBegin(GL_TRIANGLE_STRIP);

        SetGLVertex(B, r);
        SetGLVertex(C, r);
        
        for (j = 1; j <= i; j++)
        {
            float v[3];

            InterpolateVertex(B, A, (float) j / (i + 1), v);
            SetGLVertex(v, r);

            InterpolateVertex(C, D, (float) j / i, v);
            SetGLVertex(v, r);
        }

        SetGLVertex(A, r);
        
        glEnd();
    }
}


/* OK, this function is a bit of misnomer, it only draws half a sphere.  Indeed
 * it draws two panels and allows us to colour this one way,  then draw the
 * same shape again rotated 90 degrees in a different colour.  Resulting in what
 * looks like a four-panel beanbag in two complementary colours. */
 
void DrawSphere(float rad)
{
    int Levels = 4;
    float v1[3], v2[3], v3[3];
    
    v1[0] = 1.0f, v1[1] = 0.0f; v1[2] = 0.0f;
    v2[0] = 0.0f, v2[1] = 1.0f; v2[2] = 0.0f;
    v3[0] = 0.0f, v3[1] = 0.0f; v3[2] = 1.0f;
    SphereSegment(v1, v2, v3, rad, Levels);
    
    v2[1] = -1.0f;
    SphereSegment(v2, v1, v3, rad, Levels);
    
    v1[0] = v3[2] = -1.0f;
    SphereSegment(v2, v1, v3, rad, Levels);

    v2[1] = 1.0f;
    SphereSegment(v1, v2, v3, rad, Levels);
}


void DrawRing(void)
{
    const int Facets = 22;
    const float w = 0.1f;
    GLUquadric* pQuad = gluNewQuadric();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -w / 2.0f);

    gluCylinder(pQuad, 1.0f, 1.0f, w, Facets, 1);
    gluQuadricOrientation(pQuad, GLU_INSIDE);

    gluCylinder(pQuad, 0.7f, 0.7f, w, Facets, 1);
    gluQuadricOrientation(pQuad, GLU_OUTSIDE);

    glTranslatef(0.0f, 0.0f, w);
    gluDisk(pQuad, 0.7, 1.0f, Facets, 1);

    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, w);
    gluDisk(pQuad, 0.7, 1.0f, Facets, 1);

    gluDeleteQuadric(pQuad);
}


/* The club follows a 'circus club' design i.e. it has stripes running down the
 * body.  The club is draw such that the one stripe uses the current material
 * and the second stripe the standard silver colour. */

void DrawClub(void)
{
    const float r[4] = {0.06f, 0.1f, 0.34f, 0.34f / 2.0f};
    const float z[4] = {-0.4f, 0.6f, 1.35f, 2.1f};
    float na[4];
    const int n = 18;
    int i, j;
    GLUquadric* pQuad;

    na[0] = (float) atan((r[1] - r[0]) / (z[1] - z[0]));
    na[1] = (float) atan((r[2] - r[1]) / (z[2] - z[1]));
    na[2] = (float) atan((r[3] - r[1]) / (z[3] - z[1]));
    na[3] = (float) atan((r[3] - r[2]) / (z[3] - z[2]));

    for (i = 0; i < n; i += 2)
    {
        float a1 = i * PI * 2.0f / n;
        float a2 = (i + 1) * PI * 2.0f / n;

        glBegin(GL_TRIANGLE_STRIP);
            for (j = 1; j < 4; j++)
            {
                glNormal3f(cosf(na[j]) * cosf(a1),
                    cosf(na[j]) * sinf(a1), sinf(na[j]));

                glVertex3f(r[j] * cosf(a1), r[j] * sinf(a1), z[j]);

                glNormal3f(cosf(na[j]) * cosf(a2),
                    cosf(na[j]) * sinf(a2),    sinf(na[j]));

                glVertex3f(r[j] * cosf(a2), r[j] * sinf(a2), z[j]);
            }
        glEnd();
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, HandleCol);

    for (i = 1; i < n; i += 2)
    {
        float a1 = i * PI * 2.0f / n;
        float a2 = (i + 1) * PI * 2.0f / n;

        glBegin(GL_TRIANGLE_STRIP);
            for (j = 1; j < 4; j++)
            {
                glNormal3f(cosf(na[j]) * cosf(a1),
                    cosf(na[j]) * sinf(a1),    sinf(na[j]));

                glVertex3f(r[j] * cosf(a1), r[j] * sinf(a1), z[j]);

                glNormal3f(cosf(na[j]) * cosf(a2),
                    cosf(na[j]) * sinf(a2), sinf(na[j]));

                glVertex3f(r[j] * cosf(a2), r[j] * sinf(a2), z[j]);
            }
        glEnd();
    }

    pQuad = gluNewQuadric();
    glTranslatef(0.0f, 0.0f, z[0]);
    gluCylinder(pQuad, r[0], r[1], z[1] - z[0], n, 1);

    glTranslatef(0.0f, 0.0f, z[3] - z[0]);
    gluDisk(pQuad, 0.0, r[3], n, 1);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, z[3] - z[0]);
    gluDisk(pQuad, 0.0, r[0], n, 1);
    gluDeleteQuadric(pQuad);
}


/* In total 6 display lists are used.  There are created based on the DL_
 * constants defined earlier.  The function returns the index of the first
 * display list, all others can be calculated based on an offset from there. */

int InitGLDisplayLists(void)
{
    int s = glGenLists(6);
    GLUquadric* pQuad;

    glNewList(s + DL_BALL, GL_COMPILE);
    DrawSphere(BallRad);
    glEndList();

    glNewList(s + DL_CLUB, GL_COMPILE);
    DrawClub();
    glEndList();

    glNewList(s + DL_RING, GL_COMPILE);
    DrawRing();
    glEndList();
    
    pQuad =  gluNewQuadric();
    gluQuadricNormals(pQuad, GLU_SMOOTH);    
    
    glNewList(s + DL_TORSO, GL_COMPILE);
        glPushMatrix();
            glTranslatef(ShoulderPos[0], ShoulderPos[1], -ShoulderPos[2]);
            glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
            gluCylinder(pQuad, 0.3, 0.3, ShoulderPos[0] * 2, 18, 1);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0f, -1.0f, -ShoulderPos[2]);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(pQuad, 0.3, 0.3, ShoulderPos[1] + 1.0f, 18, 1);
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(pQuad, 0.0, 0.3, 18, 1);
        glPopMatrix();
        
        /* draw the head */
        glPushMatrix();
            glTranslatef(0.0f, ShoulderPos[1] + 1.0f, -ShoulderPos[2]);
            glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(pQuad, 0.5, 0.5, 0.3, 15, 1);
            
            glPushMatrix();
                glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
                glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
                gluDisk(pQuad, 0.0, 0.5, 15, 1);
            glPopMatrix(); 
                
            glTranslatef(0.0f, 0.0f, .3f);
            gluDisk(pQuad, 0.0, 0.5, 15, 1);
        glPopMatrix();        
    glEndList();
    
    glNewList(s + DL_UPPERARM, GL_COMPILE);
        gluQuadricNormals(pQuad, GLU_SMOOTH);
        gluQuadricDrawStyle(pQuad, GLU_FILL);
        gluSphere(pQuad, 0.3, 12, 8);

        gluCylinder(pQuad, 0.3, 0.3, UArmLen, 12, 1); 
        glTranslatef(0.0f, 0.0f, UArmLen);
        gluSphere(pQuad, 0.3, 12, 8);
    glEndList();

    glNewList(s + DL_FOREARM, GL_COMPILE);
        gluCylinder(pQuad, 0.3, 0.3 / 2.0f, LArmLen, 12, 1);
        glTranslatef(0.0f, 0.0f, LArmLen);
        gluDisk(pQuad, 0, 0.3 / 2.0f, 18, 1);
    glEndList();

    gluDeleteQuadric(pQuad);
    return s;
}


/* Drawing the arm requires connecting the upper and fore arm between the
 * shoulder and hand position.  Thinking about things kinematically by treating
 * the shoulder and elbow as ball joints then, provided the arm can stretch far
 * enough, there's a infnite number of ways to position the elbow.  Basically
 * it's possible to fix and hand and shoulder and then rotate the elbow a full
 * 360 degrees.  Clearly human anatomy isn't like this and picking a natural
 * elbow position can be complex.  We chicken out and assume that poking the
 * elbow out by 20 degrees from the lowest position gives a reasonably looking
 * orientation. */

void DrawArm(RENDER_STATE* pState, float TimePos, int Left)
{
    POS Pos;
    float x, y, len, len2, ang, ang2;
    
    GetHandPosition(pState->pPattern, Left, TimePos, &Pos);

    x = Pos.x + (Left ? -ShoulderPos[0] : ShoulderPos[0]);
    y = Pos.y - ShoulderPos[1];


    len = sqrtf(x * x + y * y + ShoulderPos[2] * ShoulderPos[2]);
    len2 = sqrtf(x * x + ShoulderPos[2] * ShoulderPos[2]);

    ang = (float) CosineRule(UArmLen, len, LArmLen);
    ang2 = (float) CosineRule(UArmLen, LArmLen, len);

    if (ang == 0.0 && ang2 == 0)
        ang2 = 180.0;


    glPushMatrix();
        glTranslatef(Left ? ShoulderPos[0] : -ShoulderPos[0], ShoulderPos[1],
            -ShoulderPos[2]);
        glRotatef((float)(180.0f * asin(x / len2) / 3.14f), 0.0f, 1.0f, 0.0f);
        glRotatef((float)(-180.f * asin(y / len) / 3.14), 1.0f, 0.0f, 0.0f);
        glRotatef(Left ? 20.0f : -20.0f, 0.0f, 0.0f, 1.0f);
        glRotatef((float) ang, 1.0f, 0.0f, 0.0f);
        glCallList(DL_UPPERARM + pState->DLStart);

        glRotatef((float)(ang2 - 180.0), 1.0f, 0.0f, 0.f);
        glCallList(DL_FOREARM + pState->DLStart);
    glPopMatrix();
}


void DrawGLScene(RENDER_STATE* pState)
{
    float Time = pState->Time;
    int nCols = sizeof(Cols) / sizeof(Cols[0]);
    int i;

    PATTERN_INFO* pPattern = pState->pPattern;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(5.0f * sinf(pState->TranslateAngle), 0.0f, 0.0f);

    gltrackball_rotate (pState->trackball);

    glRotatef(pState->SpinAngle, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0, 0.0, -1.0f);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, DiffCol);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, SpecCol);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);

    /* draw test objects
    float x, y;
    const float min_y = -3.0;
    const float max_y = 10.0;
    const float min_x = -5.0;
    const float max_x = 5.0;

    for (int i = 1; i < 5; i++) {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Cols[i % nCols]);
        glPushMatrix();

        switch (i) {
        case 1: 
          x = min_x;
          y = min_y;
          break;
        case 2:
          x = min_x;
          y = max_y;
          break;
        case 3:
          x = max_x;
          y = max_y;
          break;
        case 4:
          x = max_x;
          y = min_y;
          break;
        }
        
        glTranslatef(x, y, 0);  
        //glRotatef(pState->balls[i].Rot, 0.6963f, 0.6963f, 0.1742f);
        glCallList(DL_BALL + pState->DLStart);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, AltCols[i % nCols]);
        glCallList(DL_BALL + pState->DLStart);  
        
        glPopMatrix();
    }
    */

    for (i = 0; i < pPattern->Objects; i++)
    {
        POS ObjPos;
        
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Cols[i % nCols]);
        glPushMatrix();

        switch (pPattern->pObjectInfo[i].ObjectType)
        {
            case OBJECT_CLUB:
                GetObjectPosition(pPattern, i, Time, 1.0f, &ObjPos);
                glTranslatef(ObjPos.x, ObjPos.y, ObjPos.z);
                glRotatef(ObjPos.Rot, 0.0f, 1.0f, 0.0f);
                glRotatef(ObjPos.Elev, -1.0f, 0.0f, 0.0f);
                glTranslatef(0.0f, 0.0f, -1.0f);
                glCallList(DL_CLUB + pState->DLStart);
                break;

            case OBJECT_RING:
                GetObjectPosition(pPattern, i, Time, 1.0f, &ObjPos);
                glTranslatef(ObjPos.x, ObjPos.y, ObjPos.z);
                glRotatef(ObjPos.Rot, 0.0f, 1.0f, 0.0f);
                glRotatef(ObjPos.Elev, -1.0f, 0.0f, 0.0f);
                glCallList(DL_RING + pState->DLStart);
                break;

            default:
                GetObjectPosition(pPattern, i, Time, 0.0f, &ObjPos);
                glTranslatef(ObjPos.x, ObjPos.y, ObjPos.z);        
                glRotatef(ObjPos.Rot, 0.6963f, 0.6963f, 0.1742f);
                glCallList(DL_BALL + pState->DLStart);
                glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
                    AltCols[i % nCols]);
                glCallList(DL_BALL + pState->DLStart);
                break;
        }

        glPopMatrix();
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, BodyCol);
    glCallList(DL_TORSO + pState->DLStart);
    DrawArm(pState, Time, 1);
    DrawArm(pState, Time, 0);
}


int RandInRange(int Min, int Max)
{
    return Min + rand() % (1 + Max - Min);
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
    SetCamera(pState);
}

void UpdatePattern(
    RENDER_STATE* pState, int MinBalls, int MaxBalls, 
    int MinHeightInc, int MaxHeightInc)
{
    if (pState->pPattern != NULL)
        ReleasePatternInfo(pState->pPattern);
    
    pState->pPattern = (PATTERN_INFO*) malloc(sizeof(PATTERN_INFO));
    
    if ((rand() % 3) == 1)
    {    
        int ExtSiteLen;
        int n = rand() % (sizeof(PatternText) / sizeof(PatternText[0]));
        EXT_SITE_INFO* pExtInfo = ParsePattern(PatternText[n], &ExtSiteLen);
        InitPatternInfo(pState->pPattern, NULL, pExtInfo, ExtSiteLen);
        free(pExtInfo);
    }
    else
    {
        int* pRand;
        int ballcount, maxweight;
        const int RandPatternLen = 1500;
        
        ballcount = RandInRange(MinBalls, MaxBalls);
        maxweight = ballcount  + RandInRange(MinHeightInc, MaxHeightInc);
        
        pRand = Generate(RandPatternLen, maxweight, ballcount);
        InitPatternInfo(pState->pPattern, pRand, NULL, RandPatternLen);
        free(pRand);
    }
    
    pState->CameraElev = 50.0f - rand() % 90;
    pState->TranslateAngle = (float)(rand() % 360);
    pState->SpinAngle = (float)(rand() % 360);
    pState->Time = 50.0f;
    SetCamera(pState);
}

#ifdef __cplusplus
}
#endif

void JMDrawArm(JUGGLEMASTER_RENDER_STATE* pState, int Left)
{
    POS Pos;
    float x, y, len, len2, ang, ang2;
    
    if (Left) Pos = pState->leftHand;
    else Pos = pState->rightHand;
    //GetHandPosition(pState->pPattern, Left, TimePos, &Pos);

    x = Pos.x + (Left ? -ShoulderPos[0] : ShoulderPos[0]);
    y = Pos.y - ShoulderPos[1];

    //x = -1.0;
    //y = 10.0;

    len = sqrtf(x * x + y * y + ShoulderPos[2] * ShoulderPos[2]);
    len2 = sqrtf(x * x + ShoulderPos[2] * ShoulderPos[2]);

    ang = (float) CosineRule(UArmLen, len, LArmLen);
    ang2 = (float) CosineRule(UArmLen, LArmLen, len);

    if (ang == 0.0 && ang2 == 0)
        ang2 = 180.0;


    glPushMatrix();
        glTranslatef(Left ? ShoulderPos[0] : -ShoulderPos[0], ShoulderPos[1],
            -ShoulderPos[2]);
        glRotatef((float)(180.0f * asin(x / len2) / 3.14f), 0.0f, 1.0f, 0.0f);
        glRotatef((float)(-180.f * asin(y / len) / 3.14), 1.0f, 0.0f, 0.0f);
        glRotatef(Left ? 20.0f : -20.0f, 0.0f, 0.0f, 1.0f);
        glRotatef((float) ang, 1.0f, 0.0f, 0.0f);
        glCallList(DL_UPPERARM + pState->DLStart);

        glRotatef((float)(ang2 - 180.0), 1.0f, 0.0f, 0.f);
        glCallList(DL_FOREARM + pState->DLStart);
    glPopMatrix();
}

// Render a JuggleMaster state using JuggleSaver
void JMDrawGLScene(JUGGLEMASTER_RENDER_STATE* pState)
{
    int nCols = sizeof(Cols) / sizeof(Cols[0]);
    int i;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(5.0f * sinf(pState->TranslateAngle), 0.0f, 0.0f);

    gltrackball_rotate (pState->trackball);

    glRotatef(pState->SpinAngle, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0, 0.0, -1.0f);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, DiffCol);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, SpecCol);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);

    /* draw test objects 
    float x, y;
    const float min_y = -3.0;
    const float max_y = 10.0;
    const float min_x = -5.0;
    const float max_x = 5.0;

    for (int i = 1; i < 5; i++) {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Cols[i % nCols]);
        glPushMatrix();

        switch (i) {
        case 1: 
          x = min_x;
          y = min_y;
          break;
        case 2:
          x = min_x;
          y = max_y;
          break;
        case 3:
          x = max_x;
          y = max_y;
          break;
        case 4:
          x = max_x;
          y = min_y;
          break;
        }
        
        glTranslatef(x, y, 0);  
        //glRotatef(pState->balls[i].Rot, 0.6963f, 0.6963f, 0.1742f);
        glCallList(DL_BALL + pState->DLStart);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, AltCols[i % nCols]);
        glCallList(DL_BALL + pState->DLStart);  
        
        glPopMatrix();
    }
    */

    for (i = 0; i < pState->objectCount; i++)
    {
        //POS ObjPos;
        
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Cols[i % nCols]);
        glPushMatrix();

        switch (pState->objectTypes[i])
        {
            case OBJECT_CLUB:
                glTranslatef(pState->objects[i].x, pState->objects[i].y, pState->objects[i].z);
                glRotatef(pState->objects[i].Rot, 0.0f, 1.0f, 0.0f);
                glRotatef(pState->objects[i].Elev, -1.0f, 0.0f, 0.0f);
                glTranslatef(0.0f, 0.0f, -1.0f);
                glCallList(DL_CLUB + pState->DLStart);
                break;

            case OBJECT_RING:
                glTranslatef(pState->objects[i].x, pState->objects[i].y, pState->objects[i].z);
                glRotatef(pState->objects[i].Rot, 0.0f, 1.0f, 0.0f);
                glRotatef(pState->objects[i].Elev, -1.0f, 0.0f, 0.0f);
                glCallList(DL_RING + pState->DLStart);
                break;

            default:
                glTranslatef(pState->objects[i].x, pState->objects[i].y, pState->objects[i].z);        
                glRotatef(pState->objects[i].Rot, 0.6963f, 0.6963f, 0.1742f);
                glCallList(DL_BALL + pState->DLStart);
                glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
                    AltCols[i % nCols]);
                glCallList(DL_BALL + pState->DLStart);
                break;
        }

        glPopMatrix();
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, BodyCol);
    glCallList(DL_TORSO + pState->DLStart);
    JMDrawArm(pState, 1);
    JMDrawArm(pState, 0);
}
