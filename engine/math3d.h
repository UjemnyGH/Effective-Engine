#ifndef _EFFECTIVE_MATH3D_
#define _EFFECTIVE_MATH3D_

#include <math.h>

typedef float real_t; 

typedef struct vec4_s {
    real_t x, y, z, w;
} vec4_t;

vec4_t VAddV(vec4_t v1, vec4_t v2) { return (vec4_t){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w}; }
vec4_t VSubV(vec4_t v1, vec4_t v2) { return (vec4_t){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w}; }
vec4_t VMulV(vec4_t v1, vec4_t v2) { return (vec4_t){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w}; }
vec4_t VDivV(vec4_t v1, vec4_t v2) { return (vec4_t){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w}; }

vec4_t VAddR(vec4_t v1, real_t r) { return (vec4_t){v1.x + r, v1.y + r, v1.z + r, v1.w + r}; }
vec4_t VSubR(vec4_t v1, real_t r) { return (vec4_t){v1.x - r, v1.y - r, v1.z - r, v1.w - r}; }
vec4_t VMulR(vec4_t v1, real_t r) { return (vec4_t){v1.x * r, v1.y * r, v1.z * r, v1.w * r}; }
vec4_t VDivR(vec4_t v1, real_t r) { return (vec4_t){v1.x / r, v1.y / r, v1.z / r, v1.w / r}; }

real_t VDot(vec4_t v1, vec4_t v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w; }
vec4_t VCross(vec4_t v1, vec4_t v2) { return (vec4_t){v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x + v2.y - v1.y * v2.x, 0.0}; }
real_t VLength(vec4_t v) { return sqrt(VDot(v, v)); }
real_t VDistance(vec4_t v1, vec4_t v2) { return VLength(VSubV(v2, v1)); }
vec4_t VNormalize(vec4_t v) { return VMulR(v, 1.0 / VLength(v)); }

vec4_t VPlaneNormal(vec4_t a, vec4_t b, vec4_t c) { return VNormalize(VCross(VSubV(b, a), VSubV(c, a))); }

typedef struct mat4x4_s {
    real_t m[16];
} mat4x4_t, mat4_t;

mat4_t MX4Zero() {
    mat4_t result = {{0.0}};

    return result;
}

mat4_t MX4One() {
    mat4_t result = {{1.0}};

    return result;
}

mat4_t MX4Identity() {
    return (mat4_t){
        {
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        }
    };
}

mat4_t MX4RotateX(real_t radians) {
    mat4_t result = MX4Identity();

    result.m[5] = cos(radians);
    result.m[6] = -sin(radians);
    result.m[9] = sin(radians);
    result.m[10] = cos(radians);

    return result;
}

mat4_t MX4RotateY(real_t radians) {
    mat4_t result = MX4Identity();

    result.m[0] = cos(radians);
    result.m[2] = sin(radians);
    result.m[8] = -sin(radians);
    result.m[10] = cos(radians);

    return result;
}

mat4_t MX4RotateZ(real_t radians) {
    mat4_t result = MX4Identity();

    result.m[0] = cos(radians);
    result.m[1] = -sin(radians);
    result.m[4] = sin(radians);
    result.m[5] = cos(radians);

    return result;
}

mat4_t MX4Translate(vec4_t pos) {
    mat4_t result = MX4Identity();

    result.m[3] = pos.x;
    result.m[7] = pos.y;
    result.m[11] = pos.z;

    return result;
}

mat4_t MX4Scale(vec4_t scale) {
    mat4_t result = MX4Identity();

    result.m[0] = scale.x;
    result.m[5] = scale.y;
    result.m[10] = scale.z;

    return result;
}

mat4_t MX4Transpose(mat4_t m) {
    return (mat4_t){
        {
            m.m[0], m.m[4], m.m[8], m.m[12],
            m.m[1], m.m[5], m.m[9], m.m[13],
            m.m[2], m.m[6], m.m[10], m.m[14],
            m.m[3], m.m[7], m.m[11], m.m[15]
        }
    };
}

mat4_t MX4AddMX4(mat4_t m1, mat4_t m2) {
    mat4_t result;

    for(int i = 0; i < 16; i++) {
        result.m[i] = m1.m[i] + m2.m[i];
    }

    return result;
}

