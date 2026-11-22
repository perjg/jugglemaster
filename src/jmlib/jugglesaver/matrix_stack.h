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

#ifndef MATRIX_STACK_H
#define MATRIX_STACK_H

#include <vector>
#include <cmath>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 4x4 Matrix in column-major order (OpenGL convention)
struct Matrix4 {
    float m[16];

    Matrix4() {
        identity();
    }

    void identity() {
        memset(m, 0, sizeof(m));
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    // Matrix multiplication: result = this * other
    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 result;
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                result.m[i*4 + j] = 0.0f;
                for(int k = 0; k < 4; k++) {
                    result.m[i*4 + j] += m[k*4 + j] * other.m[i*4 + k];
                }
            }
        }
        return result;
    }

    Matrix4 transpose() const {
        Matrix4 result;
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                result.m[i*4 + j] = m[j*4 + i];
            }
        }
        return result;
    }

    // Calculate inverse of 4x4 matrix
    // Returns identity if singular
    Matrix4 inverse() const {
        float inv[16], det;
        int i;

        inv[0] = m[5]  * m[10] * m[15] -
                 m[5]  * m[11] * m[14] -
                 m[9]  * m[6]  * m[15] +
                 m[9]  * m[7]  * m[14] +
                 m[13] * m[6]  * m[11] -
                 m[13] * m[7]  * m[10];

        inv[4] = -m[4]  * m[10] * m[15] +
                  m[4]  * m[11] * m[14] +
                  m[8]  * m[6]  * m[15] -
                  m[8]  * m[7]  * m[14] -
                  m[12] * m[6]  * m[11] +
                  m[12] * m[7]  * m[10];

        inv[8] = m[4]  * m[9] * m[15] -
                 m[4]  * m[11] * m[13] -
                 m[8]  * m[5] * m[15] +
                 m[8]  * m[7] * m[13] +
                 m[12] * m[5] * m[11] -
                 m[12] * m[7] * m[9];

        inv[12] = -m[4]  * m[9] * m[14] +
                   m[4]  * m[10] * m[13] +
                   m[8]  * m[5] * m[14] -
                   m[8]  * m[6] * m[13] -
                   m[12] * m[5] * m[10] +
                   m[12] * m[6] * m[9];

        inv[1] = -m[1]  * m[10] * m[15] +
                  m[1]  * m[11] * m[14] +
                  m[9]  * m[2] * m[15] -
                  m[9]  * m[3] * m[14] -
                  m[13] * m[2] * m[11] +
                  m[13] * m[3] * m[10];

        inv[5] = m[0]  * m[10] * m[15] -
                 m[0]  * m[11] * m[14] -
                 m[8]  * m[2] * m[15] +
                 m[8]  * m[3] * m[14] +
                 m[12] * m[2] * m[11] -
                 m[12] * m[3] * m[10];

        inv[9] = -m[0]  * m[9] * m[15] +
                  m[0]  * m[11] * m[13] +
                  m[8]  * m[1] * m[15] -
                  m[8]  * m[3] * m[13] -
                  m[12] * m[1] * m[11] +
                  m[12] * m[3] * m[9];

        inv[13] = m[0]  * m[9] * m[14] -
                  m[0]  * m[10] * m[13] -
                  m[8]  * m[1] * m[14] +
                  m[8]  * m[2] * m[13] +
                  m[12] * m[1] * m[10] -
                  m[12] * m[2] * m[9];

        inv[2] = m[1]  * m[6] * m[15] -
                 m[1]  * m[7] * m[14] -
                 m[5]  * m[2] * m[15] +
                 m[5]  * m[3] * m[14] +
                 m[13] * m[2] * m[7] -
                 m[13] * m[3] * m[6];

        inv[6] = -m[0]  * m[6] * m[15] +
                  m[0]  * m[7] * m[14] +
                  m[4]  * m[2] * m[15] -
                  m[4]  * m[3] * m[14] -
                  m[12] * m[2] * m[7] +
                  m[12] * m[3] * m[6];

        inv[10] = m[0]  * m[5] * m[15] -
                  m[0]  * m[7] * m[13] -
                  m[4]  * m[1] * m[15] +
                  m[4]  * m[3] * m[13] +
                  m[12] * m[1] * m[7] -
                  m[12] * m[3] * m[5];

        inv[14] = -m[0]  * m[5] * m[14] +
                   m[0]  * m[6] * m[13] +
                   m[4]  * m[1] * m[14] -
                   m[4]  * m[2] * m[13] -
                   m[12] * m[1] * m[6] +
                   m[12] * m[2] * m[5];

        inv[3] = -m[1] * m[6] * m[11] +
                  m[1] * m[7] * m[10] +
                  m[5] * m[2] * m[11] -
                  m[5] * m[3] * m[10] -
                  m[9] * m[2] * m[7] +
                  m[9] * m[3] * m[6];

        inv[7] = m[0] * m[6] * m[11] -
                 m[0] * m[7] * m[10] -
                 m[4] * m[2] * m[11] +
                 m[4] * m[3] * m[10] +
                 m[8] * m[2] * m[7] -
                 m[8] * m[3] * m[6];

        inv[11] = -m[0] * m[5] * m[11] +
                   m[0] * m[7] * m[9] +
                   m[4] * m[1] * m[11] -
                   m[4] * m[3] * m[9] -
                   m[8] * m[1] * m[7] +
                   m[8] * m[3] * m[5];

        inv[15] = m[0] * m[5] * m[10] -
                  m[0] * m[6] * m[9] -
                  m[4] * m[1] * m[10] +
                  m[4] * m[2] * m[9] +
                  m[8] * m[1] * m[6] -
                  m[8] * m[2] * m[5];

        det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

        if (det == 0)
            return Matrix4(); // Identity

        det = 1.0 / det;

        Matrix4 result;
        for (i = 0; i < 16; i++)
            result.m[i] = inv[i] * det;

        return result;
    }
};

