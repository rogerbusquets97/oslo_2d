#pragma once

/*
    Math utils, most of them from https://github.com/MrFrenik/gunslinger
*/

#include "defines.h"
#include <math.h>       // floor, acos, sin, sqrt, tan

// Defines
#define EPSILON  (1e-6)
#define PI       3.1415926535897932
#define TAU      2.0 * PI

// Useful Utility
#define v2(...)  vec2_ctor(__VA_ARGS__)
#define v3(...)  vec3_ctor(__VA_ARGS__)
#define v4(...)  vec4_ctor(__VA_ARGS__)
#define quat(...) quat_ctor(__VA_ARGS__)

#define v2s(__S)  vec2_ctor((__S), (__S))
#define v3s(__S)  vec3_ctor((__S), (__S), (__S))
#define v4s(__S)  vec4_ctor((__S), (__S), (__S), (__S))

#define v4_xy_v(__X, __Y, __V) vec4_ctor((__X), (__Y), (__V).x, (__V).y)
#define v4_xyz_s(__XYZ, __S) vec4_ctor((__XYZ).x, (__XYZ).y, (__XYZ).z, (__S))

#define XAXIS    v3(1.f, 0.f, 0.f)
#define YAXIS    v3(0.f, 1.f, 0.f)
#define ZAXIS    v3(0.f, 0.f, 1.f)

/*================================================================================
// Useful Common Math Functions
================================================================================*/

#define rad2deg(__R)\
    (float)((__R * 180.0f) / PI) 

#define deg2rad(__D)\
    (float)((__D * PI) / 180.0f)

// Interpolation
// Source: https://codeplea.com/simple-interpolation

oslo_inline float
interp_linear(float a, float b, float t)
{
    return (a + t * (b - a));
}

oslo_inline float
interp_smoothstep(float a, float b, float t)
{
    return interp_linear(a, b, t * t * (3.0f - 2.0f * t));
}

oslo_inline float 
interp_cosine(float a, float b, float t)
{
    return interp_linear(a, b, (float)-cos(PI * t) * 0.5f + 0.5f);
}

oslo_inline float 
interp_acceleration(float a, float b, float t) 
{
    return interp_linear(a, b, t * t);
}

oslo_inline float 
interp_deceleration(float a, float b, float t) 
{
    return interp_linear(a, b, 1.0f - (1.0f - t) * (1.0f - t));
}

oslo_inline float 
round_value(float val) 
{
    return (float)floor(val + 0.5f);
}

oslo_inline float
map_range(float input_start, float input_end, float output_start, float output_end, float val)
{
    float slope = (output_end - output_start) / (input_end - input_start);
    return (output_start + (slope * (val - input_start)));
}

// Easings from: https://github.com/raysan5/raylib/blob/ea0f6c7a26f3a61f3be542aa8f066ce033766a9f/examples/others/easings.h
oslo_inline
float ease_cubic_in(float t, float b, float c, float d) 
{ 
    t /= d; 
    return (c*t*t*t + b); 
}

oslo_inline
float ease_cubic_out(float t, float b, float c, float d) 
{ 
    t = t/d - 1.0f; 
    return (c*(t*t*t + 1.0f) + b); 
}

oslo_inline
float ease_cubic_in_out(float t, float b, float c, float d)
{
    if ((t/=d/2.0f) < 1.0f) 
    {
        return (c/2.0f*t*t*t + b);
    }
    t -= 2.0f; 
    return (c/2.0f*(t*t*t + 2.0f) + b);
}

/*================================================================================
// Vec2
================================================================================*/

/** @brief struct vec2 in gs math */
typedef struct 
{
    union 
    {
        f32 xy[2];
        struct 
        {
            f32 x, y;
        };
    };
} vec2_t;

typedef vec2_t vec2;

oslo_inline vec2 
vec2_ctor(f32 _x, f32 _y) 
{
    vec2 v;
    v.x = _x;
    v.y = _y;
    return v;
}

oslo_inline vec2 
vec2_add(vec2 v0, vec2 v1) 
{
    return vec2_ctor(v0.x + v1.x, v0.y + v1.y);
}

oslo_inline vec2 
vec2_sub(vec2 v0, vec2 v1)
{
    return vec2_ctor(v0.x - v1.x, v0.y - v1.y);
}

oslo_inline vec2 
vec2_mul(vec2 v0, vec2 v1) 
{
    return vec2_ctor(v0.x * v1.x, v0.y * v1.y);
}

oslo_inline vec2 
vec2_div(vec2 v0, vec2 v1) 
{
    return vec2_ctor(v0.x / v1.x, v0.y / v1.y);
}

oslo_inline bool
vec2_equals(vec2 v0, vec2 v1)
{
    return (v0.x == v1.x && v0.y == v1.y);
}

oslo_inline vec2 
vec2_scale(vec2 v, f32 s)
{
    return vec2_ctor(v.x * s, v.y * s);
}

oslo_inline f32 
vec2_dot(vec2 v0, vec2 v1) 
{
    return (f32)(v0.x * v1.x + v0.y * v1.y);
}

oslo_inline f32 
vec2_len(vec2 v)
{
    return (f32)sqrt(vec2_dot(v, v));
}

oslo_inline vec2
vec2_project_onto(vec2 v0, vec2 v1)
{
    f32 dot = vec2_dot(v0, v1);
    f32 len = vec2_dot(v1, v1);

    // Orthogonal, so return v1
    if (len == 0.f) return v1;

    return vec2_scale(v1, dot / len);
}

oslo_inline vec2 vec2_norm(vec2 v) 
{
    f32 len = vec2_len(v);
    return vec2_scale(v, len != 0.f ? 1.0f / vec2_len(v) : 1.f);
}

