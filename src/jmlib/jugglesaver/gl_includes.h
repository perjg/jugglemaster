/*
 * JMLib - Portable JuggleMaster Library
 * Version 3.0
 * Copyright (c) Per Johan Groland 2025-2026
 *
 * JuggleMaster is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#ifndef GL_INCLUDES_H
#define GL_INCLUDES_H

#ifdef USE_OPENGL_ES
  // OpenGL ES 3.0 (for iOS, WebAssembly)
  #include <GLES3/gl3.h>
  #define USING_OPENGL_ES_3
#elif defined(USE_SHADER_RENDERING)
    // Desktop OpenGL with shaders
    #ifdef __APPLE__
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
        #include <OpenGL/glext.h>
    #else
        #define GL_GLEXT_PROTOTYPES
        #include <GL/gl.h>
        #include <GL/glu.h>
        #include <GL/glext.h>
    #endif
#else
  // Desktop OpenGL Fixed Pipeline
  #ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <OpenGL/glext.h>
  #else
    #include <GL/gl.h>
    #include <GL/glu.h>
  #endif
#endif

#endif // GL_INCLUDES_H
