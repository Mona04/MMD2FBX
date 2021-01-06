#pragma once

class Quaternion final
{
public:

    static const Quaternion QuaternionFromAngleAxis(const float& angle, const class Vector3& axis);
    static const Quaternion QuaternionFromYawPitchRoll_ZYX(const float& yaw, const float& pitch, const float& roll);
    static const Quaternion QuaternionFromYawPitchRoll_XZY(const float& yaw, const float& pitch, const float& roll);

    static const Quaternion QuaternionFromEulerAngle(const class Vector3& rotation);
    static const Quaternion QuaternionFromEulerAngle(const float& x, const float& y, const float& z);
    static const Vector3 QuaternionToYawPitchRoll_ZYX(const Quaternion& quaternion);
    static const Vector3 QuaternionToYawPitchRoll_XZY(const Quaternion& quaternion);

    static const Quaternion QuaternionToReverse(const Quaternion& rotation);

public:
    Quaternion();
    Quaternion(float x, float y, float z, float w);
    ~Quaternion() = default;

    bool operator==(const Quaternion& rhs) const;
    bool operator!=(const Quaternion& rhs) const;
    Quaternion operator*(const Quaternion& rhs) const;
    Vector3 operator*(const Vector3& rhs) const;

public:
    const class Vector3 ToEulerAngle() const;

    const Vector3 GetAxis() const { return Vector3(x, y, z).Normalize(); }
    const float LengthSq() const { return x * x + y * y + z * z + w * w; }
    const Quaternion Conjugate()  const { return Quaternion(-x, -y, -z, w); }

    const Quaternion Normalize() const;
    const Quaternion Inverse() const;

public:
    float x;
    float y;
    float z;
    float w;
};