oslo_inline 
f32 vec2_dist(vec2 a, vec2 b)
{
    f32 dx = (a.x - b.x);
    f32 dy = (a.y - b.y);
    return (float)(sqrt(dx * dx + dy * dy));
}

oslo_inline
f32 vec2_cross(vec2 a, vec2 b) 
{
    return a.x * b.y - a.y * b.x;
}

oslo_inline
f32 vec2_angle(vec2 a, vec2 b) 
{
    return (float)acos(vec2_dot(a, b) / (vec2_len(a) * vec2_len(b)));
}

oslo_inline
b32 vec2_equal(vec2 a, vec2 b)
{
    return (a.x == b.x && a.y == b.y);
}

/*================================================================================
// Vec3
================================================================================*/

typedef struct
{
    union 
    {
        f32 xyz[3];
        struct 
        {
            f32 x, y, z;
        };
    }; 

} vec3_t;

typedef vec3_t vec3;

oslo_inline vec3 
vec3_ctor(f32 _x, f32 _y, f32 _z)
{
    vec3 v;
    v.x = _x;
    v.y = _y;
    v.z = _z;
    return v;
}

oslo_inline bool 
vec3_eq(vec3 v0, vec3 v1)
{
    return (v0.x == v1.x && v0.y == v1.y && v0.z == v1.z);
}

