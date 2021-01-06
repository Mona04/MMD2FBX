#include "Framework.h"
#include "Quaternion.h"
#include "Math.h"

const Quaternion Quaternion::QuaternionFromAngleAxis(const float& angle, const Vector3& axis)
{
    float half = angle * 0.5f;
    float sin = sinf(half);
    float cos = cosf(half);

    return Quaternion
    (
        axis.x * sin,
        axis.y * sin,
        axis.z * sin,
        cos
    ).Normalize();
}

const Quaternion Quaternion::QuaternionFromYawPitchRoll_ZYX(const float& yaw, const float& pitch, const float& roll)
{
    double halfRoll = roll * 0.5;
    double halfPitch = pitch * 0.5;
    double halfYaw = yaw * 0.5;

    double sinRoll = sin(halfRoll);
    double cosRoll = cos(halfRoll);
    double sinPitch = sin(halfPitch);
    double cosPitch = cos(halfPitch);
    double sinYaw = sin(halfYaw);
    double cosYaw = cos(halfYaw);

    return Quaternion
    (
        sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw,
        cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
        cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw,
        cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw
    ).Normalize();
}

const Quaternion Quaternion::QuaternionFromYawPitchRoll_XZY(const float& yaw, const float& pitch, const float& roll)
{
    double halfRoll = roll * 0.5;
    double halfPitch = pitch * 0.5;
    double halfYaw = yaw * 0.5;

    double sinRoll = sin(halfRoll);
    double cosRoll = cos(halfRoll);
    double sinPitch = sin(halfPitch);
    double cosPitch = cos(halfPitch);
    double sinYaw = sin(halfYaw);
    double cosYaw = cos(halfYaw);

    return Quaternion
    (
        (+cosPitch * cosYaw * sinRoll + sinPitch * sinYaw * cosRoll),
        (+cosPitch * sinYaw * sinRoll + sinPitch * cosYaw * cosRoll),
        (-sinPitch * cosYaw * sinRoll + cosPitch * sinYaw * cosRoll),
        (+cosPitch * cosYaw * cosRoll - sinPitch * sinYaw * sinRoll)
    ).Normalize();
}


const Quaternion Quaternion::QuaternionFromEulerAngle(const float& x, const float& y, const float& z)
{
    return QuaternionFromYawPitchRoll_XZY
    (
        Math::ToRadian(z),
        Math::ToRadian(y),
        Math::ToRadian(x)
    );
}

const Vector3 Quaternion::QuaternionToYawPitchRoll_ZYX(const Quaternion& q)
{
    // Derivation from http://www.geometrictools.com/Documentation/EulerAngles.pdf
    float check = 2.0f * (q.y * q.w - q.z * q.x);

    if (check < -0.995f)
    {
        return Vector3
        (
            atan2f(2.0f * (q.x * q.w + q.z * q.y), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)) * 180 / Math::pi,
            -90.0f,
            0
        );
    }

    if (check > 0.995f)
    {
        return Vector3
        (
            atan2f(2.0f * (q.x * q.w + q.z * q.y), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)) * 180 / Math::pi,
            +90.0f,
            0
        );
    }

    return Vector3
    (
        atan2f(2.0f * (q.x * q.w + q.z * q.y), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)) * 180 / Math::pi,
        asinf(check) * 180 / Math::pi,
        atan2f(2.0f * (q.x * q.y + q.w * q.z), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)) * 180 / Math::pi
    );
}

const Vector3 Quaternion::QuaternionToYawPitchRoll_XZY(const Quaternion& q)
{
    // Derivation from http://www.geometrictools.com/Documentation/EulerAngles.pdf

    float check = 2.0f * (q.x * q.y + q.w * q.z);

    if (check < -0.995f)
    {
        return Vector3
        (
            atan2f(2.0f * (q.x * q.w - q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.z * q.z)) * 180 / Math::pi,
            0,
            -90
        );
    }

    if (check > 0.995f)
    {
        return Vector3
        (
            atan2f(2.0f * (q.x * q.w - q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.z * q.z)) * 180 / Math::pi,
            0,
            90
        );
    }
    
    return Vector3
    (
        atan2f(2.0f * (q.x * q.w - q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.z * q.z)) * 180 / Math::pi,
        atan2f(2.0f * (q.y * q.w - q.x * q.z), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)) * 180 / Math::pi,
        asinf(check) * 180 / Math::pi
    );
}

const Quaternion Quaternion::QuaternionToReverse(const Quaternion& q)
{
    //float inverseLength = 1.0f / static_cast<float>(sqrt(rhs.LengthSq()));

    return Quaternion
    (
        -q.x, //* inverseLength,
        -q.y, //* inverseLength,
        -q.z, //* inverseLength,
        +q.w  //* inverseLength
    );
}

Quaternion::Quaternion() : x(0), y(0), z(0), w(1)
{
}

Quaternion::Quaternion(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w)
{
}

bool Quaternion::operator==(const Quaternion& rhs) const
{
    return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

bool Quaternion::operator!=(const Quaternion& rhs) const
{
    return !(*this == rhs);
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
    return Quaternion
    (
        ((y * rhs.z) - (z * rhs.y)) + (x * rhs.w) + (w * rhs.x),
        (-(x * rhs.z) + (z * rhs.x)) + (y * rhs.w) + (w * rhs.y),
        ((x * rhs.y) - (y * rhs.x)) + (z * rhs.w) + (w * rhs.z),
        (w * rhs.w) - ((x * rhs.x) + (y * rhs.y) + (z * rhs.z))
    ).Normalize();
}

Vector3 Quaternion::operator*(const Vector3& rhs) const
{
	return Matrix::QuaternionToMatrix(*this) * rhs;
}

const Vector3 Quaternion::ToEulerAngle() const
{
    return QuaternionToYawPitchRoll_XZY(*this);
}

const Quaternion Quaternion::Normalize() const
{
    float inverseLength = 1.0f / static_cast<float>(sqrt(LengthSq()));

    return Quaternion
    (
        x * inverseLength,
        y * inverseLength,
        z * inverseLength,
        w * inverseLength
    );
}

const Quaternion Quaternion::Inverse() const
{
    float inverseLength = 1.0f / static_cast<float>(sqrt(LengthSq()));

    return Quaternion
    (
        -x * inverseLength,
        -y * inverseLength,
        -z * inverseLength,
        w * inverseLength
    );
}

const Quaternion Quaternion::QuaternionFromEulerAngle(const Vector3& rotation)
{
    return QuaternionFromYawPitchRoll_XZY
    (
        Math::ToRadian(rotation.z),
        Math::ToRadian(rotation.y),
        Math::ToRadian(rotation.x)
    );
}
