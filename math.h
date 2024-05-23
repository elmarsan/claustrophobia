#pragma once

#include <math.h>
#include <cmath>
#include <iostream>

namespace showcase
{
struct vec3
{
    vec3() : x(0), y(0), z(0) {}
    vec3(const float x, const float y, const float z) : x(x), y(y), z(z) {}

    vec3 cross(const vec3& r) const
    {
        return vec3{
            (y * r.z) - (z * r.y),
            (z * r.x) - (x * r.z),
            (x * r.y) - (y * r.x),
        };
    }

    float dot(const vec3& r) const { return x * r.x + y * r.y + z * r.z; }

    vec3 normalize() const { return *this / magnitude(); }

    float magnitude() const { return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); }

    vec3 operator*(const float& s) const { return vec3{x * s, y * s, z * s}; }

    void operator*=(const float& s)
    {
        x *= s;
        y *= s;
        z *= s;
    }

    vec3 operator*(const vec3& r) const { return vec3{x * r.x, y * r.y, z * r.z}; }

    void operator*=(const vec3& r)
    {
        x *= r.x;
        y *= r.y;
        z *= r.z;
    }

    vec3 operator-(const vec3& r) const { return vec3{x - r.x, y - r.y, z - r.z}; }
    void operator-=(const vec3& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;
    }

    vec3 operator+(const vec3& r) const { return vec3{x + r.x, y + r.y, z + r.z}; }
    void operator+=(const vec3& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;
    }

    vec3 operator/(const float& r) const { return vec3{x / r, y / r, z / r}; }

    float& operator[](const int i) { return (&x)[i]; }
    const float& operator[](const int i) const { return (&x)[i]; }

    friend vec3 operator*(const float& l, const vec3& r) { return vec3{r.x * l, r.y * l, r.z * l}; }

    float x;
    float y;
    float z;
};

struct vec4
{
    vec4() = default;
    vec4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}

    float& operator[](const int i) { return (&x)[i]; }
    const float& operator[](const int i) const { return (&x)[i]; }

    vec4 operator*(const vec4& r) const { return vec4{x * r.x, y * r.y, z * r.z, w * r.w}; }

    float x;
    float y;
    float z;
    float w;
};

struct mat4
{
    mat4() = default;
    mat4(const float diagonal)
        : col0{diagonal, 0.0f, 0.0f, 0.0f},
          col1{0.0f, diagonal, 0.0f, 0.0f},
          col2{0.0f, 0.0f, diagonal, 0.0f},
          col3{0.0f, 0.0f, 0.0f, diagonal}
    {
    }
    mat4(const mat4& r) : col0(r.col0), col1(r.col1), col2(r.col2), col3(r.col3) {}

    vec4& operator[](const int i) { return (&col0)[i]; }
    const vec4& operator[](const int i) const { return (&col0)[i]; }

    // Wrong!!!!!!!!!!!!!!!! Fix it.
    /* mat4 operator*(const mat4& rhs) const */
    /* { */
    /*     mat4 result; */

    /*     for (int i = 0; i < 4; ++i) */
    /*     { */
    /*         for (int j = 0; j < 4; ++j) */
    /*         { */
    /*             result[i][j] = col0[i] * rhs[0][j] + col1[i] * rhs[1][j] + col2[i] * rhs[2][j] + col3[i] * rhs[3][j]; */
    /*         } */
    /*     } */

    /*     return result; */
    /* } */

    vec4 col0;
    vec4 col1;
    vec4 col2;
    vec4 col3;
};

inline float radians(const float& deg) { return deg * M_PI / 180; }

inline mat4 translate(const mat4& m, const vec3& v)
{
    mat4 t{m};
    t[3][0] = v.x;
    t[3][1] = v.y;
    t[3][2] = v.z;
    return t;
}