mat4_t MX4SubMX4(mat4_t m1, mat4_t m2) {
    mat4_t result;

    for(int i = 0; i < 16; i++) {
        result.m[i] = m1.m[i] - m2.m[i];
    }

    return result;
}

mat4_t MX4MulR(mat4_t m1, real_t r) {
    mat4_t result;

    for(int i = 0; i < 16; i++) {
        result.m[i] = m1.m[i] * r;
    }

    return result;
}

mat4_t MX4DivR(mat4_t m1, real_t r) {
    mat4_t result;

    for(int i = 0; i < 16; i++) {
        result.m[i] = m1.m[i] / r;
    }

    return result;
}

mat4_t MX4MulMX4(mat4_t m1, mat4_t m2) {
    mat4_t result;

    for(int j = 0; j < 4; j++) {
        for(int i = 0; i < 4; i++) {
            result.m[j * 4 + i] = VDot((vec4_t){m1.m[0 + i], m1.m[4 + i], m1.m[8 + i], m1.m[12 + i]}, (vec4_t){m2.m[4 * j], m2.m[4 * j + 1], m2.m[4 * j + 2], m2.m[4 * j + 3]});
        }
    }

    return result;
}

vec4_t MX4MulV(mat4_t m, vec4_t v) {
    return (vec4_t){
        VDot((vec4_t){m.m[0], m.m[1], m.m[2], m.m[3]}, v),
        VDot((vec4_t){m.m[4], m.m[5], m.m[6], m.m[7]}, v),
        VDot((vec4_t){m.m[8], m.m[9], m.m[10], m.m[11]}, v),
        VDot((vec4_t){m.m[12], m.m[13], m.m[14], m.m[15]}, v)
    };
}

vec4_t MX4ToDirection(mat4_t m, vec4_t v) {
    return (vec4_t){
        m.m[0] * v.x + m.m[1] * v.y + m.m[2] * v.z,
        m.m[4] * v.x + m.m[5] * v.y + m.m[6] * v.z,
        m.m[8] * v.x + m.m[9] * v.y + m.m[10] * v.z,
        1.0
    };
}

vec4_t MX4ToPoint(mat4_t m, vec4_t v) {
    vec4_t p = MX4ToDirection(m, v);

    real_t w = m.m[12] * v.x + m.m[13] * v.y + m.m[14] * v.z * m.m[15];

    return VDivR(p, w);
}