oslo_inline vec3 
vec3_add(vec3 v0, vec3 v1)
{
    return vec3_ctor(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
}

oslo_inline vec3 
vec3_sub(vec3 v0, vec3 v1) 
{
    return vec3_ctor(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
}

oslo_inline vec3 
vec3_mul(vec3 v0, vec3 v1) 
{
    return vec3_ctor(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z);
}

oslo_inline vec3 
vec3_div(vec3 v0, vec3 v1) 
{
    return vec3_ctor(v0.x / v1.x, v0.y / v1.y, v0.z / v1.z);
}

oslo_inline vec3 
vec3_scale(vec3 v, f32 s) 
{
    return vec3_ctor(v.x * s, v.y * s, v.z * s);
}

oslo_inline vec3
vec3_neg(vec3 v)
{
    return vec3_scale(v, -1.f);
}

oslo_inline f32 
vec3_dot(vec3 v0, vec3 v1) 
{
    f32 dot = (f32)((v0.x * v1.x) + (v0.y * v1.y) + v0.z * v1.z);
    return dot;
}

oslo_inline bool 
vec3_same_dir(vec3 v0, vec3 v1)
{
    return (vec3_dot(v0, v1) > 0.f);
}

oslo_inline vec3 
vec3_sign(vec3 v)
{
    return (vec3_ctor(
        v.x < 0.f ? -1.f : v.x > 0.f ? 1.f : 0.f,
        v.y < 0.f ? -1.f : v.y > 0.f ? 1.f : 0.f,
        v.z < 0.f ? -1.f : v.z > 0.f ? 1.f : 0.f
    ));
}

oslo_inline float 
vec3_signX(vec3 v)
{
    return (v.x < 0.f ? -1.f : v.x > 0.f ? 1.f : 0.f);
}

oslo_inline float 
vec3_signY(vec3 v)
{
    return (v.y < 0.f ? -1.f : v.y > 0.f ? 1.f : 0.f);
}

oslo_inline float 
vec3_signZ(vec3 v)
{
    return (v.z < 0.f ? -1.f : v.z > 0.f ? 1.f : 0.f);
}

oslo_inline f32 
vec3_len(vec3 v)
{
    return (f32)sqrt(vec3_dot(v, v));
}

oslo_inline f32 
vec3_len2(vec3 v)
{
    return (f32)(vec3_dot(v, v));
}

oslo_inline vec3
vec3_project_onto(vec3 v0, vec3 v1)
{
    f32 dot = vec3_dot(v0, v1);
    f32 len = vec3_dot(v1, v1);

    // Orthogonal, so return v1
    if (len == 0.f) return v1;

    return vec3_scale(v1, dot / len);
}

oslo_inline bool
vec3_nan(vec3 v)
{
    if (v.x != v.x || v.y != v.y || v.z != v.z) return true;
    return false; 
}

oslo_inline
f32 vec3_dist2(vec3 a, vec3 b)
{ 
    f32 dx = (a.x - b.x);
    f32 dy = (a.y - b.y);
    f32 dz = (a.z - b.z);
    return (dx * dx + dy * dy + dz * dz);
} 

oslo_inline 
f32 vec3_dist(vec3 a, vec3 b)
{
    return sqrt(vec3_dist2(a, b));
}

oslo_inline vec3 
vec3_norm(vec3 v)
{
    f32 len = vec3_len(v);
    return len == 0.f ? v : vec3_scale(v, 1.f / len);
}

oslo_inline vec3 
vec3_cross(vec3 v0, vec3 v1) 
{
    return vec3_ctor
    (
        v0.y * v1.z - v0.z * v1.y,
        v0.z * v1.x - v0.x * v1.z,
        v0.x * v1.y - v0.y * v1.x
    );
}

oslo_inline void vec3_scale_ip(vec3* vp, f32 s)
{
    vp->x *= s;
    vp->y *= s;
    vp->z *= s;
}

oslo_inline float vec3_angle_between(vec3 v0, vec3 v1)
{
    return acosf(vec3_dot(v0, v1));
}

oslo_inline float vec3_angle_between_signed(vec3 v0, vec3 v1)
{
    return asinf(vec3_len(vec3_cross(v0, v1)));
}

oslo_inline vec3 vec3_triple_cross_product(vec3 a, vec3 b, vec3 c)
{
    return vec3_sub((vec3_scale(b, vec3_dot(c, a))), (vec3_scale(a, vec3_dot(c, b))));
}

/*================================================================================
// Vec4
================================================================================*/

typedef struct
{
    union 
    {
        f32 xyzw[4];
        struct 
        {
            f32 x, y, z, w;
        };
    };
} vec4_t;

typedef vec4_t vec4;

oslo_inline vec4 
vec4_ctor(f32 _x, f32 _y, f32 _z, f32 _w)
{
    vec4 v; 
    v.x = _x;
    v.y = _y; 
    v.z = _z; 
    v.w = _w;
    return v;
} 

oslo_inline vec4
vec4_add(vec4 v0, vec4 v1) 
{
    return vec4_ctor(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w);
}

oslo_inline vec4
vec4_sub(vec4 v0, vec4 v1) 
{
    return vec4_ctor(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w);
}

oslo_inline vec4
vec4_mul(vec4 v0, vec4 v1) 
{
    return vec4_ctor(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z, v0.w * v1.w);
}

oslo_inline vec4
vec4_div(vec4 v0, vec4 v1) 
{
    return vec4_ctor(v0.x / v1.x, v0.y / v1.y, v0.z / v1.z, v0.w / v1.w);
}

oslo_inline vec4
vec4_scale(vec4 v, f32 s) 
{
    return vec4_ctor(v.x * s, v.y * s, v.z * s, v.w * s);
}

oslo_inline f32
vec4_dot(vec4 v0, vec4 v1) 
{
    return (f32)(v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w);
}

oslo_inline f32
vec4_len(vec4 v) 
{
    return (f32)sqrt(vec4_dot(v, v));
}

oslo_inline vec4
vec4_project_onto(vec4 v0, vec4 v1)
{
    f32 dot = vec4_dot(v0, v1);
    f32 len = vec4_dot(v1, v1);

    // Orthogonal, so return v1
    if (len == 0.f) return v1;

    return vec4_scale(v1, dot / len);
}

oslo_inline vec4
vec4_norm(vec4 v) 
{
    return vec4_scale(v, 1.0f / vec4_len(v));
}

oslo_inline f32
vec4_dist(vec4 v0, vec4 v1)
{
    f32 dx = (v0.x - v1.x);
    f32 dy = (v0.y - v1.y);
    f32 dz = (v0.z - v1.z);
    f32 dw = (v0.w - v1.w);
    return (float)(sqrt(dx * dx + dy * dy + dz * dz + dw * dw));
}

/*================================================================================
// Useful Vector Functions
================================================================================*/

oslo_inline
vec3 v4_to_v3(vec4 v) 
{
    return v3(v.x, v.y, v.z);
}

oslo_inline
vec2 v3_to_v2(vec3 v) 
{
    return v2(v.x, v.y);
}

/*================================================================================
// Mat4x4
================================================================================*/

/*
    Matrices are stored in linear, contiguous memory and assume a column-major ordering.
*/

typedef struct mat4
{
	union {
		vec4 rows[4];
        float m[4][4];
		float elements[16]; 
        struct {
            vec4 right, up, dir, position;
        } v;
	};
} mat4_t;

typedef mat4_t mat4;

oslo_inline mat4 
mat4_diag(f32 val)
{
    mat4 m;
    memset(m.elements, 0, sizeof(m.elements));
    m.elements[0 + 0 * 4] = val;
    m.elements[1 + 1 * 4] = val;
    m.elements[2 + 2 * 4] = val;
    m.elements[3 + 3 * 4] = val;
    return m;
}

#define mat4_identity()\
    mat4_diag(1.0f)

oslo_inline mat4
mat4_ctor() {
    mat4 mat = default_val();
    return mat;
}

oslo_inline
mat4 mat4_elem(const float* elements)
{
    mat4 mat = mat4_ctor();
    memcpy(mat.elements, elements, sizeof(f32) * 16);
    return mat;
}

oslo_inline mat4 
mat4_mul(mat4 m0, mat4 m1)
{
    mat4 m_res = mat4_ctor(); 
    for (u32 y = 0; y < 4; ++y)
    {
        for (u32 x = 0; x < 4; ++x)
        {
            f32 sum = 0.0f;
            for (u32 e = 0; e < 4; ++e)
            {
                sum += m0.elements[x + e * 4] * m1.elements[e + y * 4];
            }
            m_res.elements[x + y * 4] = sum;
        }
    }

    return m_res;
}

oslo_inline 
mat4 mat4_mul_list(uint32_t count, ...)
{
    va_list ap;
    mat4 m = mat4_identity();
    va_start(ap, count);
    for (uint32_t i = 0; i < count; ++i) {
        m = mat4_mul(m, va_arg(ap, mat4));
    }
    va_end(ap);
    return m;
}

oslo_inline
void mat4_set_elements(mat4* m, float* elements, uint32_t count)
{
    for (u32 i = 0; i < count; ++i)
    {
        m->elements[i] = elements[i];
    }
}

oslo_inline
mat4 mat4_ortho_norm(const mat4* m)
{
    mat4 r = *m;
    r.v.right = vec4_norm(r.v.right);
    r.v.up = vec4_norm(r.v.up);
    r.v.dir = vec4_norm(r.v.dir);
    return r;
} 

oslo_inline
mat4 mat4_transpose(mat4 m)
{
    mat4 t = mat4_identity();

    // First row
    t.elements[0 * 4 + 0] = m.elements[0 * 4 + 0];
    t.elements[1 * 4 + 0] = m.elements[0 * 4 + 1];
    t.elements[2 * 4 + 0] = m.elements[0 * 4 + 2];
    t.elements[3 * 4 + 0] = m.elements[0 * 4 + 3];

    // Second row
    t.elements[0 * 4 + 1] = m.elements[1 * 4 + 0];
    t.elements[1 * 4 + 1] = m.elements[1 * 4 + 1];
    t.elements[2 * 4 + 1] = m.elements[1 * 4 + 2];
    t.elements[3 * 4 + 1] = m.elements[1 * 4 + 3];

    // Third row
    t.elements[0 * 4 + 2] = m.elements[2 * 4 + 0];
    t.elements[1 * 4 + 2] = m.elements[2 * 4 + 1];
    t.elements[2 * 4 + 2] = m.elements[2 * 4 + 2];
    t.elements[3 * 4 + 2] = m.elements[2 * 4 + 3];

    // Fourth row
    t.elements[0 * 4 + 3] = m.elements[3 * 4 + 0];
    t.elements[1 * 4 + 3] = m.elements[3 * 4 + 1];
    t.elements[2 * 4 + 3] = m.elements[3 * 4 + 2];
    t.elements[3 * 4 + 3] = m.elements[3 * 4 + 3];

    return t;
}

oslo_inline
mat4 mat4_inverse(mat4 m)
{
    mat4 res = mat4_identity();

    f32 temp[16];

    temp[0] = m.elements[5] * m.elements[10] * m.elements[15] -
        m.elements[5] * m.elements[11] * m.elements[14] -
        m.elements[9] * m.elements[6] * m.elements[15] +
        m.elements[9] * m.elements[7] * m.elements[14] +
        m.elements[13] * m.elements[6] * m.elements[11] -
        m.elements[13] * m.elements[7] * m.elements[10];

    temp[4] = -m.elements[4] * m.elements[10] * m.elements[15] +
        m.elements[4] * m.elements[11] * m.elements[14] +
        m.elements[8] * m.elements[6] * m.elements[15] -
        m.elements[8] * m.elements[7] * m.elements[14] -
        m.elements[12] * m.elements[6] * m.elements[11] +
        m.elements[12] * m.elements[7] * m.elements[10];

    temp[8] = m.elements[4] * m.elements[9] * m.elements[15] -
        m.elements[4] * m.elements[11] * m.elements[13] -
        m.elements[8] * m.elements[5] * m.elements[15] +
        m.elements[8] * m.elements[7] * m.elements[13] +
        m.elements[12] * m.elements[5] * m.elements[11] -
        m.elements[12] * m.elements[7] * m.elements[9];

    temp[12] = -m.elements[4] * m.elements[9] * m.elements[14] +
        m.elements[4] * m.elements[10] * m.elements[13] +
        m.elements[8] * m.elements[5] * m.elements[14] -
        m.elements[8] * m.elements[6] * m.elements[13] -
        m.elements[12] * m.elements[5] * m.elements[10] +
        m.elements[12] * m.elements[6] * m.elements[9];

    temp[1] = -m.elements[1] * m.elements[10] * m.elements[15] +
        m.elements[1] * m.elements[11] * m.elements[14] +
        m.elements[9] * m.elements[2] * m.elements[15] -
        m.elements[9] * m.elements[3] * m.elements[14] -
        m.elements[13] * m.elements[2] * m.elements[11] +
        m.elements[13] * m.elements[3] * m.elements[10];

    temp[5] = m.elements[0] * m.elements[10] * m.elements[15] -
        m.elements[0] * m.elements[11] * m.elements[14] -
        m.elements[8] * m.elements[2] * m.elements[15] +
        m.elements[8] * m.elements[3] * m.elements[14] +
        m.elements[12] * m.elements[2] * m.elements[11] -
        m.elements[12] * m.elements[3] * m.elements[10];

    temp[9] = -m.elements[0] * m.elements[9] * m.elements[15] +
        m.elements[0] * m.elements[11] * m.elements[13] +
        m.elements[8] * m.elements[1] * m.elements[15] -
        m.elements[8] * m.elements[3] * m.elements[13] -
        m.elements[12] * m.elements[1] * m.elements[11] +
        m.elements[12] * m.elements[3] * m.elements[9];

    temp[13] = m.elements[0] * m.elements[9] * m.elements[14] -
        m.elements[0] * m.elements[10] * m.elements[13] -
        m.elements[8] * m.elements[1] * m.elements[14] +
        m.elements[8] * m.elements[2] * m.elements[13] +
        m.elements[12] * m.elements[1] * m.elements[10] -
        m.elements[12] * m.elements[2] * m.elements[9];

    temp[2] = m.elements[1] * m.elements[6] * m.elements[15] -
        m.elements[1] * m.elements[7] * m.elements[14] -
        m.elements[5] * m.elements[2] * m.elements[15] +
        m.elements[5] * m.elements[3] * m.elements[14] +
        m.elements[13] * m.elements[2] * m.elements[7] -
        m.elements[13] * m.elements[3] * m.elements[6];

    temp[6] = -m.elements[0] * m.elements[6] * m.elements[15] +
        m.elements[0] * m.elements[7] * m.elements[14] +
        m.elements[4] * m.elements[2] * m.elements[15] -
        m.elements[4] * m.elements[3] * m.elements[14] -
        m.elements[12] * m.elements[2] * m.elements[7] +
        m.elements[12] * m.elements[3] * m.elements[6];

    temp[10] = m.elements[0] * m.elements[5] * m.elements[15] -
        m.elements[0] * m.elements[7] * m.elements[13] -
        m.elements[4] * m.elements[1] * m.elements[15] +
        m.elements[4] * m.elements[3] * m.elements[13] +
        m.elements[12] * m.elements[1] * m.elements[7] -
        m.elements[12] * m.elements[3] * m.elements[5];

    temp[14] = -m.elements[0] * m.elements[5] * m.elements[14] +
        m.elements[0] * m.elements[6] * m.elements[13] +
        m.elements[4] * m.elements[1] * m.elements[14] -
        m.elements[4] * m.elements[2] * m.elements[13] -
        m.elements[12] * m.elements[1] * m.elements[6] +
        m.elements[12] * m.elements[2] * m.elements[5];

    temp[3] = -m.elements[1] * m.elements[6] * m.elements[11] +
        m.elements[1] * m.elements[7] * m.elements[10] +
        m.elements[5] * m.elements[2] * m.elements[11] -
        m.elements[5] * m.elements[3] * m.elements[10] -
        m.elements[9] * m.elements[2] * m.elements[7] +
        m.elements[9] * m.elements[3] * m.elements[6];

    temp[7] = m.elements[0] * m.elements[6] * m.elements[11] -
        m.elements[0] * m.elements[7] * m.elements[10] -
        m.elements[4] * m.elements[2] * m.elements[11] +
        m.elements[4] * m.elements[3] * m.elements[10] +
        m.elements[8] * m.elements[2] * m.elements[7] -
        m.elements[8] * m.elements[3] * m.elements[6];

    temp[11] = -m.elements[0] * m.elements[5] * m.elements[11] +
        m.elements[0] * m.elements[7] * m.elements[9] +
        m.elements[4] * m.elements[1] * m.elements[11] -
        m.elements[4] * m.elements[3] * m.elements[9] -
        m.elements[8] * m.elements[1] * m.elements[7] +
        m.elements[8] * m.elements[3] * m.elements[5];

    temp[15] = m.elements[0] * m.elements[5] * m.elements[10] -
        m.elements[0] * m.elements[6] * m.elements[9] -
        m.elements[4] * m.elements[1] * m.elements[10] +
        m.elements[4] * m.elements[2] * m.elements[9] +
        m.elements[8] * m.elements[1] * m.elements[6] -
        m.elements[8] * m.elements[2] * m.elements[5];

    float determinant = m.elements[0] * temp[0] + m.elements[1] * temp[4] + m.elements[2] * temp[8] + m.elements[3] * temp[12];
    determinant = 1.0f / determinant;

    for (int i = 0; i < 4 * 4; i++)
        res.elements[i] = (float)(temp[i] * (float)determinant);

    return res;
}

/*
    f32 l : left
    f32 r : right
    f32 b : bottom
    f32 t : top
    f32 n : near
    f32 f : far
*/
oslo_inline mat4 
mat4_ortho(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f)
{
    mat4 m_res = mat4_identity();     

    // Main diagonal
    m_res.elements[0 + 0 * 4] = 2.0f / (r - l);
    m_res.elements[1 + 1 * 4] = 2.0f / (t - b);
    m_res.elements[2 + 2 * 4] = -2.0f / (f - n);

    // Last column
    m_res.elements[0 + 3 * 4] = -(r + l) / (r - l);
    m_res.elements[1 + 3 * 4] = -(t + b) / (t - b);
    m_res.elements[2 + 3 * 4] = -(f + n) / (f - n);

    return m_res;
}

oslo_inline mat4 
mat4_perspective(f32 fov, f32 asp_ratio, f32 n, f32 f)
{
    // Zero matrix
    mat4 m_res = mat4_ctor();

    f32 q = 1.0f / (float)tan(deg2rad(0.5f * fov));
    f32 a = q / asp_ratio;
    f32 b = (n + f) / (n - f);
    f32 c = (2.0f * n * f) / (n - f);

    m_res.elements[0 + 0 * 4] = a;
    m_res.elements[1 + 1 * 4] = q;
    m_res.elements[2 + 2 * 4] = b;
    m_res.elements[2 + 3 * 4] = c;
    m_res.elements[3 + 2 * 4] = -1.0f;

    return m_res;
}

oslo_inline mat4 
mat4_translatev(const vec3 v)
{
    mat4 m_res = mat4_identity();

    m_res.elements[0 + 4 * 3] = v.x;
    m_res.elements[1 + 4 * 3] = v.y;
    m_res.elements[2 + 4 * 3] = v.z;

    return m_res;
}

oslo_inline mat4 
mat4_translate(float x, float y, float z)
{
    return mat4_translatev(v3(x, y, z));
}

oslo_inline mat4 
mat4_scalev(const vec3 v)
{
    mat4 m_res = mat4_identity();
    m_res.elements[0 + 0 * 4] = v.x;
    m_res.elements[1 + 1 * 4] = v.y;
    m_res.elements[2 + 2 * 4] = v.z;
    return m_res;
}

oslo_inline mat4
mat4_scale(float x, float y, float z)
{
    return (mat4_scalev(v3(x, y, z)));
}

// Assumes normalized axis
oslo_inline mat4 
mat4_rotatev(float angle, vec3 axis)
{
    mat4 m_res = mat4_identity();

    float a = angle;
    float c = (float)cos(a);
    float s = (float)sin(a);

    vec3 naxis = vec3_norm(axis);
    float x = naxis.x;
    float y = naxis.y;
    float z = naxis.z;

    //First column
    m_res.elements[0 + 0 * 4] = x * x * (1 - c) + c;    
    m_res.elements[1 + 0 * 4] = x * y * (1 - c) + z * s;    
    m_res.elements[2 + 0 * 4] = x * z * (1 - c) - y * s;    
    
    //Second column
    m_res.elements[0 + 1 * 4] = x * y * (1 - c) - z * s;    
    m_res.elements[1 + 1 * 4] = y * y * (1 - c) + c;    
    m_res.elements[2 + 1 * 4] = y * z * (1 - c) + x * s;    
    
    //Third column
    m_res.elements[0 + 2 * 4] = x * z * (1 - c) + y * s;    
    m_res.elements[1 + 2 * 4] = y * z * (1 - c) - x * s;    
    m_res.elements[2 + 2 * 4] = z * z * (1 - c) + c;    

    return m_res;
}

oslo_inline mat4
mat4_rotate(float angle, float x, float y, float z)
{
    return mat4_rotatev(angle, v3(x, y, z));
}

oslo_inline mat4 
mat4_look_at(vec3 position, vec3 target, vec3 up)
{
    vec3 f = vec3_norm(vec3_sub(target, position));
    vec3 s = vec3_norm(vec3_cross(f, up));
    vec3 u = vec3_cross(s, f);

    mat4 m_res = mat4_identity();
    m_res.elements[0 * 4 + 0] = s.x;
    m_res.elements[1 * 4 + 0] = s.y;
    m_res.elements[2 * 4 + 0] = s.z;

    m_res.elements[0 * 4 + 1] = u.x;
    m_res.elements[1 * 4 + 1] = u.y;
    m_res.elements[2 * 4 + 1] = u.z;

    m_res.elements[0 * 4 + 2] = -f.x;
    m_res.elements[1 * 4 + 2] = -f.y;
    m_res.elements[2 * 4 + 2] = -f.z;

    m_res.elements[3 * 4 + 0] = -vec3_dot(s, position);;
    m_res.elements[3 * 4 + 1] = -vec3_dot(u, position);
    m_res.elements[3 * 4 + 2] = vec3_dot(f, position); 

    return m_res;
}

// Modified from github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp

oslo_inline
void mat4_decompose(const mat4* m, float* translation, float* rotation, float* scale)
{
    mat4 mat = *m;

    scale[0] = vec4_len(mat.v.right);
    scale[1] = vec4_len(mat.v.up);
    scale[2] = vec4_len(mat.v.dir); 

    mat = mat4_ortho_norm(&mat);

    rotation[0] = rad2deg(atan2f(mat.m[1][2], mat.m[2][2]));
    rotation[1] = rad2deg(atan2f(-mat.m[0][2], sqrtf(mat.m[1][2] * mat.m[1][2] + 
                mat.m[2][2] * mat.m[2][2])));
    rotation[2] = rad2deg(atan2f(mat.m[0][1], mat.m[0][0]));

    translation[0] = mat.v.position.x;
    translation[1] = mat.v.position.y;
    translation[2] = mat.v.position.z;
}

// Modified from github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp

oslo_inline
mat4 mat4_recompose(const float* translation, const float* rotation, const float* scale)
{
    mat4 mat = mat4_identity();

    vec3 direction_unary[3] = {
        XAXIS, 
        YAXIS, 
        ZAXIS
    };

    mat4 rot[3] = {mat4_identity(), mat4_identity(), mat4_identity()};
    for (uint32_t i = 0; i < 3; ++i) {
        rot[i] = mat4_rotatev(deg2rad(rotation[i]), direction_unary[i]);
    }

    mat = mat4_mul_list(3, rot[2], rot[1], rot[0]);

    float valid_scale[3] = default_val();
    for (uint32_t i = 0; i < 3; ++i) {
        valid_scale[i] = fabsf(scale[i]) < EPSILON ? 0.001f : scale[i];
    }

    mat.v.right = vec4_scale(mat.v.right, valid_scale[0]);
    mat.v.up = vec4_scale(mat.v.up, valid_scale[1]);
    mat.v.dir = vec4_scale(mat.v.dir, valid_scale[2]);
    mat.v.position = v4(translation[0], translation[1], translation[2], 1.f);

    return mat; 
}


oslo_inline
vec4 mat4_mul_vec4(mat4 m, vec4 v)
{
    return vec4_ctor
    (
        m.elements[0 + 4 * 0] * v.x + m.elements[0 + 4 * 1] * v.y + m.elements[0 + 4 * 2] * v.z + m.elements[0 + 4 * 3] * v.w,  
        m.elements[1 + 4 * 0] * v.x + m.elements[1 + 4 * 1] * v.y + m.elements[1 + 4 * 2] * v.z + m.elements[1 + 4 * 3] * v.w,  
        m.elements[2 + 4 * 0] * v.x + m.elements[2 + 4 * 1] * v.y + m.elements[2 + 4 * 2] * v.z + m.elements[2 + 4 * 3] * v.w,  
        m.elements[3 + 4 * 0] * v.x + m.elements[3 + 4 * 1] * v.y + m.elements[3 + 4 * 2] * v.z + m.elements[3 + 4 * 3] * v.w
    );
}

oslo_inline
vec3 mat4_mul_vec3(mat4 m, vec3 v)
{
    return v4_to_v3(mat4_mul_vec4(m, v4_xyz_s(v, 1.f)));
    // return v4_to_v3(v4);
    // return vec3_ctor
    // (
    //     m.elements[0 + 4 * 0] * v.x + m.elements[0 + 4 * 1] * v.y + m.elements[0 + 4 * 2] * v.z,  
    //     m.elements[1 + 4 * 0] * v.x + m.elements[1 + 4 * 1] * v.y + m.elements[1 + 4 * 2] * v.z,  
    //     m.elements[2 + 4 * 0] * v.x + m.elements[2 + 4 * 1] * v.y + m.elements[2 + 4 * 2] * v.z
    // );
}

/*================================================================================
// Quaternion
================================================================================*/

typedef struct
{
    union 
    {
        struct {
            union {
                vec3 xyz;
                vec3 axis;
            } axis;
            float a;
        } aa;
        vec4 v;
        f32 xyzw[4];
        struct 
        {
            f32 x, y, z, w;
        };
    };
} quat_t;

typedef quat_t quat;

oslo_inline
quat quat_default()
{
    quat q;
    q.x = 0.f;  
    q.y = 0.f;  
    q.z = 0.f;  
    q.w = 1.f;  
    return q;
}

oslo_inline
quat quat_ctor(f32 _x, f32 _y, f32 _z, f32 _w)
{
    quat q;
    q.x = _x;
    q.y = _y;
    q.z = _z;
    q.w = _w;
    return q;
}

oslo_inline quat 
quat_add(quat q0, quat q1) 
{
    return quat_ctor(q0.x + q1.x, q0.y + q1.y, q0.z + q1.z, q0.w + q1.w);
}

oslo_inline quat 
quat_sub(quat q0, quat q1)
{
    return quat_ctor(q0.x - q1.x, q0.y - q1.y, q0.z - q1.z, q0.w - q1.w);
}

oslo_inline quat
quat_mul(quat q0, quat q1)
{
    return quat_ctor(
        q0.w * q1.x + q1.w * q0.x + q0.y * q1.z - q1.y * q0.z,
        q0.w * q1.y + q1.w * q0.y + q0.z * q1.x - q1.z * q0.x,
        q0.w * q1.z + q1.w * q0.z + q0.x * q1.y - q1.x * q0.y,
        q0.w * q1.w - q0.x * q1.x - q0.y * q1.y - q0.z * q1.z
    );
}

oslo_inline 
quat quat_mul_list(u32 count, ...)
{
    va_list ap;
    quat q = quat_default();
    va_start(ap, count);
    for (u32 i = 0; i < count; ++i)
    {
        q = quat_mul(q, va_arg(ap, quat));
    }
    va_end(ap);
    return q;
}

oslo_inline quat 
quat_mul_quat(quat q0, quat q1)
{
    return quat_ctor(
        q0.w * q1.x + q1.w * q0.x + q0.y * q1.z - q1.y * q0.z,
        q0.w * q1.y + q1.w * q0.y + q0.z * q1.x - q1.z * q0.x,
        q0.w * q1.z + q1.w * q0.z + q0.x * q1.y - q1.x * q0.y,
        q0.w * q1.w - q0.x * q1.x - q0.y * q1.y - q0.z * q1.z
    );
}

oslo_inline 
quat quat_scale(quat q, f32 s)
{
    return quat_ctor(q.x * s, q.y * s, q.z * s, q.w * s);
}

oslo_inline f32 
quat_dot(quat q0, quat q1)
{
    return (f32)(q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w);
}

oslo_inline 
quat quat_conjugate(quat q)
{
    return (quat_ctor(-q.x, -q.y, -q.z, q.w));
}

oslo_inline f32
quat_len(quat q)
{
    return (f32)sqrt(quat_dot(q, q));
}

oslo_inline quat
quat_norm(quat q) 
{
    return quat_scale(q, 1.0f / quat_len(q));
}

oslo_inline quat
quat_cross(quat q0, quat q1)
{
    return quat_ctor (                                           
        q0.x * q1.x + q0.x * q1.w + q0.y * q1.z - q0.z * q1.y,  
        q0.w * q1.y + q0.y * q1.w + q0.z * q1.x - q0.x * q1.z,  
        q0.w * q1.z + q0.z * q1.w + q0.x * q1.y - q0.y * q1.x,  
        q0.w * q1.w - q0.x * q1.x - q0.y * q1.y - q0.z * q1.z   
    );
}

// Inverse := Conjugate / Dot;
oslo_inline
quat quat_inverse(quat q)
{
    return (quat_scale(quat_conjugate(q), 1.0f / quat_dot(q, q)));
} 

oslo_inline quat 
quat_angle_axis(f32 rad, vec3 axis)
{
    // Normalize axis
    vec3 a = vec3_norm(axis);

    // Get scalar
    f32 half_angle = 0.5f * rad;
    f32 s = (float)sin(half_angle);

    return quat_ctor(a.x * s, a.y * s, a.z * s, (float)cos(half_angle));
}

oslo_inline vec3 
quat_rotate(quat q, vec3 v)
{
    // nVidia SDK implementation
    vec3 qvec = vec3_ctor(q.x, q.y, q.z);
    vec3 uv = vec3_cross(qvec, v);
    vec3 uuv = vec3_cross(qvec, uv);
    uv = vec3_scale(uv, 2.f * q.w);
    uuv = vec3_scale(uuv, 2.f);
    return (vec3_add(v, vec3_add(uv, uuv)));
}

oslo_inline vec3
quat_forward(quat q)
{
    return quat_rotate(q, v3(0.f, 0.f, -1.f));
}

oslo_inline vec3
quat_backward(quat q)
{
    return quat_rotate(q, v3(0.f, 0.f, 1.f));
}

oslo_inline vec3
quat_left(quat q)
{
    return quat_rotate(q, v3(-1.f, 0.f, 0.f));
}

oslo_inline vec3
quat_right(quat q)
{
    return quat_rotate(q, v3(1.f, 0.f, 0.f));
}

oslo_inline vec3
quat_up(quat q)
{
    return quat_rotate(q, v3(0.f, 1.f, 0.f));
}

oslo_inline vec3
quat_down(quat q)
{
    return quat_rotate(q, v3(0.f, -1.f, 0.f));
}

oslo_inline quat 
quat_from_to_rotation(vec3 src, vec3 dst)
{
    src = vec3_norm(src);
    dst = vec3_norm(dst);
    const float d = vec3_dot(src, dst);

    if (d  >= 1.f)
    {
        return quat_default();
    }
    else if (d <= -1.f)
    {
        // Orthonormalize, find axis of rotation
        vec3 axis = vec3_cross(src, XAXIS);
        if (vec3_len2(axis) < 1e-6)
        {
            axis = vec3_cross(src, YAXIS);
        }
        return quat_angle_axis((float)PI, vec3_norm(axis));
    } 
    else
    {
        const float s = sqrtf(vec3_len2(src) * vec3_len2(dst)) + 
            vec3_dot(src, dst);

        vec3 axis = vec3_cross(src, dst);

        return quat_norm(quat_ctor(axis.x, axis.y, axis.z, s));
    }
}

oslo_inline 
quat quat_look_rotation(vec3 position, vec3 target, vec3 up)
{ 
    const vec3 forward = vec3_norm(vec3_sub(position, target));
    const quat q0 = quat_from_to_rotation(ZAXIS, forward);
    if (vec3_len2(vec3_cross(forward, up)) < 1e-6)
    {
        return q0;
    } 

    const vec3 new_up = quat_rotate(q0, up);
    const quat q1 = quat_from_to_rotation(new_up, up);

    return quat_mul(q1, q0);
}

oslo_inline
quat quat_slerp(quat a, quat b, f32 t)
{
    f32 c = quat_dot(a, b);
    quat end = b;

    if (c < 0.0f)
    {
        // Reverse all signs
        c *= -1.0f;
        end.x *= -1.0f;
        end.y *= -1.0f;
        end.z *= -1.0f;
        end.w *= -1.0f;
    }

    // Calculate coefficients
    f32 sclp, sclq;
    if ((1.0f - c) > 0.0001f)
    {
        f32 omega = (float)acosf(c);
        f32 s = (float)sinf(omega);
        sclp = (float)sinf((1.0f - t) * omega) / s;
        sclq = (float)sinf(t * omega) / s; 
    }
    else
    {
        sclp = 1.0f - t;
        sclq = t;
    }

    quat q;
    q.x = sclp * a.x + sclq * end.x;
    q.y = sclp * a.y + sclq * end.y;
    q.z = sclp * a.z + sclq * end.z;
    q.w = sclp * a.w + sclq * end.w;

    return q;
}

#define quat_axis_angle(__AXS, __ANG)\
    quat_angle_axis(__ANG, __AXS)

/*
* @brief Convert given quaternion param into equivalent 4x4 rotation matrix
* @note: From http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm 
*/
oslo_inline mat4 quat_to_mat4(quat _q)
{
    mat4 mat = mat4_identity();
    quat q = quat_norm(_q);

    f32 xx = q.x * q.x; 
    f32 yy = q.y * q.y; 
    f32 zz = q.z * q.z; 
    f32 xy = q.x * q.y;
    f32 xz = q.x * q.z;
    f32 yz = q.y * q.z;
    f32 wx = q.w * q.x;
    f32 wy = q.w * q.y;
    f32 wz = q.w * q.z;

    mat.elements[0 * 4 + 0] = 1.0f - 2.0f * (yy + zz);
    mat.elements[1 * 4 + 0] = 2.0f * (xy - wz);
    mat.elements[2 * 4 + 0] = 2.0f * (xz + wy);

    mat.elements[0 * 4 + 1] = 2.0f * (xy + wz);
    mat.elements[1 * 4 + 1] = 1.0f - 2.0f * (xx + zz);
    mat.elements[2 * 4 + 1] = 2.0f * (yz - wx);

    mat.elements[0 * 4 + 2] = 2.0f * (xz - wy);
    mat.elements[1 * 4 + 2] = 2.0f * (yz + wx);
    mat.elements[2 * 4 + 2] = 1.0f - 2.0f * (xx + yy);

    return mat;
}

oslo_inline 
quat quat_from_euler(f32 yaw_deg, f32 pitch_deg, f32 roll_deg)
{
    f32 yaw = deg2rad(yaw_deg);
    f32 pitch = deg2rad(pitch_deg);
    f32 roll = deg2rad(roll_deg);

    quat q;
    f32 cy = (float)cos(yaw * 0.5f);
    f32 sy = (float)sin(yaw * 0.5f);
    f32 cr = (float)cos(roll * 0.5f);
    f32 sr = (float)sin(roll * 0.5f);
    f32 cp = (float)cos(pitch * 0.5f);
    f32 sp = (float)sin(pitch * 0.5f);

    q.x = cy * sr * cp - sy * cr * sp;
    q.y = cy * cr * sp + sy * sr * cp;
    q.z = sy * cr * cp - cy * sr * sp;
    q.w = cy * cr * cp + sy * sr * sp;

    return q;
}

oslo_inline
float quat_pitch(quat* q)
{
    return atan2(2.0f * q->y * q->z + q->w * q->x, q->w * q->w - q->x * q->x - q->y * q->y + q->z * q->z);
}

oslo_inline
float quat_yaw(quat* q)
{
    return asin(-2.0f * (q->x * q->z - q->w * q->y));
}

oslo_inline
float quat_roll(quat* q)
{
    return atan2(2.0f * q->x * q->y +  q->z * q->w,  q->x * q->x + q->w * q->w - q->y * q->y - q->z * q->z);
}

oslo_inline
vec3 quat_to_euler(quat* q)
{
    return v3(quat_yaw(q), quat_pitch(q), quat_roll(q));
}