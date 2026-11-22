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

#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "gl_includes.h"
#include <string>
#include <map>

class ShaderProgram {
private:
    GLuint programId;
    GLuint vertexShader;
    GLuint fragmentShader;
    // Location lookups are string-keyed GL calls (a JS-boundary call each in
    // WebGL); cache per program, invalidated on relink
    std::map<std::string, GLint> uniformCache;
    std::map<std::string, GLint> attribCache;

    bool compileShader(GLuint shader, const char* source);
    bool linkProgram();

public:
    ShaderProgram();
    ~ShaderProgram();

    // Load and compile shaders
    bool loadFromStrings(const char* vertexSource, const char* fragmentSource);

    // Use this program
    void use();

    // Get uniform/attribute locations (cached)
    GLint getUniformLocation(const char* name);
    GLint getAttribLocation(const char* name);

    // Set uniforms
    void setMatrix4(const char* name, const float* matrix);
    void setVec3(const char* name, float x, float y, float z);
    void setVec4(const char* name, float x, float y, float z, float w);
    void setFloat(const char* name, float value);
    void setInt(const char* name, int value);

    // Get program ID
    GLuint getProgramId() const { return programId; }

    // Check if valid
    bool isValid() const { return programId != 0; }
};

#endif // SHADER_MANAGER_H
