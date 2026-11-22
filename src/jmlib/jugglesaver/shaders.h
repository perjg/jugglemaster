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

#ifndef SHADERS_H
#define SHADERS_H

#ifdef USE_OPENGL_ES
// ============================================================================
// GLSL ES 3.00 (WebGL 2.0 / native OpenGL ES 3.0)
// ============================================================================

// Basic flat color shader (for 2D rendering and simple objects)
static const char* flatColorVertexShader = R"(#version 300 es
precision highp float;

in vec3 aPosition;
uniform mat4 uMVPMatrix;

void main() {
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
}
)";

static const char* flatColorFragmentShader = R"(#version 300 es
precision highp float;

uniform vec4 uColor;
out vec4 fragColor;

void main() {
    fragColor = uColor;
}
)";

// Phong lighting shader (for 3D rendering with lighting)
static const char* phongVertexShader = R"(#version 300 es
precision highp float;

in vec3 aPosition;
in vec3 aNormal;

uniform mat4 uMVPMatrix;

uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

out vec3 vNormal;
out vec3 vFragPos;

void main() {
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
    vFragPos = vec3(uModelViewMatrix * vec4(aPosition, 1.0));
    vNormal = mat3(uNormalMatrix) * aNormal;
}
)";

static const char* phongFragmentShader = R"(#version 300 es
precision highp float;

in vec3 vNormal;
in vec3 vFragPos;

uniform vec4 uColor;
uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
uniform float uAmbient;
uniform float uSpecularStrength;
uniform float uShininess;

out vec4 fragColor;

void main() {
    vec3 ambient = uAmbient * uLightColor;
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    vec3 viewDir = normalize(uViewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    vec3 specular = uSpecularStrength * spec * uLightColor;
    vec3 result = (ambient + diffuse + specular) * uColor.rgb;
    fragColor = vec4(result, uColor.a);
}
)";

// Simple 2D line shader (for pattern trails)
static const char* lineVertexShader = R"(#version 300 es
precision highp float;

in vec2 aPosition;
uniform mat4 uMVPMatrix;

void main() {
    gl_Position = uMVPMatrix * vec4(aPosition, 0.0, 1.0);
}
)";

static const char* lineFragmentShader = R"(#version 300 es
precision highp float;

uniform vec4 uColor;
out vec4 fragColor;

void main() {
    fragColor = uColor;
}
)";

#else
// ============================================================================
// GLSL 1.20 (OpenGL 2.1) for desktop builds with wxWidgets
// ============================================================================

// Basic flat color shader (for 2D rendering and simple objects)
static const char* flatColorVertexShader = R"(#version 120

attribute vec3 aPosition;

uniform mat4 uMVPMatrix;

void main() {
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
}
)";

static const char* flatColorFragmentShader = R"(#version 120

uniform vec4 uColor;

void main() {
    gl_FragColor = uColor;
}
)";

// Phong lighting shader (for 3D rendering with lighting)
static const char* phongVertexShader = R"(#version 120

attribute vec3 aPosition;
attribute vec3 aNormal;

uniform mat4 uMVPMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

varying vec3 vNormal;
varying vec3 vFragPos;

void main() {
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
    vFragPos = vec3(uModelViewMatrix * vec4(aPosition, 1.0));
    vNormal = mat3(uNormalMatrix) * aNormal;
}
)";

static const char* phongFragmentShader = R"(#version 120

varying vec3 vNormal;
varying vec3 vFragPos;

uniform vec4 uColor;
uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
uniform float uAmbient;
uniform float uSpecularStrength;
uniform float uShininess;

void main() {
    vec3 ambient = uAmbient * uLightColor;
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    vec3 viewDir = normalize(uViewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    vec3 specular = uSpecularStrength * spec * uLightColor;
    vec3 result = (ambient + diffuse + specular) * uColor.rgb;
    gl_FragColor = vec4(result, uColor.a);
}
)";

// Simple 2D line shader (for pattern trails)
static const char* lineVertexShader = R"(#version 120

attribute vec2 aPosition;

uniform mat4 uMVPMatrix;

void main() {
    gl_Position = uMVPMatrix * vec4(aPosition, 0.0, 1.0);
}
)";

static const char* lineFragmentShader = R"(#version 120

uniform vec4 uColor;

void main() {
    gl_FragColor = uColor;
}
)";

#endif // USE_OPENGL_ES

#endif // SHADERS_H