inline mat4 rotate(const mat4& m, const float& a, const vec3& v)
{
    const auto c = cos(a);
    const auto s = sin(a);

    const auto r = v.normalize();
    const auto x = r.x;
    const auto y = r.y;
    const auto z = r.z;

    mat4 mr{m};

    mr[0][0] = (1 - c) * pow(x, 2) + c;
    mr[0][1] = (1 - c) * x * y + s * z;
    mr[0][2] = (1 - c) * x * z - s * y;

    mr[1][0] = (1 - c) * x * y - s * z;
    mr[1][1] = (1 - c) * pow(y, 2) + c;
    mr[1][2] = (1 - c) * y * z + s * x;

    mr[2][0] = (1 - c) * x * z + s * y;
    mr[2][1] = (1 - c) * y * z - s * x;
    mr[2][2] = (1 - c) * pow(z, 2) + c;

    return mr;
}

/* inline mat4 xFrustum(float fov, float aspectRatio, float zNear, float zFar) */
/* { */
/*     const float tanHalfFov = tan(fov / 2); */
/*     const float right = zNear * tanHalfFov; */
/*     const float top = right / aspectRatio; */

/*     mat4 f{1.0f}; */

/*     f[0][0] = zNear / right; */
/*     f[1][1] = zNear / top; */
/*     f[2][2] = -(zFar + zNear) / (zFar + zNear); */
/*     f[2][3] = -1; */
/*     f[3][2] = -(2 * zFar * zNear) / (zFar - zNear); */
/*     f[3][3] = 0; */

/*     return f; */
/* } */

inline mat4 perspective(const float fov, const float aspectRatio, const float zNear, const float zFar)
{
    mat4 p{0};

    const auto tanHalfFov = tan(fov / 2);

    p[0][0] = 1 / (aspectRatio * tanHalfFov);
    p[1][1] = 1 / tanHalfFov;
    p[2][2] = -(zFar + zNear) / (zFar - zNear);
    p[2][3] = -1;
    p[3][2] = -(2 * zFar * zNear) / (zFar - zNear);

    return p;
}

inline mat4 lookAt(const vec3 eye, const vec3 target, const vec3 worldUp)
{
    /* const vec3 forward = (target - eye).normalize(); */
    /* const vec3 xaxis = worldUp.normalize().cross(forward).normalize(); */
    /* const vec3 yaxis = forward.cross(xaxis); */

    /* mat4 r{1.0f}; */
    /* r[0][0] = xaxis.x; */
    /* r[0][1] = yaxis.x; */
    /* r[0][2] = forward.x; */

    /* r[1][0] = xaxis.y; */
    /* r[1][1] = yaxis.y; */
    /* r[1][2] = forward.y; */

    /* r[2][0] = xaxis.z; */
    /* r[2][1] = yaxis.z; */
    /* r[2][2] = forward.z; */

    /* mat4 t{1.0f}; */
    /* t[3][0] = -eye.x; */
    /* t[3][1] = -eye.y; */
    /* t[3][2] = -eye.z; */

    /* return r * t; */

    const auto forward = (eye - target).normalize();
    const auto left = worldUp.cross(forward).normalize();
    const auto up = forward.cross(left);

    mat4 lookAtm{1.0f};

    lookAtm[0][0] = left.x;
    lookAtm[0][1] = up.x;
    lookAtm[0][2] = forward.x;

    lookAtm[1][0] = left.y;
    lookAtm[1][1] = up.y;
    lookAtm[1][2] = forward.y;

    lookAtm[2][0] = left.z;
    lookAtm[2][1] = up.z;
    lookAtm[2][2] = forward.z;

    lookAtm[3][0] = -left.x * eye.x - left.y * eye.y - left.z * eye.z;
    lookAtm[3][1] = -up.x * eye.x - up.y * eye.y - up.z * eye.z;
    lookAtm[3][2] = -forward.x * eye.x - forward.y * eye.y - forward.z * eye.z;

    return lookAtm;
}
}  // namespace showcase