mat4_t MX4Inverse(mat4_t m) {
    mat4_t result;

    result.m[0]  =  m.m[5] * m.m[10] * m.m[15] - m.m[5] * m.m[11] * m.m[14] - m.m[9] * m.m[6] * m.m[15] + m.m[9] * m.m[7] * m.m[14] + m.m[13] * m.m[6] * m.m[11] - m.m[13] * m.m[7] * m.m[10];
    result.m[1]  = -m.m[1] * m.m[10] * m.m[15] + m.m[1] * m.m[11] * m.m[14] + m.m[9] * m.m[2] * m.m[15] - m.m[9] * m.m[3] * m.m[14] - m.m[13] * m.m[2] * m.m[11] + m.m[13] * m.m[3] * m.m[10];
    result.m[2]  =  m.m[1] * m.m[6]  * m.m[15] - m.m[1] * m.m[7]  * m.m[14] - m.m[5] * m.m[2] * m.m[15] + m.m[5] * m.m[3] * m.m[14] + m.m[13] * m.m[2] * m.m[7]  - m.m[13] * m.m[3] * m.m[6];
    result.m[3]  = -m.m[1] * m.m[6]  * m.m[11] + m.m[1] * m.m[7]  * m.m[10] + m.m[5] * m.m[2] * m.m[11] - m.m[5] * m.m[3] * m.m[10] - m.m[9]  * m.m[2] * m.m[7]  + m.m[9]  * m.m[3] * m.m[6];
    result.m[4]  = -m.m[4] * m.m[10] * m.m[15] + m.m[4] * m.m[11] * m.m[14] + m.m[8] * m.m[6] * m.m[15] - m.m[8] * m.m[7] * m.m[14] - m.m[12] * m.m[6] * m.m[11] + m.m[12] * m.m[7] * m.m[10];
    result.m[5]  =  m.m[0] * m.m[10] * m.m[15] - m.m[0] * m.m[11] * m.m[14] - m.m[8] * m.m[2] * m.m[15] + m.m[8] * m.m[3] * m.m[14] + m.m[12] * m.m[2] * m.m[11] - m.m[12] * m.m[3] * m.m[10];
    result.m[6]  = -m.m[0] * m.m[6]  * m.m[15] + m.m[0] * m.m[7]  * m.m[14] + m.m[4] * m.m[2] * m.m[15] - m.m[4] * m.m[3] * m.m[14] - m.m[12] * m.m[2] * m.m[7]  + m.m[12] * m.m[3] * m.m[6];
    result.m[7]  =  m.m[0] * m.m[6]  * m.m[11] - m.m[0] * m.m[7]  * m.m[10] - m.m[4] * m.m[2] * m.m[11] + m.m[4] * m.m[3] * m.m[10] + m.m[8]  * m.m[2] * m.m[7]  - m.m[8]  * m.m[3] * m.m[6];
    result.m[8]  =  m.m[4] * m.m[9]  * m.m[15] - m.m[4] * m.m[11] * m.m[13] - m.m[8] * m.m[5] * m.m[15] + m.m[8] * m.m[7] * m.m[13] + m.m[12] * m.m[5] * m.m[11] - m.m[12] * m.m[7] * m.m[9];
    result.m[9]  = -m.m[0] * m.m[9]  * m.m[15] + m.m[0] * m.m[11] * m.m[13] + m.m[8] * m.m[1] * m.m[15] - m.m[8] * m.m[3] * m.m[13] - m.m[12] * m.m[1] * m.m[11] + m.m[12] * m.m[3] * m.m[9];
    result.m[10] =  m.m[0] * m.m[5]  * m.m[15] - m.m[0] * m.m[7]  * m.m[13] - m.m[4] * m.m[1] * m.m[15] + m.m[4] * m.m[3] * m.m[13] + m.m[12] * m.m[1] * m.m[7]  - m.m[12] * m.m[3] * m.m[5];
    result.m[11] = -m.m[0] * m.m[5]  * m.m[11] + m.m[0] * m.m[7]  * m.m[9]  + m.m[4] * m.m[1] * m.m[11] - m.m[4] * m.m[3] * m.m[9]  -  m.m[8] * m.m[1] * m.m[7]  + m.m[8]  * m.m[3] * m.m[5];
    result.m[12] = -m.m[4] * m.m[9]  * m.m[14] + m.m[4] * m.m[10] * m.m[13] + m.m[8] * m.m[5] * m.m[14] - m.m[8] * m.m[6] * m.m[13] - m.m[12] * m.m[5] * m.m[10] + m.m[12] * m.m[6] * m.m[9];
    result.m[13] =  m.m[0] * m.m[9]  * m.m[14] - m.m[0] * m.m[10] * m.m[13] - m.m[8] * m.m[1] * m.m[14] + m.m[8] * m.m[2] * m.m[13] + m.m[12] * m.m[1] * m.m[10] - m.m[12] * m.m[2] * m.m[9];
    result.m[14] = -m.m[0] * m.m[5]  * m.m[14] + m.m[0] * m.m[6]  * m.m[13] + m.m[4] * m.m[1] * m.m[14] - m.m[4] * m.m[2] * m.m[13] - m.m[12] * m.m[1] * m.m[6]  + m.m[12] * m.m[2] * m.m[5];
    result.m[15] =  m.m[0] * m.m[5]  * m.m[10] - m.m[0] * m.m[6]  * m.m[9]  - m.m[4] * m.m[1] * m.m[10] + m.m[4] * m.m[2] * m.m[9]  +  m.m[8] * m.m[1] * m.m[6]  - m.m[8]  * m.m[2] * m.m[5];

    const real_t det = m.m[0] * result.m[0] + m.m[1] * result.m[4] + m.m[2] * result.m[8] + m.m[3] * result.m[12];

    MX4DivR(result, det);

    return result;
}

