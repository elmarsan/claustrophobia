#pragma once

#include <math.h>
#include <cmath>

namespace claustrophobia
{
inline float radians(const float& deg) { return deg * M_PI / 180; }
inline float degress(const float& radians) { return radians * 180 / M_PI; }

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

    float angle(const vec3& r) const { return pow(cos(dot(r)) / magnitude() * r.magnitude(), -1); }

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

    float dot(const vec4& r) const { return x * r.x + y * r.y + z * r.z + w * r.w; }

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

    mat4 operator*(const mat4& r)
    {
        mat4 m{};

        const vec4 row0{col0.x, col1.x, col2.x, col3.x};
        const vec4 row1{col0.y, col1.y, col2.y, col3.y};
        const vec4 row2{col0.z, col1.z, col2.z, col3.z};
        const vec4 row3{col0.w, col1.w, col2.w, col3.w};

        m[0][0] = row0.dot(r.col0);
        m[1][0] = row0.dot(r.col1);
        m[2][0] = row0.dot(r.col2);
        m[3][0] = row0.dot(r.col3);

        m[0][1] = row1.dot(r.col0);
        m[1][1] = row1.dot(r.col1);
        m[2][1] = row1.dot(r.col2);
        m[3][1] = row1.dot(r.col3);

        m[0][2] = row2.dot(r.col0);
        m[1][2] = row2.dot(r.col1);
        m[2][2] = row2.dot(r.col2);
        m[3][2] = row2.dot(r.col3);

        m[0][3] = row3.dot(r.col0);
        m[1][3] = row3.dot(r.col1);
        m[2][3] = row3.dot(r.col2);
        m[3][3] = row3.dot(r.col3);

        return m;
    }

    vec4 col0;
    vec4 col1;
    vec4 col2;
    vec4 col3;
};

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

inline mat4 perspective(const float& fov, const float& aspectRatio, const float& zNear, const float& zFar)
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

inline mat4 lookAt(const vec3& eye, const vec3& target, const vec3& worldUp)
{
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

inline mat4 scale(const mat4& m, const vec3& v)
{
    mat4 s{m};
    s[0][0] = v.x;
    s[1][1] = v.y;
    s[2][2] = v.z;
    return s;
}
}  // namespace claustrophobia
