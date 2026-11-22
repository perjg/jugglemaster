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

#include "shader_manager.h"
#include <stdio.h>
#include <stdlib.h>

ShaderProgram::ShaderProgram() : programId(0), vertexShader(0), fragmentShader(0) {
}

ShaderProgram::~ShaderProgram() {
    if(vertexShader) glDeleteShader(vertexShader);
    if(fragmentShader) glDeleteShader(fragmentShader);
    if(programId) glDeleteProgram(programId);
}

bool ShaderProgram::compileShader(GLuint shader, const char* source) {
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation failed: %s\n", infoLog);
        return false;
    }
    return true;
}

bool ShaderProgram::linkProgram() {
    glLinkProgram(programId);

    GLint success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        fprintf(stderr, "Shader program linking failed: %s\n", infoLog);
        return false;
    }
    return true;
}

bool ShaderProgram::loadFromStrings(const char* vertexSource, const char* fragmentSource) {
    uniformCache.clear();
    attribCache.clear();

    // Create shaders
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile shaders
    if(!compileShader(vertexShader, vertexSource)) return false;
    if(!compileShader(fragmentShader, fragmentSource)) return false;

    // Create and link program
    programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);

    if(!linkProgram()) return false;

    return true;
}

void ShaderProgram::use() {
    if(programId) {
        glUseProgram(programId);
    }
}

GLint ShaderProgram::getUniformLocation(const char* name) {
    if(!programId) return -1;
    std::map<std::string, GLint>::iterator it = uniformCache.find(name);
    if (it != uniformCache.end()) return it->second;
    GLint loc = glGetUniformLocation(programId, name);
    uniformCache[name] = loc;
    return loc;
}

GLint ShaderProgram::getAttribLocation(const char* name) {
    if(!programId) return -1;
    std::map<std::string, GLint>::iterator it = attribCache.find(name);
    if (it != attribCache.end()) return it->second;
    GLint loc = glGetAttribLocation(programId, name);
    attribCache[name] = loc;
    return loc;
}

void ShaderProgram::setMatrix4(const char* name, const float* matrix) {
    GLint loc = getUniformLocation(name);
    if(loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, matrix);
    }
}

void ShaderProgram::setVec3(const char* name, float x, float y, float z) {
    GLint loc = getUniformLocation(name);
    if(loc >= 0) {
        glUniform3f(loc, x, y, z);
    }
}

void ShaderProgram::setVec4(const char* name, float x, float y, float z, float w) {
    GLint loc = getUniformLocation(name);
    if(loc >= 0) {
        glUniform4f(loc, x, y, z, w);
    }
}

void ShaderProgram::setFloat(const char* name, float value) {
    GLint loc = getUniformLocation(name);
    if(loc >= 0) {
        glUniform1f(loc, value);
    }
}

void ShaderProgram::setInt(const char* name, int value) {
    GLint loc = getUniformLocation(name);
    if(loc >= 0) {
        glUniform1i(loc, value);
    }
}