mat4_t MX4PerspectiveFOV(real_t fov, real_t width, real_t height, real_t zNear, real_t zFar) {
    mat4_t result;

    const real_t field = 1.0 / tan(fov / 2.0);

    result.m[0] = field * (height / width);
    result.m[5] = field;
    result.m[10] = -(zFar + zNear) / (zFar - zNear);
    result.m[11] = -1.0;
    result.m[11] = -(2.0 * zFar * zNear) / (zFar - zNear);
    result.m[15] = 1.0;

    return result;
}

mat4_t MX4Perspective(real_t right, real_t left, real_t top, real_t bottom, real_t zNear, real_t zFar) {
    mat4_t result;

    result.m[0] = (2.0 * zNear) / (right - left);
    result.m[2] = (right + left) / (right - left);
    result.m[5] = (2.0 * zNear) / (top - bottom);
    result.m[6] = (top + bottom) / (top - bottom);
    result.m[10] = -(zFar + zNear) / (zFar - zNear);
    result.m[11] = (-2.0 * zFar * zNear) / (zFar - zNear);
    result.m[14] = -1.0;

    return result;
}

mat4_t MX4PerspectiveSymmetrical(real_t right, real_t top, real_t zNear, real_t zFar) {
    mat4_t result;

    result.m[0] = zNear / right;
    result.m[5] = zNear / top;
    result.m[10] = -(zFar + zNear) / (zFar - zNear);
    result.m[11] = (-2.0 * zFar * zNear) / (zFar - zNear);
    result.m[14] = -1.0;

    return result;
}

mat4_t MX4Orthographic(real_t right, real_t left, real_t top, real_t bottom, real_t zNear, real_t zFar) {
    mat4_t result;

    result.m[0] = 2.0 / (right - left);
    result.m[3] = -(right + left) / (right - left);
    result.m[5] = 2.0 / (top - bottom);
    result.m[7] = -(top + bottom) / (top - bottom);
    result.m[10] = -2.0 / (zFar - zNear);
    result.m[11] = -(zFar + zNear) / (zFar - zNear);
    result.m[15] = 1.0;

    return result;
}

mat4_t MX4OrthographicSymmetrical(real_t right, real_t top, real_t zNear, real_t zFar) {
    mat4_t result;

    result.m[0] = 1.0 / right;
    result.m[5] = 1.0 / top;
    result.m[10] = -2.0 / (zFar - zNear);
    result.m[11] = -(zFar + zNear) / (zFar - zNear);
    result.m[15] = 1.0;

    return result;
}

mat4_t MX4LookAt(vec4_t eye, vec4_t at, vec4_t up) {
    vec4_t za = VNormalize(VSubV(at, eye));
    vec4_t xa = VNormalize(VCross(up, za));
    vec4_t ya = VCross(za, xa);

    return (mat4_t){{
        xa.x, ya.x, za.x, 0.0, 
        xa.y, ya.y, za.y, 0.0, 
        xa.z, ya.z, za.z, 0.0, 
        VDot(VMulR(xa, -1.0), eye), VDot(VMulR(ya, -1.0), eye), VDot(VMulR(za, -1.0), eye), 1.0, 
    }};
}

typedef struct Transform_s {
    vec4_t mPosition, mScale, mRotation;
    mat4_t mTransformMat;
} Transform_t;

void __TFRecalculateMatrix(Transform_t* pTrans) {
    pTrans->mTransformMat = MX4MulMX4(MX4MulMX4(MX4MulMX4(MX4MulMX4(MX4Scale(pTrans->mScale), MX4Translate(pTrans->mPosition)), MX4RotateX(pTrans->mRotation.x)), MX4RotateY(pTrans->mRotation.y)), MX4RotateZ(pTrans->mRotation.z));
}

void TFSetPosition(Transform_t* pTrans, vec4_t v) {
    pTrans->mPosition = v;

    __TFRecalculateMatrix(pTrans);
}

void TFSetRotation(Transform_t* pTrans, vec4_t v) {
    pTrans->mRotation = v;

    __TFRecalculateMatrix(pTrans);
}

void TFSetScale(Transform_t* pTrans, vec4_t v) {
    pTrans->mScale = v;

    __TFRecalculateMatrix(pTrans);
}

#endif