// Matrix stack for emulating glPushMatrix/glPopMatrix
class MatrixStack {
private:
    std::vector<Matrix4> stack;

public:
    MatrixStack() {
        stack.push_back(Matrix4()); // Initialize with identity
    }

    void push() {
        stack.push_back(stack.back());
    }

    void pop() {
        if(stack.size() > 1) {
            stack.pop_back();
        }
    }

    void loadIdentity() {
        stack.back().identity();
    }

    Matrix4& top() {
        return stack.back();
    }

    const Matrix4& top() const {
        return stack.back();
    }

    void multiply(const Matrix4& mat) {
        stack.back() = stack.back() * mat;
    }

    // Transform operations
    void translate(float x, float y, float z) {
        Matrix4 mat;
        mat.m[12] = x;
        mat.m[13] = y;
        mat.m[14] = z;
        multiply(mat);
    }

    void scale(float x, float y, float z) {
        Matrix4 mat;
        mat.m[0] = x;
        mat.m[5] = y;
        mat.m[10] = z;
        multiply(mat);
    }

    void rotate(float angleDeg, float x, float y, float z) {
        float angle = angleDeg * M_PI / 180.0f;
        float c = cosf(angle);
        float s = sinf(angle);

        // Normalize axis
        float len = sqrtf(x*x + y*y + z*z);
        if(len < 0.0001f) return;
        x /= len; y /= len; z /= len;

        Matrix4 mat;
        mat.m[0] = x*x*(1-c) + c;
        mat.m[1] = y*x*(1-c) + z*s;
        mat.m[2] = x*z*(1-c) - y*s;
        mat.m[3] = 0;

        mat.m[4] = x*y*(1-c) - z*s;
        mat.m[5] = y*y*(1-c) + c;
        mat.m[6] = y*z*(1-c) + x*s;
        mat.m[7] = 0;

        mat.m[8] = x*z*(1-c) + y*s;
        mat.m[9] = y*z*(1-c) - x*s;
        mat.m[10] = z*z*(1-c) + c;
        mat.m[11] = 0;

        mat.m[12] = 0;
        mat.m[13] = 0;
        mat.m[14] = 0;
        mat.m[15] = 1;

        multiply(mat);
    }
};

// Projection matrix functions
inline Matrix4 perspective(float fovyDeg, float aspect, float znear, float zfar) {
    Matrix4 mat;
    mat.identity();

    float fovy = fovyDeg * M_PI / 180.0f;
    float f = 1.0f / tanf(fovy / 2.0f);

    mat.m[0] = f / aspect;
    mat.m[5] = f;
    mat.m[10] = (zfar + znear) / (znear - zfar);
    mat.m[11] = -1.0f;
    mat.m[14] = (2.0f * zfar * znear) / (znear - zfar);
    mat.m[15] = 0.0f;

    return mat;
}

inline Matrix4 ortho(float left, float right, float bottom, float top, float znear, float zfar) {
    Matrix4 mat;
    mat.identity();

    mat.m[0] = 2.0f / (right - left);
    mat.m[5] = 2.0f / (top - bottom);
    mat.m[10] = -2.0f / (zfar - znear);
    mat.m[12] = -(right + left) / (right - left);
    mat.m[13] = -(top + bottom) / (top - bottom);
    mat.m[14] = -(zfar + znear) / (zfar - znear);

    return mat;
}

inline Matrix4 lookAt(float eyeX, float eyeY, float eyeZ,
                     float centerX, float centerY, float centerZ,
                     float upX, float upY, float upZ) {
    // Forward vector
    float fx = centerX - eyeX;
    float fy = centerY - eyeY;
    float fz = centerZ - eyeZ;
    float flen = sqrtf(fx*fx + fy*fy + fz*fz);
    fx /= flen; fy /= flen; fz /= flen;

    // Side vector = forward x up
    float sx = fy * upZ - fz * upY;
    float sy = fz * upX - fx * upZ;
    float sz = fx * upY - fy * upX;
    float slen = sqrtf(sx*sx + sy*sy + sz*sz);
    sx /= slen; sy /= slen; sz /= slen;

    // Recompute up = side x forward
    float ux = sy * fz - sz * fy;
    float uy = sz * fx - sx * fz;
    float uz = sx * fy - sy * fx;

    Matrix4 mat;
    mat.m[0] = sx;
    mat.m[4] = sy;
    mat.m[8] = sz;
    mat.m[12] = 0;

    mat.m[1] = ux;
    mat.m[5] = uy;
    mat.m[9] = uz;
    mat.m[13] = 0;

    mat.m[2] = -fx;
    mat.m[6] = -fy;
    mat.m[10] = -fz;
    mat.m[14] = 0;

    mat.m[3] = 0;
    mat.m[7] = 0;
    mat.m[11] = 0;
    mat.m[15] = 1;

    // Translate
    Matrix4 trans;
    trans.m[12] = -eyeX;
    trans.m[13] = -eyeY;
    trans.m[14] = -eyeZ;

    return mat * trans;
}

#endif // MATRIX_STACK_H
