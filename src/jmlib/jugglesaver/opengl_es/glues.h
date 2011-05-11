#ifndef __GLUES_H__
#define __GLUES_H__

//fixme: add other applicable platforms here
#ifdef __IPHONE__
#include <OpenGLES/ES1/gl.h>
#else
#include <GLES/gl.h>
#endif

//#pragma comment(lib, "glues.lib")

#if defined (__cplusplus)
extern "C" {
#endif

void gluPerspectivef(GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f);

void gluPerspectivef2x(GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f);

void gluPerspectivex(GLfixed fovy, GLfixed aspect, GLfixed n, GLfixed f);

void gluLookAtf(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
      GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz); 

void gluLookAtf2x(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
      GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz); 

void gluLookAtx(GLfixed eyex, GLfixed eyey, GLfixed eyez, GLfixed centerx,
      GLfixed centery, GLfixed centerz, GLfixed upx, GLfixed upy, GLfixed upz);

#ifdef GLES_CL
#	define gluPerspective	gluPerspectivef2x
#	define gluLookAt		gluLookAtf2x
#else
#	define gluPerspective	gluPerspectivef
#	define gluLookAt		gluLookAtf
#endif

#if defined (__cplusplus)
}
#endif
      
#endif // __GLUES_H__